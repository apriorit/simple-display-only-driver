#pragma once

//Mode can be const or non-const
template <class Detail, class Mode>
class ModeGuard
{
public:
    typedef BaseModeSetWrapper<Detail> Set;

    ModeGuard(Set* set = nullptr, Mode* mode = nullptr);
    ~ModeGuard();

    Mode* operator->() const;

    Mode* Get() const;
    Mode* Release();
    void Reset(Set* set = nullptr, Mode* mode = nullptr);

private:
    void Clean();

private:
    ModeGuard(const ModeGuard&);
    ModeGuard& operator=(const ModeGuard&);

private:
    Set* m_set;
    Mode* m_mode;
};

template <class Detail, class Mode>
ModeGuard<Detail, Mode>::ModeGuard(Set* set /*= nullptr*/, Mode* mode /*= nullptr*/)
    : m_set(set)
    , m_mode(mode)
{
    ASSERT((m_set == nullptr && m_mode == nullptr) || (m_set != nullptr && m_mode != nullptr));
}

template <class Detail, class Mode>
ModeGuard<Detail, Mode>::~ModeGuard()
{
    Clean();
}

template <class Detail, class Mode>
Mode* ModeGuard<Detail, Mode>::operator->() const
{
    ASSERT(m_mode != nullptr);
    return Get();
}

template <class Detail, class Mode>
Mode* ModeGuard<Detail, Mode>::Get() const
{
    return m_mode;
}

template <class Detail, class Mode>
Mode* ModeGuard<Detail, Mode>::Release()
{
    Mode* mode = m_mode;

    m_set = nullptr;
    m_mode = nullptr;

    return mode;
}

template <class Detail, class Mode>
void ModeGuard<Detail, Mode>::Reset(Set* set /*= nullptr*/, Mode* mode /*= nullptr*/)
{
    Clean();
    m_set = set;
    m_mode = mode;

    ASSERT((m_set == nullptr && m_mode == nullptr) || (m_set != nullptr && m_mode != nullptr));
}

template <class Detail, class Mode>
void ModeGuard<Detail, Mode>::Clean()
{
    if (m_mode != nullptr)
    {
        ASSERT(m_set != nullptr);
        m_set->ReleaseModeInfo(m_mode);
    }
}
