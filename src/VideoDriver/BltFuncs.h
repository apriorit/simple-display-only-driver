#pragma once

typedef struct _BLT_INFO
{
    PVOID pBits;
    UINT Pitch;
    UINT BitsPerPel;
    POINT Offset; // To unrotated top-left of dirty rects
    D3DKMDT_VIDPN_PRESENT_PATH_ROTATION Rotation;
    UINT Width; // For the unrotated image
    UINT Height; // For the unrotated image
} BLT_INFO;

// Must be Non-Paged
void BltBits(BLT_INFO* dest, const BLT_INFO* source, UINT numRects, const RECT* rects);
