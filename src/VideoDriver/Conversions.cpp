#include "stdafx.h"
#include "Conversions.h"


UINT BppFromPixelFormat(D3DDDIFORMAT Format)
{
    switch (Format)
    {
    case D3DDDIFMT_UNKNOWN:
        return 0;
    case D3DDDIFMT_P8:
        return 8;
    case D3DDDIFMT_R5G6B5:
        return 16;
    case D3DDDIFMT_R8G8B8:
        return 24;
    case D3DDDIFMT_X8R8G8B8:
    case D3DDDIFMT_A8R8G8B8:
        return 32;

    default:
        ASSERT(false);
        return 0;
    }
}

UINT BytesFromPixelFormat(D3DDDIFORMAT format)
{
    return BppFromPixelFormat(format) / 8;
}

D3DDDIFORMAT PixelFormatFromBpp(UINT bpp)
{
    switch (bpp)
    {
    case 8:
        return D3DDDIFMT_P8;
    case 16:
        return D3DDDIFMT_R5G6B5;
    case 24:
        return D3DDDIFMT_R8G8B8;
    case 32:
        return D3DDDIFMT_X8R8G8B8;

    default:
        ASSERT(false);
        return D3DDDIFMT_UNKNOWN;
    }
}
