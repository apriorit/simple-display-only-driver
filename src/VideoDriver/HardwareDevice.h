#pragma once

class DxgkWrapper;

class HardwareDevice
{
public:
    HardwareDevice(DEVICE_OBJECT* device, DxgkWrapper& dxgk);

    void RegisterHardware();

private:
    // Set the information in the registry as described here: http://msdn.microsoft.com/en-us/library/windows/hardware/ff569240(v=vs.85).aspx
    void RegisterInfo(
        const wchar_t* chip, const wchar_t* dac, const wchar_t* adapter, const wchar_t* bios);

private:
    DEVICE_OBJECT* m_device;
    DxgkWrapper* m_dxgk;
};

