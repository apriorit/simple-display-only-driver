#include "stdafx.h"
#include "Memory.h"

static const ULONG tag = 'SDDt';

static void* Allocate(size_t size, POOL_TYPE pool)
{
    size = (size == 0) ? 1 : size;

    void* ptr = ExAllocatePoolWithTag(pool, size, tag);

#if DBG
    if (ptr != NULL)
    {
        RtlFillMemory(ptr, size, 0xCD);
    }
#endif

    return ptr;
}

static void Deallocate(void* ptr)
{
    if (ptr != nullptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}

_When_((PoolType & NonPagedPoolMustSucceed) != 0,
    __drv_reportError("Must succeed pool allocations are forbidden. "
            "Allocation failures cause a system crash"))
void* __cdecl operator new(size_t size, POOL_TYPE poolType)
{
    return Allocate(size, poolType);
}

_When_((PoolType & NonPagedPoolMustSucceed) != 0,
    __drv_reportError("Must succeed pool allocations are forbidden. "
            "Allocation failures cause a system crash"))
void* __cdecl operator new[](size_t size, POOL_TYPE poolType)
{
    return Allocate(size, poolType);
}

void __cdecl operator delete(void* ptr)
{
    Deallocate(ptr);
}

void __cdecl operator delete[](void* ptr)
{
    Deallocate(ptr);
}
