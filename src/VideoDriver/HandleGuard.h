#pragma once

/* Simple guard template.
 * Releases handle on destruction.
 */
template <typename HandleType, typename CleanupRoutine>
struct HandleGuard
{
public:
    HandleGuard(HandleType hndl, CleanupRoutine cleanup) : m_handle(hndl), m_cleanup(cleanup)
    {
    }

    // do not close returned handle
    HandleType Get()
    {
        return m_handle;
    }

    void Reset(HandleType hndl)
    {
        if (m_handle != NULL)
        {
            m_cleanup(m_handle);
        }

        m_handle = hndl;
    }

    HandleType Release()
    {
        HandleType oldHandle = m_handle;
        m_handle = NULL;
        
        return oldHandle;
    }

    ~HandleGuard()
    {
        if (m_handle != NULL)
        {
            m_cleanup(m_handle);
        }
        
    }
private:
    HandleType m_handle;
    CleanupRoutine m_cleanup;

    //Copy construction and copy assignment denied
    HandleGuard(const HandleGuard&);
    HandleGuard& operator=(const HandleGuard&);
};