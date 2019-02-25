#include "stdafx.h"
#include "BDD.hxx"
#include "Mapping.h"
#include "SourceModeSetCreator.h"
#include "TargetModeSetCreator.h"
#include "TopologyCreator.h"
#include "MonitorSet.h"
#include "Conversions.h"
#include "Memory.h"
#include "GraphicMode.h"

namespace
{
    template <class Creator, class Set>
    NTSTATUS HasPinnedMode(Creator& modes, bool& hasPinned)
    {
        NTSTATUS status = modes.Acquire();
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        typename Set::CMode mode;
        status = modes->GetPinnedMode(mode);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        hasPinned = mode.Get() != nullptr;
        return STATUS_SUCCESS;
    }

    template <class Creator, class Set, class Id>
    NTSTATUS AddModesIfEmpty(
        const ModeHolder& holder,
        const DXGK_VIDPN_INTERFACE* vidPnInterface,
        D3DKMDT_HVIDPN vidPn,
        Id id)
    {
        Creator modes(vidPnInterface, vidPn, id);
        
        bool hasPinned = false;
        NTSTATUS status = HasPinnedMode<Creator, Set>(modes, hasPinned);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        // if there's no pinned, add possible modes (otherwise they've already been added)
        if (!hasPinned)
        {
            status = holder.CreateNewSet(modes);
        }

        return status;
    }
}

// Display-Only Devices can only return display modes of D3DDDIFMT_A8R8G8B8.
// Color conversion takes place if the app's fullscreen backbuffer has different format.
// Full display drivers can add more if the hardware supports them.
D3DDDIFORMAT gBddPixelFormats[] = {
    D3DDDIFMT_A8R8G8B8
};

NTSTATUS BASIC_DISPLAY_DRIVER::IsSupportedVidPn(DXGKARG_ISSUPPORTEDVIDPN* supported)
{
    PAGED_CODE();
    ASSERT(supported != nullptr);

    if (supported->hDesiredVidPn == NULL)
    {
        supported->IsVidPnSupported = TRUE;
        return STATUS_SUCCESS;
    }

    supported->IsVidPnSupported = FALSE;

    const DXGK_VIDPN_INTERFACE* vidpnInterface = nullptr;
    NTSTATUS status = m_dxgk.QueryVidPnInterface(
        supported->hDesiredVidPn, DXGK_VIDPN_INTERFACE_VERSION_V1, &vidpnInterface);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    TopologyCreator topology(vidpnInterface);
    status = topology.Get(supported->hDesiredVidPn);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    // For every source in this topology, make sure they don't have more paths than there are targets
    for (D3DDDI_VIDEO_PRESENT_SOURCE_ID id = 0; id < MAX_VIEWS; ++id)
    {
        SIZE_T paths = 0;
        status = topology->GetNumPathsFromSource(id, &paths);
        if (status == STATUS_GRAPHICS_SOURCE_NOT_IN_TOPOLOGY)
        {
            continue;
        }
        else if (!NT_SUCCESS(status))
        {
            return status;
        }
        else if (paths > MAX_CHILDREN)
        {
            // This VidPn is not supported, which has already been set as the default
            return STATUS_SUCCESS;
        }
    }

    supported->IsVidPnSupported = TRUE;

    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::RecommendMonitorModes(const DXGKARG_RECOMMENDMONITORMODES* recommend)
{
    PAGED_CODE();

    MonitorSet set(recommend->pMonitorSourceModeSetInterface, recommend->hMonitorSourceModeSet);
    GraphicMode& targetMode = m_currentModes.Get()[recommend->VideoPresentTargetId];
    ModeHolder modeHolder(&targetMode);
    return modeHolder.AddModes(set);
}

NTSTATUS BASIC_DISPLAY_DRIVER::EnumVidPnCofuncModality(
    const DXGKARG_ENUMVIDPNCOFUNCMODALITY* modality)
{
    PAGED_CODE();

    ASSERT(modality != NULL);

    const DXGK_VIDPN_INTERFACE* vidPnInterface = NULL;
    NTSTATUS status = m_dxgk.QueryVidPnInterface(
        modality->hConstrainingVidPn, DXGK_VIDPN_INTERFACE_VERSION_V1, &vidPnInterface);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    TopologyCreator topology(vidPnInterface);
    status = topology.Get(modality->hConstrainingVidPn);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    TopologyPathEnumerator enumerator = topology->EnumeratePaths();
    for (; !enumerator.End(); enumerator.Next())
    {
        TopologyPathGuard& path = enumerator.GetPath();
        GraphicMode& targetMode = m_currentModes.Get()[path->VidPnTargetId];
        ModeHolder modeHolder(&targetMode);
        if (modality->EnumPivotType != D3DKMDT_EPT_VIDPNSOURCE ||
            modality->EnumPivot.VidPnSourceId != path->VidPnSourceId)
        {
            status = AddModesIfEmpty<SourceModeSetCreator, SourceModeSet>(
                modeHolder, vidPnInterface, modality->hConstrainingVidPn, path->VidPnSourceId);

            if (!NT_SUCCESS(status))
            {
                return status;
            }
        }

        if (modality->EnumPivotType != D3DKMDT_EPT_VIDPNTARGET ||
            modality->EnumPivot.VidPnTargetId != path->VidPnTargetId)
        {
            status = AddModesIfEmpty<TargetModeSetCreator, TargetModeSet>(
                modeHolder, vidPnInterface, modality->hConstrainingVidPn, path->VidPnTargetId);
            
            if (!NT_SUCCESS(status))
            {
                return status;
            }
        }

        D3DKMDT_VIDPN_PRESENT_PATH changedPath = *path;
        bool isPathChanged = false;

        if ((modality->EnumPivotType == D3DKMDT_EPT_SCALING ||
            modality->EnumPivot.VidPnSourceId != path->VidPnSourceId ||
            modality->EnumPivot.VidPnTargetId != path->VidPnTargetId) &&
            path->ContentTransformation.Scaling == D3DKMDT_VPPS_UNPINNED)
        {
            // Identity and centered scaling are supported, but not any stretch modes
            D3DKMDT_VIDPN_PRESENT_PATH_SCALING_SUPPORT& scaling =
                changedPath.ContentTransformation.ScalingSupport;

            RtlZeroMemory(&scaling, sizeof(D3DKMDT_VIDPN_PRESENT_PATH_SCALING_SUPPORT));

            scaling.Identity = 1;
            scaling.Centered = 1;
            isPathChanged = true;
        } 

        if ((modality->EnumPivotType == D3DKMDT_EPT_ROTATION ||
            modality->EnumPivot.VidPnSourceId != path->VidPnSourceId ||
            modality->EnumPivot.VidPnTargetId != path->VidPnTargetId) &&
            path->ContentTransformation.Rotation == D3DKMDT_VPPR_UNPINNED)
        {
            D3DKMDT_VIDPN_PRESENT_PATH_ROTATION_SUPPORT& rotation = 
                changedPath.ContentTransformation.RotationSupport;

            RtlZeroMemory(&rotation, sizeof(D3DKMDT_VIDPN_PRESENT_PATH_ROTATION_SUPPORT));

            rotation.Identity = 1;
            rotation.Rotate90 = 1;
            rotation.Rotate180 = 1;
            rotation.Rotate270 = 1;
            isPathChanged = true;
        }

        if (isPathChanged)
        {
            status = topology->UpdatePathSupportInfo(&changedPath);;
            if (!NT_SUCCESS(status))
            {
                return status;
            }
        }
    }

    status = enumerator.GetStatus();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return (status == STATUS_GRAPHICS_NO_MORE_ELEMENTS_IN_DATASET ? STATUS_SUCCESS : status);
}

NTSTATUS BASIC_DISPLAY_DRIVER::SetVidPnSourceVisibility(
    const DXGKARG_SETVIDPNSOURCEVISIBILITY* visibility)
{
    PAGED_CODE();
    ASSERT(visibility != NULL);
    ASSERT(visibility->VidPnSourceId < MAX_VIEWS || visibility->VidPnSourceId == D3DDDI_ID_ALL);

    auto currentModeCache = m_currentModes.Get();
    auto sourceTargetCache = m_sourceTargetMap.Get();
    if(visibility->VidPnSourceId == D3DDDI_ID_ALL)
    {
        for(int id = 0; id < MAX_VIEWS; ++id)
        {
            GraphicMode& currentMode = currentModeCache[id];
            currentMode.SetVisible(visibility->Visible != FALSE);
        }
    }
    else
    {
        int id = sourceTargetCache[visibility->VidPnSourceId];
        GraphicMode& currentMode = currentModeCache[id];
        currentMode.SetVisible(visibility->Visible != FALSE);
    }
    
    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::CommitVidPn(const DXGKARG_COMMITVIDPN* commit)
{
    PAGED_CODE();

    ASSERT(commit != nullptr);
    ASSERT(commit->AffectedVidPnSourceId < MAX_VIEWS);

    if (commit->Flags.PathPoweredOff)
    {
        return STATUS_SUCCESS;
    }

    const DXGK_VIDPN_INTERFACE* vidPnInterface = nullptr;
    NTSTATUS status = m_dxgk.QueryVidPnInterface(
        commit->hFunctionalVidPn, DXGK_VIDPN_INTERFACE_VERSION_V1, &vidPnInterface);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    TopologyCreator topology(vidPnInterface);
    status = topology.Get(commit->hFunctionalVidPn);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    SIZE_T paths = 0;
    status = topology->GetNumPaths(&paths);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    SourceModeSet::CMode mode;

    if (paths != 0)
    {
        SourceModeSetCreator sources(
            vidPnInterface, commit->hFunctionalVidPn, commit->AffectedVidPnSourceId);

        status = sources.Acquire();
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        status = sources->GetPinnedMode(mode);        
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    if (mode.Get() == nullptr)
    {
        // There is no mode to pin on this source, any old paths here have already been cleared
        return STATUS_SUCCESS;
    }

    status = IsVidPnSourceModeFieldsValid(mode.Get());
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    SIZE_T targets = 0;
    status = topology->GetNumPathsFromSource(commit->AffectedVidPnSourceId, &targets);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    for (SIZE_T i = 0; i < targets; ++i)
    {
        D3DDDI_VIDEO_PRESENT_TARGET_ID target = D3DDDI_ID_UNINITIALIZED;
        status = topology->EnumPathsTargetsFromSource(commit->AffectedVidPnSourceId, i, &target);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        TopologyPathGuard path;
        status = topology->AcquirePathInfo(commit->AffectedVidPnSourceId, target, path);
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        status = IsVidPnPathFieldsValid(path.Get());
        if (!NT_SUCCESS(status))
        {
            return status;
        }

        status = SetSourceModeAndPath(mode.Get(), path.Get());
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    return status;
}

NTSTATUS BASIC_DISPLAY_DRIVER::UpdateActiveVidPnPresentPath(
    const DXGKARG_UPDATEACTIVEVIDPNPRESENTPATH* path)
{
    PAGED_CODE();

    ASSERT(path != NULL);

    auto presentPathInfo = path->VidPnPresentPathInfo;
    NTSTATUS status = IsVidPnPathFieldsValid(&(presentPathInfo));
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    D3DDDI_VIDEO_PRESENT_TARGET_ID target = presentPathInfo.VidPnTargetId;
    m_sourceTargetMap.Get()[presentPathInfo.VidPnSourceId] = target;
    GraphicMode& mode = m_currentModes.Get()[target];
    return mode.UpdateMode(presentPathInfo.ContentTransformation.Rotation);
}


NTSTATUS BASIC_DISPLAY_DRIVER::SetSourceModeAndPath(
    const D3DKMDT_VIDPN_SOURCE_MODE* source, 
    const D3DKMDT_VIDPN_PRESENT_PATH* path)
{
    PAGED_CODE();

    auto graphics = source->Format.Graphics;
    UINT width = graphics.PrimSurfSize.cx;
    UINT height = graphics.PrimSurfSize.cy;
    auto format = graphics.PixelFormat;
    D3DDDI_VIDEO_PRESENT_TARGET_ID target = path->VidPnTargetId;

    GraphicMode& mode = m_currentModes.Get()[target];
    m_sourceTargetMap.Get()[path->VidPnSourceId] = target;

    return mode.UpdateMode(
        width,
        height,
        format,
        path->ContentTransformation.Rotation,
        path->ContentTransformation.Scaling);
}


NTSTATUS BASIC_DISPLAY_DRIVER::IsVidPnPathFieldsValid(const D3DKMDT_VIDPN_PRESENT_PATH* path) const
{
    PAGED_CODE();

    if (path->VidPnSourceId >= MAX_VIEWS)
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE;
    }
    else if (path->VidPnTargetId >= MAX_CHILDREN)
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_TARGET;
    }
    else if (path->GammaRamp.Type != D3DDDI_GAMMARAMP_DEFAULT)
    {
        return STATUS_GRAPHICS_GAMMA_RAMP_NOT_SUPPORTED;
    }
    else if ((path->ContentTransformation.Scaling != D3DKMDT_VPPS_IDENTITY) &&
             (path->ContentTransformation.Scaling != D3DKMDT_VPPS_CENTERED) &&
             (path->ContentTransformation.Scaling != D3DKMDT_VPPS_NOTSPECIFIED) &&
             (path->ContentTransformation.Scaling != D3DKMDT_VPPS_UNINITIALIZED))
    {
        return STATUS_GRAPHICS_VIDPN_MODALITY_NOT_SUPPORTED;
    }
    else if ((path->ContentTransformation.Rotation != D3DKMDT_VPPR_IDENTITY) &&
             (path->ContentTransformation.Rotation != D3DKMDT_VPPR_ROTATE90) &&
             (path->ContentTransformation.Rotation != D3DKMDT_VPPR_ROTATE180) &&
             (path->ContentTransformation.Rotation != D3DKMDT_VPPR_ROTATE270) &&
             (path->ContentTransformation.Rotation != D3DKMDT_VPPR_NOTSPECIFIED) &&
             (path->ContentTransformation.Rotation != D3DKMDT_VPPR_UNINITIALIZED))
    {
        return STATUS_GRAPHICS_VIDPN_MODALITY_NOT_SUPPORTED;
    }
    else if ((path->VidPnTargetColorBasis != D3DKMDT_CB_SCRGB) &&
             (path->VidPnTargetColorBasis != D3DKMDT_CB_UNINITIALIZED))
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE;
    }
    else
    {
        return STATUS_SUCCESS;
    }
}

NTSTATUS BASIC_DISPLAY_DRIVER::IsVidPnSourceModeFieldsValid(const D3DKMDT_VIDPN_SOURCE_MODE* mode) const
{
    PAGED_CODE();

    if (mode->Type != D3DKMDT_RMT_GRAPHICS)
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE;
    }
    else if ((mode->Format.Graphics.ColorBasis != D3DKMDT_CB_SCRGB) &&
             (mode->Format.Graphics.ColorBasis != D3DKMDT_CB_UNINITIALIZED))
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE;
    }
    else if (mode->Format.Graphics.PixelValueAccessMode != D3DKMDT_PVAM_DIRECT)
    {
        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE;
    }
    else
    {
        for (UINT PelFmtIdx = 0; PelFmtIdx < ARRAYSIZE(gBddPixelFormats); ++PelFmtIdx)
        {
            if (mode->Format.Graphics.PixelFormat == gBddPixelFormats[PelFmtIdx])
            {
                return STATUS_SUCCESS;
            }
        }

        return STATUS_GRAPHICS_INVALID_VIDEO_PRESENT_SOURCE_MODE;
    }
}
