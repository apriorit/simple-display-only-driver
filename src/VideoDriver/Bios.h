#pragma once
#include "ScopedArray.h"
#include "ScopedPtr.h"

bool Isx86BiosFunctionsAvailable();
NTSTATUS GetVideoMemorySize(unsigned long& size);
NTSTATUS SetModeBIOS(ULONG mode);
NTSTATUS GetVideoModeInfoFromBIOS(
    ScopedArray<VIDEO_MODE_INFORMATION>& videoModeInfo, 
    ScopedArray<USHORT>& modeNumbers, 
    USHORT& modeCount, 
    USHORT bitsPerPixel);
