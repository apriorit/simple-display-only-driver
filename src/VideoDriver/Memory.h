#pragma once

// Defaulting the value of PoolType means that any call to new Foo()
// will raise a compiler error for being ambiguous. This is to help keep
// any calls to allocate memory from accidentally NOT going through
// these functions.
_When_((PoolType & NonPagedPoolMustSucceed) != 0,
       __drv_reportError("Must succeed pool allocations are forbidden. "
       "Allocation failures cause a system crash"))
       void* __cdecl operator new(size_t size, POOL_TYPE poolType = PagedPool);
_When_((PoolType & NonPagedPoolMustSucceed) != 0,
       __drv_reportError("Must succeed pool allocations are forbidden. "
       "Allocation failures cause a system crash"))
       void* __cdecl operator new[](size_t size, POOL_TYPE poolType = PagedPool);
void  __cdecl operator delete(void* ptr);
void  __cdecl operator delete[](void* ptr);
