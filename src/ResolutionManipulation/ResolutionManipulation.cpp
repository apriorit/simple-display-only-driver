#include "stdafx.h"
#include "VideoDriverEscape.h"

void ShowUsage(const char* program);
bool SetCanApplyResolution(bool canApplyResolution);

class DesktopDc
{
public:
    DesktopDc()
        : m_dc(::GetDC(NULL))
    {
    }

    ~DesktopDc()
    {
        if (m_dc != NULL)
        {
            if (!::ReleaseDC(NULL, m_dc))
            {
                std::cout << "ReleaseDC failed with error = " << ::GetLastError() << std::endl;
            }
        }
    }

    operator HDC()
    {
        return m_dc;
    }

private:
    HDC m_dc;
};

class Adapter
{
public:
    Adapter(D3DKMT_HANDLE adapter)
        : m_adapter(adapter)
    {
    }

    ~Adapter()
    {
        D3DKMT_CLOSEADAPTER close = {m_adapter};
        NTSTATUS status = D3DKMTCloseAdapter(&close);
        if (!NT_SUCCESS(status))
        {
            std::cout << "D3DKMTCloseAdapter failed with status = 0x" << std::hex 
                << std::setw(8) << std::setfill('0') << status << std::endl;
        }
    }

    operator D3DKMT_HANDLE()
    {
        return m_adapter;
    }

private:
    D3DKMT_HANDLE m_adapter;
};

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Invalid count of parameters" << std::endl;
        ShowUsage(argv[0]);
        return EXIT_FAILURE;
    }

    bool canApplyResolution = atoi(argv[1]) > 0 ? true : false;
    return SetCanApplyResolution(canApplyResolution) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void ShowUsage(const char* program)
{
    std::cout << "Usage:   " << program << " canApplyResolution(1 - true; 0 - false) " << std::endl
              << "Example: " << program << " 1 " << std::endl;
}


bool SetCanApplyResolution(bool canChangeResolution)
{
    DesktopDc dc;
    if (dc == NULL)
    {
        std::cout << "GetDC failed with error = " << ::GetLastError() << std::endl;
        return false;
    }

    D3DKMT_OPENADAPTERFROMHDC openAdapter = {dc};
    NTSTATUS status = D3DKMTOpenAdapterFromHdc(&openAdapter);
    if (!NT_SUCCESS(status))
    {
        std::cout << "D3DKMTOpenAdapterFromHdc failed with status = 0x" << std::hex 
            << std::setw(8) << std::setfill('0') << status << std::endl;
        return false;
    }

    Adapter adapter(openAdapter.hAdapter);

    CanApplyResolution data = {};
    PrepareCanApplyResolutionEscape(data, canChangeResolution);

    D3DKMT_ESCAPE escape = {adapter};
    escape.pPrivateDriverData = &data;
    escape.PrivateDriverDataSize = sizeof(CanApplyResolution);

    status = D3DKMTEscape(&escape);
    if (!NT_SUCCESS(status))
    {
        std::cout << "D3DKMTEscape failed with status = 0x" << std::hex 
            << std::setw(8) << std::setfill('0') << status << std::endl;
        return false;
    }

    return true;
}
