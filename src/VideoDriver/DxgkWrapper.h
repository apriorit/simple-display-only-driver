#pragma once

class DxgkWrapper
{
public:
    DxgkWrapper();

    void Set(const DXGKRNL_INTERFACE* dinterface);

    NTSTATUS GetDeviceInfo(DXGK_DEVICE_INFO* info);
    NTSTATUS ReadDeviceSpace(ULONG dataType, void* buffer, ULONG offset, ULONG length, ULONG* read);

    NTSTATUS IndicateChildStatus(DXGK_CHILD_STATUS* status);

    void NotifyDpc();

    NTSTATUS QueryVidPnInterface(
        D3DKMDT_HVIDPN vidPn,
        DXGK_VIDPN_INTERFACE_VERSION version,
        const DXGK_VIDPN_INTERFACE** vinterface);

    NTSTATUS AcquirePostDisplayOwnership(DXGK_DISPLAY_INFORMATION* displayInfo);
    
private:
    DXGKRNL_INTERFACE m_interface;
};