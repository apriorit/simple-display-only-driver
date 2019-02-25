#include "stdafx.h"
#include "DxgkWrapper.h"

DxgkWrapper::DxgkWrapper()
{
    RtlZeroMemory(&m_interface, sizeof(DXGKRNL_INTERFACE));
}

void DxgkWrapper::Set(const DXGKRNL_INTERFACE* dinterface)
{
    ASSERT(dinterface != nullptr);
    m_interface = *dinterface;
}

NTSTATUS DxgkWrapper::GetDeviceInfo(DXGK_DEVICE_INFO* info)
{
    return m_interface.DxgkCbGetDeviceInformation(m_interface.DeviceHandle, info);
}

NTSTATUS DxgkWrapper::ReadDeviceSpace(
    ULONG dataType, void* buffer, ULONG offset, ULONG length, ULONG* read)
{
    return m_interface.DxgkCbReadDeviceSpace(
        m_interface.DeviceHandle, dataType, buffer, offset, length, read);
}

NTSTATUS DxgkWrapper::IndicateChildStatus(DXGK_CHILD_STATUS* child)
{
    return m_interface.DxgkCbIndicateChildStatus(m_interface.DeviceHandle, child);
}

void DxgkWrapper::NotifyDpc()
{
    m_interface.DxgkCbNotifyDpc(m_interface.DeviceHandle);
}

NTSTATUS DxgkWrapper::QueryVidPnInterface(
    D3DKMDT_HVIDPN vidPn,
    DXGK_VIDPN_INTERFACE_VERSION version,
    const DXGK_VIDPN_INTERFACE** vinterface)
{
    return m_interface.DxgkCbQueryVidPnInterface(vidPn, version, vinterface);
}

NTSTATUS DxgkWrapper::AcquirePostDisplayOwnership(DXGK_DISPLAY_INFORMATION* displayInfo)
{
    return m_interface.DxgkCbAcquirePostDisplayOwnership(
        m_interface.DeviceHandle, displayInfo);
}
