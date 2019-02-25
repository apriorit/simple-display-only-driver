#include "stdafx.h"
#include "RealGraphicMode.h"
#include "Memory.h"
#include "Conversions.h"
#include "Bios.h"


RealGraphicMode::RealGraphicMode()
    : GraphicMode()
    , m_canApplyResolution(true)
{
}

ScopedArray<Mode>& RealGraphicMode::GetAvailableSourceMode(UINT& count)
{
    count = m_modeCount;
    return m_availableMode;
}

void RealGraphicMode::SetCanApplyResolution(bool canApply)
{
    m_canApplyResolution = canApply;
}

NTSTATUS RealGraphicMode::Prepare()
{
    m_isx86BiosAvailable = Isx86BiosFunctionsAvailable();

    if(!m_isx86BiosAvailable)
    {
        m_modeCount = 1;
        m_availableMode.Reset(new (PagedPool) Mode[m_modeCount]);
        Mode mode = {GetMode().Width, GetMode().Height};
        m_availableMode.Get()[0] = mode;
        return STATUS_SUCCESS;
    }    

    NTSTATUS status = GetVideoModeInfoFromBIOS(m_videoModeInfo, m_modeNumbers, m_modeCount, static_cast<short>(BppFromPixelFormat(GetMode().ColorFormat)));
    if(!NT_SUCCESS(status))
    {
        return status;
    }

    m_availableMode.Reset(new (PagedPool) Mode[m_modeCount]);
    auto availableModeTmp = m_availableMode.Get();
    auto videoModeInfoTmp = m_videoModeInfo.Get();
    for (UINT i = 0; i < m_modeCount; ++i)
    {
        auto inf = videoModeInfoTmp[i];
        Mode mode = {inf.VisScreenWidth, inf.VisScreenHeight};
        availableModeTmp[i] = mode;
    }

    return STATUS_SUCCESS;
}

void* RealGraphicMode::GetPointer()
{
    return m_mappedMemory.Get();
}

const void* RealGraphicMode::GetPointer() const
{
    return m_mappedMemory.Get();
}

NTSTATUS RealGraphicMode::SetResolutionLength(ULONG length)
{
    const PHYSICAL_ADDRESS& addr = GetMode().PhysicAddress;
    NTSTATUS status = m_mappedMemory.Reset(addr, length);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (m_isx86BiosAvailable)
    {
        status = SetMode();
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS RealGraphicMode::CanApplyResolutionLength(const DXGK_DISPLAY_INFORMATION& oldMode, const DXGK_DISPLAY_INFORMATION& newMode)
{
    if(oldMode.Width != newMode.Width &&
        oldMode.Height != newMode.Height &&
        oldMode.ColorFormat != newMode.ColorFormat)
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (!m_canApplyResolution)
    {
        if(oldMode.Width == newMode.Width &&
            oldMode.Height == newMode.Height &&
            oldMode.ColorFormat == newMode.ColorFormat)
        {
            return STATUS_SUCCESS;
        }

        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS RealGraphicMode::SetMode()
{
    auto videoModeInfoCache = m_videoModeInfo.Get();
    auto videoModeNumberCache = m_modeNumbers.Get();
    auto modeCache = GetMode();
    for (USHORT i = 0; i < m_modeCount; ++i)
    {
        if(modeCache.Height == videoModeInfoCache[i].VisScreenHeight &&
            modeCache.Width == videoModeInfoCache[i].VisScreenWidth)
        {
            NTSTATUS status = SetModeBIOS(videoModeNumberCache[i]);
            if(!NT_SUCCESS(status))
            {
                return status;
            }
            break;
        }
    }

    return STATUS_SUCCESS;
}

