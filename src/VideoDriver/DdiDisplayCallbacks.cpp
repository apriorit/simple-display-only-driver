#include "stdafx.h"
#include "DdiDisplayCallbacks.h"
#include "bdd.hxx"

NTSTATUS APIENTRY BddDdiQueryAdapterInfo(const HANDLE adapter, const DXGKARG_QUERYADAPTERINFO* info)
{
    PAGED_CODE();
    ASSERT(adapter != NULL);

    BASIC_DISPLAY_DRIVER* driver = reinterpret_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    return driver->QueryAdapterInfo(info);
}

NTSTATUS APIENTRY BddDdiSetPointerPosition(
    const HANDLE adapter, const DXGKARG_SETPOINTERPOSITION* pointer)
{
    PAGED_CODE();
    
    if (!AssertIsDriverActive(adapter))
    {
        return STATUS_UNSUCCESSFUL;
    }

    if (pointer->Flags.Visible)
    {
        ASSERT(false);
        return STATUS_UNSUCCESSFUL;
    }
    else
    {
        return STATUS_SUCCESS;
    }
}

NTSTATUS APIENTRY BddDdiSetPointerShape(
    const HANDLE adapter, const DXGKARG_SETPOINTERSHAPE* /*pointer*/)
{
    PAGED_CODE();

    if (!AssertIsDriverActive(adapter))
    {
        return STATUS_UNSUCCESSFUL;
    }
        
    ASSERT(false);
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS APIENTRY BddDdiPresentDisplayOnly(
    const HANDLE adapter, const DXGKARG_PRESENT_DISPLAYONLY* present)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->PresentDisplayOnly(present);
}

NTSTATUS APIENTRY BddDdiIsSupportedVidPn(const HANDLE adapter, DXGKARG_ISSUPPORTEDVIDPN* supported)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->IsSupportedVidPn(supported);
}

NTSTATUS APIENTRY BddDdiRecommendFunctionalVidPn(
    const HANDLE adapter, const DXGKARG_RECOMMENDFUNCTIONALVIDPN* const /*recommend*/)
{
    PAGED_CODE();

    if (!AssertIsDriverActive(adapter))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_GRAPHICS_NO_RECOMMENDED_FUNCTIONAL_VIDPN;
}

NTSTATUS APIENTRY BddDdiRecommendMonitorModes(
    const HANDLE adapter, const DXGKARG_RECOMMENDMONITORMODES* const recommend)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->RecommendMonitorModes(recommend);
}

NTSTATUS APIENTRY BddDdiEnumVidPnCofuncModality(
    const HANDLE adapter, const DXGKARG_ENUMVIDPNCOFUNCMODALITY* const modality)
{
    PAGED_CODE();
    
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->EnumVidPnCofuncModality(modality);
}

NTSTATUS APIENTRY BddDdiSetVidPnSourceVisibility(
    const HANDLE adapter, CONST DXGKARG_SETVIDPNSOURCEVISIBILITY* visibility)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->SetVidPnSourceVisibility(visibility);
}

NTSTATUS APIENTRY BddDdiCommitVidPn(const HANDLE adapter, const DXGKARG_COMMITVIDPN* const commit)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->CommitVidPn(commit);
}

NTSTATUS APIENTRY BddDdiUpdateActiveVidPnPresentPath(
    const HANDLE adapter, const DXGKARG_UPDATEACTIVEVIDPNPRESENTPATH* const path)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->UpdateActiveVidPnPresentPath(path);
}

NTSTATUS APIENTRY BddDdiQueryVidPnHWCapability(
    const HANDLE adapter, DXGKARG_QUERYVIDPNHWCAPABILITY* caps)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->QueryVidPnHWCapability(caps);
}
