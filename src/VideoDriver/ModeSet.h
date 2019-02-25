#pragma once
#include "BaseModeSet.h"

template <class Detail>
class ModeSet : public BaseModeSet<Detail>
{
public:
    typedef typename Detail::ModeId ModeId;

public:
    NTSTATUS GetPinnedMode(CMode& mode);
    NTSTATUS PinMode(ModeId id);

private:
    NTSTATUS AcquirePinnedModeInfo(const NativeMode** mode);
};

template <class Detail>
NTSTATUS ModeSet<Detail>::GetPinnedMode(CMode& guard)
{
    const NativeMode* mode = nullptr;
    NTSTATUS status = AcquirePinnedModeInfo(&mode);
    if (NT_SUCCESS(status) && mode != nullptr)
    {
        guard.Reset(this, mode);
    }

    return status;
}

template <class Detail>
NTSTATUS ModeSet<Detail>::PinMode(ModeId id)
{
    ASSERT(GetInterface() != nullptr && GetSet() != NULL);
    return GetInterface()->pfnPinMode(GetSet(), id);
}

template <class Detail>
NTSTATUS ModeSet<Detail>::AcquirePinnedModeInfo(const NativeMode** mode)
{
    ASSERT(GetInterface() != nullptr && GetSet() != NULL);
    return GetInterface()->pfnAcquirePinnedModeInfo(GetSet(), mode);
}
