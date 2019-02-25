#include "stdafx.h"
#include "BDD.hxx"
#include "BltFuncs.h"
#include "Mapping.h"
#include "Conversions.h"
#include "RealGraphicMode.h"
#include "Memory.h"
#include "VideoDriverEscape.h"

BASIC_DISPLAY_DRIVER::BASIC_DISPLAY_DRIVER(DEVICE_OBJECT* device)
    : m_started(false)
    , m_device(device, m_dxgk)
    , m_adapterPowerState(PowerDeviceD0)
{
    PAGED_CODE();
}

NTSTATUS BASIC_DISPLAY_DRIVER::Init(const DXGK_DISPLAY_INFORMATION& info)
{
    m_currentModes.Reset(new (NonPagedPool) RealGraphicMode[MAX_VIEWS]);
    if(m_currentModes.Get() == nullptr)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    m_sourceTargetMap.Reset(new (PagedPool) UINT[MAX_VIEWS]);
    if(m_sourceTargetMap.Get() == nullptr)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    GraphicMode* modes = m_currentModes.Get();
    for (size_t i = 0; i < MAX_VIEWS; ++i)
    {
        GraphicMode& mode = modes[i];        
        NTSTATUS status = mode.Init(info);
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }    

    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::StartDevice(
    DXGK_START_INFO* /*startInfo*/, 
    DXGKRNL_INTERFACE* dxgkInterface, 
    ULONG* views, 
    ULONG* children)
{
    PAGED_CODE();

    ASSERT(dxgkInterface != nullptr);
    ASSERT(views != nullptr);
    ASSERT(children != nullptr);

    m_dxgk.Set(dxgkInterface);
    m_device.RegisterHardware();

    // This sample driver only uses the frame buffer of the POST device. DxgkCbAcquirePostDisplayOwnership
    // gives you the frame buffer address and ensures that no one else is drawing to it. Be sure to give it back!
    DXGK_DISPLAY_INFORMATION info = {};
    info.TargetId = D3DDDI_ID_UNINITIALIZED;
    NTSTATUS status = m_dxgk.AcquirePostDisplayOwnership(&info);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if(info.Width == 0)
    {
        // The most likely cause of failure is that the driver is simply not running on a POST device, or we are running
        // after a pre-WDDM 1.2 driver. Since we can't draw anything, we should fail to start.
        return STATUS_UNSUCCESSFUL;
    }

    status = Init(info);
    if(!NT_SUCCESS(status))
    {
        return status;
    }

    m_started = true;
    *views = MAX_VIEWS;
    *children = MAX_CHILDREN;
    
    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::StopDevice()
{
    PAGED_CODE();
    m_started = false;
    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::SetPowerState(
    ULONG hardwareUid, DEVICE_POWER_STATE deviceState, POWER_ACTION /*action*/)
{
    PAGED_CODE();
    if (hardwareUid == DISPLAY_ADAPTER_HW_ID)
    {
        if (deviceState == PowerDeviceD0)
        {
            // When returning from D3 the device visibility defined to be off for all targets
            if (m_adapterPowerState == PowerDeviceD3)
            {
                DXGKARG_SETVIDPNSOURCEVISIBILITY visibility;
                visibility.VidPnSourceId = D3DDDI_ID_ALL;
                visibility.Visible = FALSE;
                SetVidPnSourceVisibility(&visibility);
            }
        }

        m_adapterPowerState = deviceState;
    }
    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::QueryChildRelations(
    DXGK_CHILD_DESCRIPTOR* childRelations, ULONG childRelationsSize)
{
    PAGED_CODE();

    ASSERT(childRelations != nullptr);

    // the last DXGK_CHILD_DESCRIPTOR in the array must remain zeroed out
    ULONG count = (childRelationsSize / sizeof(DXGK_CHILD_DESCRIPTOR)) - 1;
    ASSERT(count <= MAX_CHILDREN);

    for (UINT i = 0; i < count; ++i)
    {
        DXGK_CHILD_DESCRIPTOR& child = childRelations[i];

        child.ChildDeviceType = TypeVideoOutput;
        child.AcpiUid = 0;
        child.ChildUid = i;

        DXGK_CHILD_CAPABILITIES& caps = child.ChildCapabilities;
        caps.HpdAwareness = HpdAwarenessInterruptible;

        DXGK_VIDEO_OUTPUT_CAPABILITIES& video = caps.Type.VideoOutput;
        video.InterfaceTechnology = D3DKMDT_VOT_OTHER;
        video.MonitorOrientationAwareness = D3DKMDT_MOA_NONE;
        video.SupportsSdtvModes = FALSE;
    }

    return STATUS_SUCCESS;
}

NTSTATUS BASIC_DISPLAY_DRIVER::QueryChildStatus(
    DXGK_CHILD_STATUS* childStatus, BOOLEAN /*nonDestructiveOnly*/)
{
    PAGED_CODE();

    ASSERT(childStatus != nullptr);
    ASSERT(childStatus->ChildUid < MAX_CHILDREN);

    switch (childStatus->Type)
    {
    case StatusConnection:
        {
            childStatus->HotPlug.Connected = IsDriverActive();
            return STATUS_SUCCESS;
        }

    case StatusRotation:
        {
            // D3DKMDT_MOA_NONE was reported, so this should never be called
            return STATUS_INVALID_PARAMETER;
        }

    default:
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
}

NTSTATUS BASIC_DISPLAY_DRIVER::QueryAdapterInfo(const DXGKARG_QUERYADAPTERINFO* info)
{
    PAGED_CODE();

    ASSERT(info != nullptr);

    switch (info->Type)
    {
    case DXGKQAITYPE_DRIVERCAPS:
        {
            if (info->OutputDataSize < sizeof(DXGK_DRIVERCAPS))
            {
                return STATUS_BUFFER_TOO_SMALL;
            }

            DXGK_DRIVERCAPS* caps = static_cast<DXGK_DRIVERCAPS*>(info->pOutputData);

            RtlZeroMemory(caps, sizeof(DXGK_DRIVERCAPS));

            caps->WDDMVersion = DXGKDDI_WDDMv1_2;
            caps->HighestAcceptableAddress.QuadPart = -1;
            caps->SupportNonVGA = TRUE;
            caps->SupportSmoothRotation = TRUE;

            return STATUS_SUCCESS;
        }

    default:
        {
            return STATUS_NOT_SUPPORTED;
        }
    }
}

NTSTATUS BASIC_DISPLAY_DRIVER::PresentDisplayOnly(const DXGKARG_PRESENT_DISPLAYONLY* present)
{
    PAGED_CODE();

    ASSERT(present != nullptr);
    ASSERT(present->VidPnSourceId < MAX_VIEWS);
    
    if (present->BytesPerPixel < MIN_BYTES_PER_PIXEL_REPORTED)
    {
        // Only >=32bpp modes are reported, therefore this Present should never pass anything less than 4 bytes per pixel
        return STATUS_INVALID_PARAMETER;
    }

    UINT target = m_sourceTargetMap.Get()[present->VidPnSourceId];
    GraphicMode& mode = m_currentModes.Get()[target];
    return mode.ExecutePresentDisplayOnly(
        present->pSource,
        present->BytesPerPixel,
        present->Pitch,
        present->NumMoves,
        present->pMoves,
        present->NumDirtyRects,
        present->pDirtyRect,
        present->Flags.Rotate != 0);
}

NTSTATUS BASIC_DISPLAY_DRIVER::StopDeviceAndReleasePostDisplayOwnership(
    D3DDDI_VIDEO_PRESENT_TARGET_ID /*target*/, DXGK_DISPLAY_INFORMATION* info)
{
    PAGED_CODE();

    // The driver has to black out the display and ensure it is visible when releasing ownership
    GraphicMode& mode = m_currentModes.Get()[0];
    mode.Blackout();
    *info = mode.GetMode();
    
    return StopDevice();
}

NTSTATUS BASIC_DISPLAY_DRIVER::QueryVidPnHWCapability(DXGKARG_QUERYVIDPNHWCAPABILITY* caps)
{
    PAGED_CODE();

    ASSERT(caps != NULL);
    ASSERT(caps->SourceId < MAX_VIEWS);
    ASSERT(caps->TargetId < MAX_CHILDREN);

    caps->VidPnHWCaps.DriverRotation             = 1; // BDD does rotation in software
    caps->VidPnHWCaps.DriverScaling              = 0; // BDD does not support scaling
    caps->VidPnHWCaps.DriverCloning              = 0; // BDD does not support clone
    caps->VidPnHWCaps.DriverColorConvert         = 1; // BDD does color conversions in software
    caps->VidPnHWCaps.DriverLinkedAdapaterOutput = 0; // BDD does not support linked adapters
    caps->VidPnHWCaps.DriverRemoteDisplay        = 0; // BDD does not support remote displays

    return STATUS_SUCCESS;
}

void BASIC_DISPLAY_DRIVER::DpcRoutine()
{
    m_dxgk.NotifyDpc();
}

NTSTATUS BASIC_DISPLAY_DRIVER::SystemDisplayEnable(
    D3DDDI_VIDEO_PRESENT_TARGET_ID target,
    DXGKARG_SYSTEM_DISPLAY_ENABLE_FLAGS* /*flags*/,
    UINT* width,
    UINT* height,
    D3DDDIFORMAT* format)
{
    ASSERT(target < MAX_CHILDREN || target == D3DDDI_ID_UNINITIALIZED);
    UNREFERENCED_PARAMETER(target);

    if (!m_currentModes.Get()[0].IsValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation = D3DKMDT_VPPR_UNINITIALIZED;
    m_currentModes.Get()[0].GetMode(width, height, format, &rotation, nullptr);

    if (rotation == D3DKMDT_VPPR_ROTATE90 || rotation == D3DKMDT_VPPR_ROTATE270)
    {
        UINT w = *width;
        *width = *height;
        *height = w;
    }

    return STATUS_SUCCESS;
}

void BASIC_DISPLAY_DRIVER::SystemDisplayWrite(
    void* source, UINT width, UINT height, UINT stride, INT x, INT y)
{
    m_currentModes.Get()[0].SystemWrite(source, width, height, stride, x, y);
}

NTSTATUS BASIC_DISPLAY_DRIVER::Escape(const DXGKARG_ESCAPE* escape)
{
    PAGED_CODE();

    ASSERT(escape != nullptr);

    UINT size = escape->PrivateDriverDataSize;
    if (size < sizeof(BaseEscape))
    {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    const BaseEscape* base = static_cast<const BaseEscape*>(escape->pPrivateDriverData);
    if (base->code == EscapeCanApplyResolution)
    {
        if (size < sizeof(CanApplyResolution))
        {
            return STATUS_INVALID_BUFFER_SIZE;
        }

        const CanApplyResolution* resolution = static_cast<const CanApplyResolution*>(
            escape->pPrivateDriverData);

        m_currentModes.Get()[0].SetCanApplyResolution(resolution->canApply);

        return STATUS_SUCCESS;
    }

    return STATUS_NOT_IMPLEMENTED;
}

bool AssertIsDriverActive(const void* driver)
{
    ASSERT(driver != nullptr);
    const BASIC_DISPLAY_DRIVER* display = static_cast<const BASIC_DISPLAY_DRIVER*>(driver);
    return display->IsDriverActive();
}
