#pragma once
#include "BaseSetCreator.h"
#include "TargetModeSet.h"

namespace detail
{
    struct TargetCreatorDetail
    {
        typedef D3DDDI_VIDEO_PRESENT_TARGET_ID       Id;

        static const DXGKDDI_VIDPN_ACQUIRETARGETMODESET   DXGK_VIDPN_INTERFACE::* acquire;
        static const DXGKDDI_VIDPN_CREATENEWTARGETMODESET DXGK_VIDPN_INTERFACE::* create;
        static const DXGKDDI_VIDPN_RELEASETARGETMODESET   DXGK_VIDPN_INTERFACE::* release;
        static const DXGKDDI_VIDPN_ASSIGNTARGETMODESET    DXGK_VIDPN_INTERFACE::* assign;

        static const char name[];
    };
}

class TargetModeSetCreator : public BaseSetCreator<TargetModeSet, detail::TargetCreatorDetail>
{
public:
    TargetModeSetCreator(
        const DXGK_VIDPN_INTERFACE* vidPnInterface,
        D3DKMDT_HVIDPN vidPn,
        D3DDDI_VIDEO_PRESENT_TARGET_ID id);
};
