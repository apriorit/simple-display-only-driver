#pragma once
#include "ModeSet.h"

namespace detail
{
    struct TargetModeSetDetail
    {
        typedef DXGK_VIDPNTARGETMODESET_INTERFACE    Interface;
        typedef D3DKMDT_HVIDPNTARGETMODESET          Set;
        typedef D3DKMDT_VIDPN_TARGET_MODE            Mode;
        typedef D3DKMDT_VIDEO_PRESENT_TARGET_MODE_ID ModeId;

        static const char name[];
    };
}

class TargetModeSet : public ModeSet<detail::TargetModeSetDetail>
{
};
