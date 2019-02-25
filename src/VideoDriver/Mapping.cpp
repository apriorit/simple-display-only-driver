#include "stdafx.h"
#include "Mapping.h"

NTSTATUS MapFrameBuffer(PHYSICAL_ADDRESS physicalAddr, ULONG length, VOID** virtualAddr)
{
    PAGED_CODE();

    if (physicalAddr.QuadPart == 0 || length == 0 || virtualAddr == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    void* ptr = MmMapIoSpace(physicalAddr, length, MmWriteCombined);
    if (ptr == NULL)
    {
        // The underlying call to MmMapIoSpace failed. This may be because, MmWriteCombined
        // isn't supported, so try again with MmNonCached

        ptr = MmMapIoSpace(physicalAddr, length, MmNonCached);
        if (ptr == NULL)
        {
            return STATUS_NO_MEMORY;
        }
    }

    *virtualAddr = ptr;
    return STATUS_SUCCESS;
}

NTSTATUS UnmapFrameBuffer(VOID* virtualAddr, ULONG length)
{
    PAGED_CODE();

    if (virtualAddr == NULL && length == 0)
    {
        // Allow this function to be called when there's no work to do, and treat as successful
        return STATUS_SUCCESS;
    }
    else if (virtualAddr == NULL || length == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    MmUnmapIoSpace(virtualAddr, length);
    return STATUS_SUCCESS;
}

