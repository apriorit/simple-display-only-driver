#include "stdafx.h"
#include "GraphicMode.h"
#include "BltFuncs.h"
#include "MdlGuard.h"
#include "Conversions.h"

namespace
{
    void HwExecutePresentDisplayOnly(
        void* dst,
        ULONG dstPitch,
        ULONG dstBpp,
        D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation,
        void* src,
        ULONG srcPitch,
        ULONG width,
        ULONG height,
        UINT numMoves,
        const D3DKMT_MOVE_RECT* moves,
        UINT numDirtyRects,
        const RECT* dirtyRect)
    {
        PAGED_CODE();

        BLT_INFO dstInfo = {};
        dstInfo.pBits = dst;
        dstInfo.Pitch = dstPitch;
        dstInfo.BitsPerPel = dstBpp;
        dstInfo.Offset.x = 0;
        dstInfo.Offset.y = 0;
        dstInfo.Rotation = rotation;
        dstInfo.Width = width;
        dstInfo.Height = height;

        BLT_INFO srcInfo = {};
        srcInfo.pBits = src;
        srcInfo.Pitch = srcPitch;
        srcInfo.BitsPerPel = 32;
        srcInfo.Offset.x = 0;
        srcInfo.Offset.y = 0;
        srcInfo.Rotation = D3DKMDT_VPPR_IDENTITY;
        srcInfo.Width = width;
        srcInfo.Height = height;

        if (rotation == D3DKMDT_VPPR_ROTATE90 || rotation == D3DKMDT_VPPR_ROTATE270)
        {
            srcInfo.Width = height;
            srcInfo.Height = width;
        }

        for (UINT i = 0; i < numMoves; i++)
        {
            BltBits(&dstInfo, &srcInfo, 1, &moves[i].DestRect);
        }

        for (UINT i = 0; i < numDirtyRects; ++i)
        {
            BltBits(&dstInfo, &srcInfo, 1, &dirtyRect[i]);
        }
    }

    template<class T>
    void Set(T* ptr, T value)
    {
        if (ptr != nullptr)
        {
            *ptr = value;
        }
    }
}

GraphicMode::GraphicMode()
    : m_rotation(D3DKMDT_VPPR_UNINITIALIZED)
    , m_scaling(D3DKMDT_VPPS_UNINITIALIZED)
    , m_visible(true)
    , m_blackouted(false)
{
    RtlZeroMemory(&m_info, sizeof(m_info));
}

NTSTATUS GraphicMode::Init(const DXGK_DISPLAY_INFORMATION& info)
{
    m_info = info;
    return Prepare();
}

NTSTATUS GraphicMode::UpdateMode(
    UINT width,
    UINT height,
    D3DDDIFORMAT format,
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation,
    D3DKMDT_VIDPN_PRESENT_PATH_SCALING scaling)
{
    DXGK_DISPLAY_INFORMATION newMode = {};
    newMode.Width = width;
    newMode.Height = height;
    newMode.ColorFormat = format;
    NTSTATUS status = CanApplyResolutionLength(m_info, newMode);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    ULONG pitch = static_cast<ULONG>(width * m_info.Pitch / m_info.Width);
    m_info.Width = width;
    m_info.Height = height;
    m_info.Pitch = pitch;
    m_info.ColorFormat = format;

    ULONG newLength = pitch * height;
    status = SetResolutionLength(newLength);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    Blackout();

    if (rotation != D3DKMDT_VPPR_UNINITIALIZED)
    {
        m_rotation = rotation;
    }

    if (scaling != D3DKMDT_VPPS_UNINITIALIZED)
    {
        m_scaling = scaling;
    }

    return STATUS_SUCCESS;
}

NTSTATUS GraphicMode::UpdateMode(D3DKMDT_VIDPN_PRESENT_PATH_ROTATION rotation)
{
    Blackout();
    if (rotation != D3DKMDT_VPPR_UNINITIALIZED)
    {
        m_rotation = rotation;
    }
    return STATUS_SUCCESS;
}

bool GraphicMode::IsValid() const
{
    return GetPointer() != nullptr;
}

const DXGK_DISPLAY_INFORMATION& GraphicMode::GetMode() const
{
    return m_info;
}

void GraphicMode::GetMode(
    UINT* width,
    UINT* height,
    D3DDDIFORMAT* format,
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION* rotation,
    D3DKMDT_VIDPN_PRESENT_PATH_SCALING* scaling) const
{
    Set(width, m_info.Width);
    Set(height, m_info.Height);
    Set(format, m_info.ColorFormat);
    Set(rotation, m_rotation);
    Set(scaling, m_scaling);
}

void GraphicMode::Blackout()
{
    if (!m_blackouted)
    {
        void* ptr = GetPointer();
        if (ptr != 0)
        {
            size_t size = m_info.Pitch * m_info.Height;
            RtlZeroMemory(ptr, size);
        }

        m_blackouted = true;
    }
}

void GraphicMode::SetVisible(bool visible)
{
    if (!visible && m_visible)
    {
        Blackout();
    }

    m_visible = visible;
}

NTSTATUS GraphicMode::ExecutePresentDisplayOnly(
    void* source,
    UINT /*bpp*/,
    LONG pitch,
    ULONG numMoves,
    const D3DKMT_MOVE_RECT* moves,
    ULONG numDirtyRects,
    const RECT* dirtyRect,
    bool rotation)
{
    if (!m_visible)
    {
        return STATUS_SUCCESS;
    }

    auto destPtr = GetPointer();
    if(destPtr == nullptr)
    {
        //It is a normal situation for a virtual display
        return STATUS_SUCCESS;
    }

    UINT height = numDirtyRects == 0 || numMoves != 0 ? m_info.Height : 0;

    if (numMoves == 0)
    {
        for (ULONG i = 0; i < numDirtyRects; ++i)
        {
            const RECT& rect = dirtyRect[i];
            UINT rectBottom = rect.bottom;
            if (rectBottom > height)
            {
                height = rectBottom;
            }
        }
    }

    MdlGuard mdl;
    NTSTATUS status = mdl.Init(source, pitch * height);
    if (status != STATUS_SUCCESS)
    {
        return status;
    }

    HwExecutePresentDisplayOnly(
        destPtr,
        m_info.Pitch,
        BppFromPixelFormat(m_info.ColorFormat),
        rotation ? m_rotation : D3DKMDT_VPPR_IDENTITY,
        mdl.Get(),
        pitch,
        m_info.Width,
        m_info.Height,
        numMoves,
        moves,
        numDirtyRects,
        dirtyRect);

    m_blackouted = false;
    return STATUS_SUCCESS;
}

void GraphicMode::SystemWrite(void* source, UINT width, UINT height, UINT stride, INT x, INT y)
{
    // Rect will be Offset by PositionX/Y in the src to reset it back to 0
    RECT rect = {static_cast<LONG>(x), static_cast<LONG>(y), static_cast<LONG>(x + width), static_cast<LONG>(y + height)};

    BLT_INFO dest = {};
    dest.pBits = GetPointer();
    dest.Pitch = m_info.Pitch;
    dest.BitsPerPel = BppFromPixelFormat(m_info.ColorFormat);
    dest.Offset.x = 0;
    dest.Offset.y = 0;
    dest.Rotation = m_rotation;
    dest.Width = m_info.Width;
    dest.Height = m_info.Height;

    BLT_INFO src = {};
    src.pBits = source;
    src.Pitch = stride;
    src.BitsPerPel = 32;
    src.Offset.x = -x;
    src.Offset.y = -y;
    src.Rotation = D3DKMDT_VPPR_IDENTITY;
    src.Width = width;
    src.Height = height;

    BltBits(&dest, &src, 1, &rect);
}
