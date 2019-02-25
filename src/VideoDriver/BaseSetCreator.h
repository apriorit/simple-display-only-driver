#pragma once
#include "ModeSetGuard.h"

template <class ModeSet, class Detail>
class BaseSetCreator
{
private:
    typedef typename Detail::Id Id;

public:
    NTSTATUS Acquire();
    NTSTATUS Create();
    NTSTATUS Assign();

    ModeSet& operator*();
    ModeSet* operator->();

protected:
    BaseSetCreator(const DXGK_VIDPN_INTERFACE* vidPnInterface, D3DKMDT_HVIDPN vidPn, Id id);

private:
    template <class AcquireOrCreateFunc>
    NTSTATUS AcquireOrCreate(
        AcquireOrCreateFunc DXGK_VIDPN_INTERFACE::* acquireOrCreate, const char* operation);

private:
    const DXGK_VIDPN_INTERFACE* m_interface;
    D3DKMDT_HVIDPN m_vidPn;
    Id m_id;
    ModeSetGuard<ModeSet, Detail> m_set;
};

template <class ModeSet, class Detail>
BaseSetCreator<ModeSet, Detail>::BaseSetCreator(
    const DXGK_VIDPN_INTERFACE* vidPnInterface, D3DKMDT_HVIDPN vidPn, Id id)
    : m_interface(vidPnInterface)
    , m_vidPn(vidPn)
    , m_id(id)
{
    ASSERT(m_interface != nullptr && m_vidPn != NULL);
}

template <class ModeSet, class Detail>
NTSTATUS BaseSetCreator<ModeSet, Detail>::Acquire()
{
    return AcquireOrCreate(Detail::acquire, "AcquireModeSet");
}

template <class ModeSet, class Detail>
NTSTATUS BaseSetCreator<ModeSet, Detail>::Create()
{
    return AcquireOrCreate(Detail::create, "CreateModeSet");
}

template <class ModeSet, class Detail>
NTSTATUS BaseSetCreator<ModeSet, Detail>::Assign()
{
    NTSTATUS status = (m_interface->*Detail::assign)(m_vidPn, m_id, m_set->GetSet());
    if (NT_SUCCESS(status))
    {
        m_set.Release();
    }

    return status;
}

template <class ModeSet, class Detail>
ModeSet& BaseSetCreator<ModeSet, Detail>::operator*()
{
    return *m_set;
}

template <class ModeSet, class Detail>
ModeSet* BaseSetCreator<ModeSet, Detail>::operator->()
{
    return &*m_set;
}

template <class ModeSet, class Detail>
template <class AcquireOrCreateFunc>
NTSTATUS BaseSetCreator<ModeSet, Detail>::AcquireOrCreate(
    AcquireOrCreateFunc DXGK_VIDPN_INTERFACE::* acquireOrCreate, const char* /*operation*/)
{
    typename ModeSet::Set set = NULL;
    const typename ModeSet::Interface* dinterface = nullptr;
    NTSTATUS status = (m_interface->*acquireOrCreate)(m_vidPn, m_id, &set, &dinterface);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    m_set.Reset(m_interface, m_vidPn, dinterface, set);
    return status;
}
