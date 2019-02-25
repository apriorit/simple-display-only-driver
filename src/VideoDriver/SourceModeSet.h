#pragma once
#include "ModeSet.h"

namespace detail
{
    struct SourceModeSetDetail
    {
        typedef DXGK_VIDPNSOURCEMODESET_INTERFACE    Interface;
        typedef D3DKMDT_HVIDPNSOURCEMODESET          Set;
        typedef D3DKMDT_VIDPN_SOURCE_MODE            Mode;
        typedef D3DKMDT_VIDEO_PRESENT_SOURCE_MODE_ID ModeId;

        static const char name[];
    };
}


class SourceModeSet : public ModeSet<detail::SourceModeSetDetail>
{
};
