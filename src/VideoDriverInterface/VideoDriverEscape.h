#pragma once
#include <windef.h>

enum EscapeCode
{
    EscapeInvalid = 0,
    EscapeCanApplyResolution = 1
};

struct BaseEscape
{
    UINT32 code;
};

struct CanApplyResolution
{
    BaseEscape code;
    bool canApply;
};

inline void PrepareCanApplyResolutionEscape(CanApplyResolution& escape, bool canApply)
{
    escape.code.code = EscapeCanApplyResolution;
    escape.canApply = canApply;
}
