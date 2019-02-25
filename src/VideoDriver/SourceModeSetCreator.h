#pragma once
#include "BaseSetCreator.h"
#include "SourceModeSet.h"

namespace detail
{
    struct SourceCreatorDetail
    {
        typedef D3DDDI_VIDEO_PRESENT_SOURCE_ID Id;

        static const DXGKDDI_VIDPN_ACQUIRESOURCEMODESET   DXGK_VIDPN_INTERFACE::* acquire;
        static const DXGKDDI_VIDPN_CREATENEWSOURCEMODESET DXGK_VIDPN_INTERFACE::* create;
        static const DXGKDDI_VIDPN_RELEASESOURCEMODESET   DXGK_VIDPN_INTERFACE::* release;
        static const DXGKDDI_VIDPN_ASSIGNSOURCEMODESET    DXGK_VIDPN_INTERFACE::* assign;

        static const char name[];
    };
}

class SourceModeSetCreator : public BaseSetCreator<SourceModeSet, detail::SourceCreatorDetail>
{
public:
    SourceModeSetCreator(
        const DXGK_VIDPN_INTERFACE* vidPnInterface,
        D3DKMDT_HVIDPN vidPn,
        D3DDDI_VIDEO_PRESENT_SOURCE_ID id);
};
