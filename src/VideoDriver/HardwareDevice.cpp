#include "stdafx.h"
#include "HardwareDevice.h"
#include "DxgkWrapper.h"

namespace
{
    struct DeviceDescription
    {
        const wchar_t* chipType;
        const wchar_t* dacType;
        const wchar_t* adapter;
        const wchar_t* bios;
    };

    const wchar_t g_adapterName[] = L"Simple Display Only Driver";

    DeviceDescription g_device = {L"VGABIOS", L"VGABIOS", g_adapterName, L"VGABIOS"};   //Bochs emulated adapter

    void WriteRegistry(HANDLE key, const wchar_t* valueName, ULONG type, const void* value, ULONG valueSize)
    {
        UNICODE_STRING valueNameStr = {};
        NTSTATUS status = RtlUnicodeStringInit(&valueNameStr, valueName);
        if (!NT_SUCCESS(status))
        {
            return;
        }

        status = ZwSetValueKey(key, &valueNameStr, 0, type, const_cast<void*>(value), valueSize);
        if (!NT_SUCCESS(status))
        {
            return;
        }
    }

    void WriteRegistryString(HANDLE key, const wchar_t* valueName, const wchar_t* value)
    {
        size_t lenWithTerminator = wcslen(value) + 1;
        ULONG size = static_cast<ULONG>(lenWithTerminator * sizeof(wchar_t));
        WriteRegistry(key, valueName, REG_SZ, value, size);
    }

    void WriteRegistryDword(HANDLE key, const wchar_t* valueName, DWORD value)
    {
        WriteRegistry(key, valueName, REG_DWORD, &value, sizeof(DWORD));
    }
}

HardwareDevice::HardwareDevice(DEVICE_OBJECT* device, DxgkWrapper& dxgk)
    : m_device(device)
    , m_dxgk(&dxgk)
{
}

void HardwareDevice::RegisterHardware()
{
    RegisterInfo(g_device.chipType, g_device.dacType, g_device.adapter, g_device.bios);
}

void HardwareDevice::RegisterInfo(
    const wchar_t* chip, const wchar_t* dac, const wchar_t* adapter, const wchar_t* bios)
{
    HANDLE key = NULL;
    NTSTATUS status = ::IoOpenDeviceRegistryKey(m_device, PLUGPLAY_REGKEY_DRIVER, KEY_SET_VALUE, &key);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    WriteRegistryString(key, L"HardwareInformation.ChipType", chip);
    WriteRegistryString(key, L"HardwareInformation.DacType", dac);
    WriteRegistryString(key, L"HardwareInformation.AdapterString", adapter);
    WriteRegistryString(key, L"HardwareInformation.BiosString", bios);
    WriteRegistryDword(key, L"HardwareInformation.MemorySize", 0); //no access to video memory

    status = ::ZwClose(key);
    if (!NT_SUCCESS(status))
    {
        return;
    }
}