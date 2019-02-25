#pragma once

template <class ModeSet, class Detail>
class ModeSetGuard
{
public:
    ModeSetGuard();
    ~ModeSetGuard();

    void Reset(const DXGK_VIDPN_INTERFACE* vidPnInterface,
        D3DKMDT_HVIDPN vidPn,
        const typename ModeSet::Interface* dinterface,
        typename ModeSet::Set set);

    void Release();

    ModeSet& operator*();
    ModeSet* operator->();

private:
    ModeSetGuard(const ModeSetGuard&);
    ModeSetGuard& operator=(const ModeSetGuard&);

private:
    void Clean();

private:
    const DXGK_VIDPN_INTERFACE* m_vidPnInterface;
    D3DKMDT_HVIDPN m_vidPn;
    ModeSet m_set;
};

template <class ModeSet, class Detail>
ModeSetGuard<ModeSet, Detail>::ModeSetGuard()
    : m_vidPnInterface(nullptr)
    , m_vidPn(NULL)
{
}

template <class ModeSet, class Detail>
ModeSetGuard<ModeSet, Detail>::~ModeSetGuard()
{
    Clean();
}

template <class ModeSet, class Detail>
void ModeSetGuard<ModeSet, Detail>::Reset(
    const DXGK_VIDPN_INTERFACE* vidPnInterface,
    D3DKMDT_HVIDPN vidPn,
    const typename ModeSet::Interface* dinterface,
    typename ModeSet::Set set)
{
    Clean();

    m_vidPnInterface = vidPnInterface;
    m_vidPn = vidPn;
    ASSERT(m_vidPnInterface != nullptr && m_vidPn != NULL);

    m_set.SetParams(dinterface, set);
}

template <class ModeSet, class Detail>
void ModeSetGuard<ModeSet, Detail>::Release()
{
    m_vidPnInterface = nullptr;
    m_vidPn = NULL;
    m_set.SetParams(nullptr, NULL);
}

template <class ModeSet, class Detail>
ModeSet& ModeSetGuard<ModeSet, Detail>::operator*()
{
    return m_set;
}

template <class ModeSet, class Detail>
ModeSet* ModeSetGuard<ModeSet, Detail>::operator->()
{
    return &m_set;
}

template <class ModeSet, class Detail>
void ModeSetGuard<ModeSet, Detail>::Clean()
{
    typename ModeSet::Set set = m_set.GetSet();
    if (set != NULL)
    {
        ASSERT(m_vidPnInterface != nullptr && m_vidPn != NULL);
        NTSTATUS status = (m_vidPnInterface->*Detail::release)(m_vidPn, set);
        if (!NT_SUCCESS(status))
        {
            //"ReleaseModeSet (%s) failed with status = 0x%lx, vidPn = 0x%p, sourceModeSet = 0x%p", Detail::name, status, m_vidPn, set;
        }
    }
}
