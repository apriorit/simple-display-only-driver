#pragma once

template <class Detail>
class BaseModeSetWrapper
{
public:
    typedef Detail Detail;
    typedef typename Detail::Interface Interface;
    typedef typename Detail::Set Set;
    typedef typename Detail::Mode NativeMode;

public:
    NTSTATUS GetNumModes(size_t* modes);
    NTSTATUS AcquireFirstModeInfo(const NativeMode** mode);
    NTSTATUS AcquireNextModeInfo(const NativeMode* mode, const NativeMode** next);
    NTSTATUS ReleaseModeInfo(const NativeMode* mode);
    NTSTATUS CreateNewModeInfo(NativeMode** mode);
    NTSTATUS AddMode(NativeMode* mode);

    void SetParams(const Interface* dinterface, Set set);
    Set GetSet() const;

protected:
    BaseModeSetWrapper();

    const Interface* GetInterface() const;

private:
    const Interface* m_interface;
    Set m_set;
};

template <class Detail>
BaseModeSetWrapper<Detail>::BaseModeSetWrapper()
    : m_interface(nullptr)
    , m_set(NULL)
{
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::GetNumModes(size_t* modes)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnGetNumModes(m_set, modes);
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::AcquireFirstModeInfo(const NativeMode** mode)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnAcquireFirstModeInfo(m_set, mode);
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::AcquireNextModeInfo(const NativeMode* mode, const NativeMode** next)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnAcquireNextModeInfo(m_set, mode, next);
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::ReleaseModeInfo(const NativeMode* mode)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnReleaseModeInfo(m_set, mode);
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::CreateNewModeInfo(NativeMode** mode)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnCreateNewModeInfo(m_set, mode);
}

template <class Detail>
NTSTATUS BaseModeSetWrapper<Detail>::AddMode(NativeMode* mode)
{
    ASSERT(m_interface != nullptr && m_set != NULL);
    return m_interface->pfnAddMode(m_set, mode);
}

template <class Detail>
void BaseModeSetWrapper<Detail>::SetParams(const Interface* dinterface, Set set)
{
    m_interface = dinterface;
    m_set = set;
}

template <class Detail>
const typename BaseModeSetWrapper<Detail>::Interface* BaseModeSetWrapper<Detail>::GetInterface() const
{
    return m_interface;
}

template <class Detail>
typename BaseModeSetWrapper<Detail>::Set BaseModeSetWrapper<Detail>::GetSet() const
{
    return m_set;
}
