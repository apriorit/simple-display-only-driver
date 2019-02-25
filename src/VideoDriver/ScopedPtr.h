#pragma once

template <class T>
class ScopedPtr
{
public:
    ScopedPtr(T* ptr = nullptr);
    ~ScopedPtr();

    void Reset(T* ptr = nullptr);
    void Release();

    T* Get() const;
    T* operator->() const;

private:
    void Clean();

private:
    ScopedPtr(const ScopedPtr&);
    ScopedPtr& operator=(const ScopedPtr&);

private:
    T* m_ptr;
};

template <class T>
ScopedPtr<T>::ScopedPtr(T* ptr /*= nullptr*/)
    : m_ptr(ptr)
{
}

template <class T>
ScopedPtr<T>::~ScopedPtr()
{
    Clean();
}

template <class T>
void ScopedPtr<T>::Reset(T* ptr /*= nullptr*/)
{
    Clean();
    m_ptr = ptr;
}

template <class T>
void ScopedPtr<T>::Release()
{
    m_ptr = nullptr;
}

template <class T>
T* ScopedPtr<T>::Get() const
{
    return m_ptr;
}

template <class T>
T* ScopedPtr<T>::operator->() const
{
    ASSERT(m_ptr != nullptr);
    return m_ptr;
}

template <class T>
void ScopedPtr<T>::Clean()
{
    typedef char typeMustBeComplete[sizeof(T) ? 1 : -1];
    (void) sizeof(typeMustBeComplete);
    delete m_ptr;
}
