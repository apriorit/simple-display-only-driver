#include "stdafx.h"
#include "MonitorModeSet.h"

const char detail::MonitorSetDetail::name[] = "monitor";

NTSTATUS MonitorModeSet::GetPreferredMode(CMode& guard)
{
    const D3DKMDT_MONITOR_SOURCE_MODE* mode = nullptr;
    NTSTATUS status = AcquirePreferredModeInfo(&mode);
    if (NT_SUCCESS(status) && mode != nullptr)
    {
        guard.Reset(this, mode);
    }

    return status;
}

NTSTATUS MonitorModeSet::AcquirePreferredModeInfo(const D3DKMDT_MONITOR_SOURCE_MODE** mode)
{
    ASSERT(GetInterface() != nullptr && GetSet() != NULL);
    return GetInterface()->pfnAcquirePreferredModeInfo(GetSet(), mode);
}
