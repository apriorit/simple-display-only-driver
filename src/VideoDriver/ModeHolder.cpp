#include "stdafx.h"
#include "ModeHolder.h"
#include "SourceModeSetCreator.h"
#include "TargetModeSetCreator.h"
#include "MonitorModeSet.h"
#include "ISourceMode.h"
#include "Conversions.h"

namespace
{
    // There is only one source format supported by display-only drivers, but more can be added in a 
    // full WDDM driver if the hardware supports them
    const D3DDDIFORMAT g_format = D3DDDIFMT_A8R8G8B8;

    void FillVideoSignalInfo(D3DKMDT_VIDEO_SIGNAL_INFO& video, const Mode& mode)
    {
        D3DKMDT_2DREGION size = {mode.width, mode.height};
        D3DDDI_RATIONAL freq = {D3DKMDT_FREQUENCY_NOTSPECIFIED, D3DKMDT_FREQUENCY_NOTSPECIFIED};

        video.VideoStandard = D3DKMDT_VSS_OTHER;
        video.TotalSize = size;
        video.ActiveSize = size;
        video.VSyncFreq = freq;
        video.HSyncFreq = freq;
        video.PixelRate = D3DKMDT_FREQUENCY_NOTSPECIFIED;
        video.ScanLineOrdering = D3DDDI_VSSLO_PROGRESSIVE;
    }

    void FillMode(SourceModeSet::Mode& setMode, const Mode& mode)
    {
        unsigned pitch = BytesFromPixelFormat(g_format) * mode.width;

        setMode->Type = D3DKMDT_RMT_GRAPHICS;

        D3DKMDT_GRAPHICS_RENDERING_FORMAT& graphics = setMode->Format.Graphics;
        D3DKMDT_2DREGION size = {mode.width, mode.height};

        graphics.PrimSurfSize = size;
        graphics.VisibleRegionSize = size;
        graphics.Stride = pitch;
        graphics.PixelFormat = g_format;
        graphics.ColorBasis = D3DKMDT_CB_SCRGB;
        graphics.PixelValueAccessMode = D3DKMDT_PVAM_DIRECT;
    }

    void FillMode(TargetModeSet::Mode& setMode, const Mode& mode)
    {
        FillVideoSignalInfo(setMode->VideoSignalInfo, mode);

        // We add this as PREFERRED since it is the only supported target
        setMode->Preference = D3DKMDT_MP_PREFERRED;
    }

    void FillMode(MonitorModeSet::Mode& setMode, const Mode& mode)
    {
        FillVideoSignalInfo(setMode->VideoSignalInfo, mode);

        D3DKMDT_COLOR_COEFF_DYNAMIC_RANGES ranges = {8, 8, 8, 8};

        setMode->Origin = D3DKMDT_MCO_DRIVER;
        setMode->Preference = D3DKMDT_MP_PREFERRED;
        setMode->ColorBasis = D3DKMDT_CB_SRGB;
        setMode->ColorCoeffDynamicRanges = ranges;
    }
}

ModeHolder::ModeHolder(ISourceMode* sourceMode)
    : m_sourceMode(sourceMode)
{
}

NTSTATUS ModeHolder::CreateNewSet(SourceModeSetCreator& sources) const
{
    PAGED_CODE();
    return CreateAndAddModes(sources);
}

NTSTATUS ModeHolder::CreateNewSet(TargetModeSetCreator& targets) const
{
    PAGED_CODE();
    return CreateAndAddModes(targets);
}

template <class T>
NTSTATUS ModeHolder::CreateAndAddModes(T& set) const
{
    NTSTATUS status = set.Create();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = AddModes(*set);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = set.Assign();
    return status;
}

NTSTATUS ModeHolder::AddModes(SourceModeSet& sources) const
{
    return AddMode(sources);
}

NTSTATUS ModeHolder::AddModes(TargetModeSet& targets) const
{
    return AddMode(targets);
}

NTSTATUS ModeHolder::AddModes(MonitorModeSet& monitors) const
{
    return AddMode(monitors);
}

template <class T>
NTSTATUS ModeHolder::AddMode(T& set) const
{
    UINT popularModeCount = 0;
    ScopedArray<Mode>& popularMode = m_sourceMode->GetAvailableSourceMode(popularModeCount);

    for(UINT index = 0; index < popularModeCount; ++index)
    {
        typename T::Mode setMode;
        NTSTATUS status = set.CreateMode(setMode);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        FillMode(setMode, popularMode.Get()[index]);

        status = set.AddMode(setMode);
        if (!NT_SUCCESS(status) && status != STATUS_GRAPHICS_MODE_ALREADY_IN_MODESET)
        {
            return status;
        }
    }

    return STATUS_SUCCESS;
}
