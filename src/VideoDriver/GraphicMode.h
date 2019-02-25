#pragma once
#include "ISourceMode.h"

class GraphicMode : public ISourceMode
{
public:
    GraphicMode();
    virtual ~GraphicMode() {}

    NTSTATUS Init(const DXGK_DISPLAY_INFORMATION& info);

    NTSTATUS UpdateMode(
        UINT width,
        UINT height,
        D3DDDIFORMAT format,
        D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation,
        D3DKMDT_VIDPN_PRESENT_PATH_SCALING scaling);

    NTSTATUS UpdateMode(
        D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation);

    bool IsValid() const;
    const DXGK_DISPLAY_INFORMATION& GetMode() const;
    void GetMode(
        UINT* width,
        UINT* height,
        D3DDDIFORMAT* format,
        D3DKMDT_VIDPN_PRESENT_PATH_ROTATION* rotation,
        D3DKMDT_VIDPN_PRESENT_PATH_SCALING* scaling) const;

    void Blackout();
    void SetVisible(bool visible);

    NTSTATUS ExecutePresentDisplayOnly(
        void* source,
        UINT bpp,
        LONG pitch,
        ULONG numMoves,
        const D3DKMT_MOVE_RECT* moves,
        ULONG numDirtyRects,
        const RECT* dirtyRect,
        bool rotation);

    void SystemWrite(void* source, UINT width, UINT height, UINT stride, INT x, INT y);
    virtual void SetCanApplyResolution(bool canApply) = 0;

protected:
    virtual NTSTATUS Prepare() = 0;
    virtual void* GetPointer() = 0;
    virtual const void* GetPointer() const = 0;
    virtual NTSTATUS SetResolutionLength(ULONG length) = 0;
    virtual NTSTATUS CanApplyResolutionLength(const DXGK_DISPLAY_INFORMATION& oldMode, const DXGK_DISPLAY_INFORMATION& newMode) = 0;

private:
    DXGK_DISPLAY_INFORMATION m_info;
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION m_rotation;
    D3DKMDT_VIDPN_PRESENT_PATH_SCALING m_scaling;
    bool m_visible;
    bool m_blackouted;
};

