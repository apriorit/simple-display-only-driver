#include "stdafx.h"
#include "DdiPnpCallbacks.h"
#include "Memory.h"
#include "bdd.hxx"

VOID BddDdiUnload()
{
    PAGED_CODE();
}

NTSTATUS BddDdiAddDevice(DEVICE_OBJECT* device, PVOID* context)
{
    PAGED_CODE();

    if (device == nullptr || context == nullptr)
    {
        return STATUS_INVALID_PARAMETER;
    }


    BASIC_DISPLAY_DRIVER* driver = new(NonPagedPoolNx) BASIC_DISPLAY_DRIVER(device);
    *context = driver;

    if (driver == nullptr)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

NTSTATUS BddDdiRemoveDevice(void* context)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);
    delete driver;

    return STATUS_SUCCESS;
}

NTSTATUS BddDdiStartDevice(
    void* context,
    DXGK_START_INFO* startInfo,
    DXGKRNL_INTERFACE* dxgkInterface,
    ULONG* views,
    ULONG* children)
{
    PAGED_CODE();

    ASSERT(context != nullptr);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);

    return driver->StartDevice(startInfo, dxgkInterface, views, children);
}

NTSTATUS BddDdiStopDevice(void* context)
{
    PAGED_CODE();

    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);

    return driver->StopDevice();
}

NTSTATUS BddDdiDispatchIoRequest(void* context, ULONG /*source*/, VIDEO_REQUEST_PACKET* /*packet*/)
{
    PAGED_CODE();

    if (!AssertIsDriverActive(context))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS BddDdiSetPowerState(
    void* context, ULONG hardwareUid, DEVICE_POWER_STATE deviceState, POWER_ACTION action)
{
    PAGED_CODE();

    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);
    if (!driver->IsDriverActive())
    {
        // If the driver isn't active, SetPowerState can still be called, however in BDD's case
        // this shouldn't do anything, as it could for instance be called on BDD Fallback after
        // Fallback has been stopped and BDD PnP is being started. Fallback doesn't have control
        // of the hardware in this case.
        return STATUS_SUCCESS;
    }

    return driver->SetPowerState(hardwareUid, deviceState, action);
}

NTSTATUS BddDdiQueryChildRelations(
    void* context, DXGK_CHILD_DESCRIPTOR* childRelations, ULONG childRelationsSize)
{
    PAGED_CODE();

    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);

    return driver->QueryChildRelations(childRelations, childRelationsSize);
}

NTSTATUS BddDdiQueryChildStatus(
    void* context, DXGK_CHILD_STATUS* childStatus, BOOLEAN nonDestructiveOnly)
{
    PAGED_CODE();

    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);

    return driver->QueryChildStatus(childStatus, nonDestructiveOnly);
}

NTSTATUS BddDdiQueryDeviceDescriptor(
    void* context, ULONG /*child*/, DXGK_DEVICE_DESCRIPTOR* /*descriptor*/)
{
    PAGED_CODE();

    if (!AssertIsDriverActive(context))
    {
        return STATUS_UNSUCCESSFUL;
    }
    
    return STATUS_GRAPHICS_CHILD_DESCRIPTOR_NOT_SUPPORTED;
}

NTSTATUS APIENTRY BddDdiStopDeviceAndReleasePostDisplayOwnership(
    void* context, D3DDDI_VIDEO_PRESENT_TARGET_ID target, DXGK_DISPLAY_INFORMATION* info)
{
    PAGED_CODE();

    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = reinterpret_cast<BASIC_DISPLAY_DRIVER*>(context);

    return driver->StopDeviceAndReleasePostDisplayOwnership(target, info);
}

BOOLEAN BddDdiInterruptRoutine(void* /*context*/, ULONG /*message*/)
{
    return FALSE;
}

void BddDdiDpcRoutine(void* context)
{
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);
    if (!AssertIsDriverActive(driver))
    {
        return;
    }
    
    driver->DpcRoutine();
}

void BddDdiResetDevice(void* /*context*/)
{
}

NTSTATUS APIENTRY BddDdiSystemDisplayEnable(
    void* context,
    D3DDDI_VIDEO_PRESENT_TARGET_ID target,
    DXGKARG_SYSTEM_DISPLAY_ENABLE_FLAGS* flags,
    UINT* width,
    UINT* height,
    D3DDDIFORMAT* format)
{
    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);
    return driver->SystemDisplayEnable(target, flags, width, height, format);
}

void APIENTRY BddDdiSystemDisplayWrite(
    void* context, void* source, UINT width, UINT height, UINT stride, UINT x, UINT y)
{
    ASSERT(context != NULL);
    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(context);
    driver->SystemDisplayWrite(source, width, height, stride, x, y);
}

NTSTATUS APIENTRY BddDdiEscape(const HANDLE adapter, const DXGKARG_ESCAPE* escape)
{
    PAGED_CODE();

    BASIC_DISPLAY_DRIVER* driver = static_cast<BASIC_DISPLAY_DRIVER*>(adapter);
    if (!AssertIsDriverActive(driver))
    {
        return STATUS_UNSUCCESSFUL;
    }

    return driver->Escape(escape);
}