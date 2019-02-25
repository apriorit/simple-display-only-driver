#pragma once
#include "BaseModeSetWrapper.h"
#include "ModeGuard.h"

template <class Detail>
class BaseModeSet : public BaseModeSetWrapper<Detail>
{
public:
    typedef ModeGuard<Detail, NativeMode> Mode;
    typedef ModeGuard<Detail, const NativeMode> CMode;

public:
    NTSTATUS CreateMode(Mode& mode);
    NTSTATUS AddMode(Mode& mode);
};

template <class Detail>
NTSTATUS BaseModeSet<Detail>::CreateMode(Mode& guard)
{
    NativeMode* mode = nullptr;
    NTSTATUS status = CreateNewModeInfo(&mode);
    if (NT_SUCCESS(status))
    {
        guard.Reset(this, mode);
    }

    return status;
}

template <class Detail>
NTSTATUS BaseModeSet<Detail>::AddMode(Mode& mode)
{
    NTSTATUS status = BaseModeSetWrapper::AddMode(mode.Get());
    if (NT_SUCCESS(status))
    {
        mode.Release();
    }

    return status;
}
