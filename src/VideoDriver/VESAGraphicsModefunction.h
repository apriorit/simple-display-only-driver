#pragma once
#include <ntdef.h>

//http://www.monstersoft.com/tutorial1/VESA_intro.html
enum VESAGraphicsModeFunction : ULONG
{
    GetVESAInfoFunction = 0x4F00,
    GetVESAModeInfoFunction = 0x4F01,
    SetVideoModeFunction = 0x4f02,
};