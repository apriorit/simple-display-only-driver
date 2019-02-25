#pragma once
#include "DxgkWrapper.h"
#include "ModeHolder.h"
#include "HardwareDevice.h"
#include "ScopedPtr.h"
#include "ScopedArray.h"
#include "GraphicMode.h"

#define MIN_BYTES_PER_PIXEL_REPORTED    4

#define MAX_CHILDREN                    1
#define MAX_VIEWS                       1                

class BASIC_DISPLAY_DRIVER
{
public:
    BASIC_DISPLAY_DRIVER(DEVICE_OBJECT* device);

    bool IsDriverActive() const
    {
        return m_started;
    }

    NTSTATUS StartDevice(
        DXGK_START_INFO* startInfo, DXGKRNL_INTERFACE* dxgkInterface, ULONG* views, ULONG* children);

    NTSTATUS StopDevice();

    // Used to either turn off/on monitor (if possible), or mark that system is going into hibernate
    NTSTATUS SetPowerState(ULONG hardwareUid, DEVICE_POWER_STATE deviceState, POWER_ACTION action);

    NTSTATUS QueryChildRelations(DXGK_CHILD_DESCRIPTOR* childRelations, ULONG childRelationsSize);
    NTSTATUS QueryChildStatus(DXGK_CHILD_STATUS* childStatus, BOOLEAN nonDestructiveOnly);

    void DpcRoutine();

    NTSTATUS QueryAdapterInfo(const DXGKARG_QUERYADAPTERINFO* info);

    NTSTATUS PresentDisplayOnly(const DXGKARG_PRESENT_DISPLAYONLY* present);

    NTSTATUS IsSupportedVidPn(DXGKARG_ISSUPPORTEDVIDPN* supported);

    NTSTATUS RecommendMonitorModes(const DXGKARG_RECOMMENDMONITORMODES* recommend);

    NTSTATUS EnumVidPnCofuncModality(const DXGKARG_ENUMVIDPNCOFUNCMODALITY* modality);

    NTSTATUS SetVidPnSourceVisibility(const DXGKARG_SETVIDPNSOURCEVISIBILITY* visibility);

    NTSTATUS CommitVidPn(const DXGKARG_COMMITVIDPN* commit);

    NTSTATUS UpdateActiveVidPnPresentPath(const DXGKARG_UPDATEACTIVEVIDPNPRESENTPATH* path);

    NTSTATUS QueryVidPnHWCapability(DXGKARG_QUERYVIDPNHWCAPABILITY* caps);

    NTSTATUS StopDeviceAndReleasePostDisplayOwnership(
        D3DDDI_VIDEO_PRESENT_TARGET_ID target, DXGK_DISPLAY_INFORMATION* info);

    // Must be Non-Paged
    // Call to initialize as part of bugcheck
    NTSTATUS SystemDisplayEnable(
        D3DDDI_VIDEO_PRESENT_TARGET_ID target,
        DXGKARG_SYSTEM_DISPLAY_ENABLE_FLAGS* flags,
        UINT* width,
        UINT* height,
        D3DDDIFORMAT* format);

    // Must be Non-Paged
    // Write out pixels as part of bugcheck
    void SystemDisplayWrite(void* source, UINT width, UINT height, UINT stride, INT x, INT y);
    NTSTATUS Escape(const DXGKARG_ESCAPE* escape);


private:
    NTSTATUS Init(const DXGK_DISPLAY_INFORMATION& info);

    // These two functions make checks on the values of some of the fields of their respective structures to ensure
    // that the specified fields are supported by BDD, i.e. gamma ramp must be D3DDDI_GAMMARAMP_DEFAULT
    NTSTATUS IsVidPnPathFieldsValid(const D3DKMDT_VIDPN_PRESENT_PATH* path) const;
    NTSTATUS IsVidPnSourceModeFieldsValid(const D3DKMDT_VIDPN_SOURCE_MODE* mode) const;

    NTSTATUS SetSourceModeAndPath(
        const D3DKMDT_VIDPN_SOURCE_MODE* source,
        const D3DKMDT_VIDPN_PRESENT_PATH* path);    

private:
    bool m_started;
    DxgkWrapper m_dxgk;
    HardwareDevice m_device;
    ScopedArray<GraphicMode> m_currentModes;
    ScopedArray<UINT> m_sourceTargetMap;
    DEVICE_POWER_STATE m_adapterPowerState;
};

bool AssertIsDriverActive(const void* driver);
