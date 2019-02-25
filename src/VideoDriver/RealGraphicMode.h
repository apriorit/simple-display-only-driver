#pragma once
#include "GraphicMode.h"
#include "MappedMemory.h"
#include "ScopedArray.h"

class RealGraphicMode : public GraphicMode
{
public:
    RealGraphicMode();
    virtual ScopedArray<Mode>& GetAvailableSourceMode(UINT& count) override;
    virtual void SetCanApplyResolution(bool canApply) override;

protected:
    virtual NTSTATUS Prepare() override;
    virtual void* GetPointer() override;
    virtual const void* GetPointer() const override;
    virtual NTSTATUS SetResolutionLength(ULONG length) override;
    virtual NTSTATUS CanApplyResolutionLength(const DXGK_DISPLAY_INFORMATION& oldMode, const DXGK_DISPLAY_INFORMATION& newMode) override;

private:
    NTSTATUS SetMode();

private:
    MappedMemory m_mappedMemory;
    ScopedArray<VIDEO_MODE_INFORMATION> m_videoModeInfo;
    ScopedArray<USHORT> m_modeNumbers;
    ScopedArray<Mode> m_availableMode;
    USHORT m_modeCount;
    bool m_isx86BiosAvailable;
    bool m_canApplyResolution;
};

