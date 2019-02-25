#pragma once

template <class T>
class ScopedArray
{
public:
    ScopedArray(T* ptr = nullptr);
    ~ScopedArray();

    void Reset(T* ptr = nullptr);
    void Release();

    T* Get() const;

private:
    void Clean();

private:
    ScopedArray(const ScopedArray&);
    ScopedArray& operator=(const ScopedArray&);

private:
    T* m_ptr;
};

template <class T>
ScopedArray<T>::ScopedArray(T* ptr /*= nullptr*/)
    : m_ptr(ptr)
{
}

template <class T>
ScopedArray<T>::~ScopedArray()
{
    Clean();
}

template <class T>
void ScopedArray<T>::Reset(T* ptr /*= nullptr*/)
{
    Clean();
    m_ptr = ptr;
}

template <class T>
void ScopedArray<T>::Release()
{
    m_ptr = nullptr;
}

template <class T>
T* ScopedArray<T>::Get() const
{
    return m_ptr;
}

template <class T>
void ScopedArray<T>::Clean()
{
    typedef char typeMustBeComplete[sizeof(T) ? 1 : -1];
    (void) sizeof(typeMustBeComplete);
    delete[] m_ptr;
}