#include "stdafx.h"
#include "DdiPnpCallbacks.h"
#include "DdiDisplayCallbacks.h"

extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" NTSTATUS DriverEntry(DRIVER_OBJECT* driverObject, UNICODE_STRING* registryPath)
{
    PAGED_CODE();

    KMDDOD_INITIALIZATION_DATA data = {DXGKDDI_INTERFACE_VERSION};
    data.DxgkDdiAddDevice                                = BddDdiAddDevice;
    data.DxgkDdiStartDevice                              = BddDdiStartDevice;
    data.DxgkDdiStopDevice                               = BddDdiStopDevice;
    data.DxgkDdiResetDevice                              = BddDdiResetDevice;
    data.DxgkDdiRemoveDevice                             = BddDdiRemoveDevice;
    data.DxgkDdiDispatchIoRequest                        = BddDdiDispatchIoRequest;
    data.DxgkDdiInterruptRoutine                         = BddDdiInterruptRoutine;
    data.DxgkDdiDpcRoutine                               = BddDdiDpcRoutine;
    data.DxgkDdiQueryChildRelations                      = BddDdiQueryChildRelations;
    data.DxgkDdiQueryChildStatus                         = BddDdiQueryChildStatus;
    data.DxgkDdiQueryDeviceDescriptor                    = BddDdiQueryDeviceDescriptor;
    data.DxgkDdiSetPowerState                            = BddDdiSetPowerState;
    data.DxgkDdiUnload                                   = BddDdiUnload;
    data.DxgkDdiQueryAdapterInfo                         = BddDdiQueryAdapterInfo;
    data.DxgkDdiSetPointerPosition                       = BddDdiSetPointerPosition;
    data.DxgkDdiSetPointerShape                          = BddDdiSetPointerShape;
    data.DxgkDdiIsSupportedVidPn                         = BddDdiIsSupportedVidPn;
    data.DxgkDdiRecommendFunctionalVidPn                 = BddDdiRecommendFunctionalVidPn;
    data.DxgkDdiEnumVidPnCofuncModality                  = BddDdiEnumVidPnCofuncModality;
    data.DxgkDdiSetVidPnSourceVisibility                 = BddDdiSetVidPnSourceVisibility;
    data.DxgkDdiCommitVidPn                              = BddDdiCommitVidPn;
    data.DxgkDdiUpdateActiveVidPnPresentPath             = BddDdiUpdateActiveVidPnPresentPath;
    data.DxgkDdiRecommendMonitorModes                    = BddDdiRecommendMonitorModes;
    data.DxgkDdiQueryVidPnHWCapability                   = BddDdiQueryVidPnHWCapability;
    data.DxgkDdiPresentDisplayOnly                       = BddDdiPresentDisplayOnly;
    data.DxgkDdiStopDeviceAndReleasePostDisplayOwnership = BddDdiStopDeviceAndReleasePostDisplayOwnership;
    data.DxgkDdiSystemDisplayEnable                      = BddDdiSystemDisplayEnable;
    data.DxgkDdiSystemDisplayWrite                       = BddDdiSystemDisplayWrite;
    data.DxgkDdiEscape                                   = BddDdiEscape;

    return DxgkInitializeDisplayOnlyDriver(driverObject, registryPath, &data);
}
