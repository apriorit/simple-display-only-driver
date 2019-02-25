#include "stdafx.h"
#include "SourceModeSetCreator.h"

const DXGKDDI_VIDPN_ACQUIRESOURCEMODESET DXGK_VIDPN_INTERFACE::* detail::SourceCreatorDetail::acquire =
    &DXGK_VIDPN_INTERFACE::pfnAcquireSourceModeSet;

const DXGKDDI_VIDPN_CREATENEWSOURCEMODESET DXGK_VIDPN_INTERFACE::* detail::SourceCreatorDetail::create =
    &DXGK_VIDPN_INTERFACE::pfnCreateNewSourceModeSet;

const DXGKDDI_VIDPN_RELEASESOURCEMODESET DXGK_VIDPN_INTERFACE::* detail::SourceCreatorDetail::release =
    &DXGK_VIDPN_INTERFACE::pfnReleaseSourceModeSet;

const DXGKDDI_VIDPN_ASSIGNSOURCEMODESET DXGK_VIDPN_INTERFACE::* detail::SourceCreatorDetail::assign =
    &DXGK_VIDPN_INTERFACE::pfnAssignSourceModeSet;

const char detail::SourceCreatorDetail::name[] = "source";

SourceModeSetCreator::SourceModeSetCreator(
    const DXGK_VIDPN_INTERFACE* vidPnInterface,
    D3DKMDT_HVIDPN vidPn,
    D3DDDI_VIDEO_PRESENT_SOURCE_ID id)
    : BaseSetCreator(vidPnInterface, vidPn, id)
{
}
