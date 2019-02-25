#include "stdafx.h"
#include "TargetModeSetCreator.h"

const DXGKDDI_VIDPN_ACQUIRETARGETMODESET DXGK_VIDPN_INTERFACE::* detail::TargetCreatorDetail::acquire =
    &DXGK_VIDPN_INTERFACE::pfnAcquireTargetModeSet;

const DXGKDDI_VIDPN_CREATENEWTARGETMODESET DXGK_VIDPN_INTERFACE::* detail::TargetCreatorDetail::create =
    &DXGK_VIDPN_INTERFACE::pfnCreateNewTargetModeSet;

const DXGKDDI_VIDPN_RELEASETARGETMODESET DXGK_VIDPN_INTERFACE::* detail::TargetCreatorDetail::release =
    &DXGK_VIDPN_INTERFACE::pfnReleaseTargetModeSet;

const DXGKDDI_VIDPN_ASSIGNTARGETMODESET DXGK_VIDPN_INTERFACE::* detail::TargetCreatorDetail::assign =
    &DXGK_VIDPN_INTERFACE::pfnAssignTargetModeSet;

const char detail::TargetCreatorDetail::name[] = "target";

TargetModeSetCreator::TargetModeSetCreator(
    const DXGK_VIDPN_INTERFACE* vidPnInterface,
    D3DKMDT_HVIDPN vidPn,
    D3DDDI_VIDEO_PRESENT_TARGET_ID id)
    : BaseSetCreator(vidPnInterface, vidPn, id)
{
}
