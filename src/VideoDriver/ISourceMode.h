#pragma once
#include "ScopedArray.h"
#include "Mode.h"

class ISourceMode
{
public:
    virtual ~ISourceMode(){}
    virtual ScopedArray<Mode>& GetAvailableSourceMode(UINT& count) = 0;
};