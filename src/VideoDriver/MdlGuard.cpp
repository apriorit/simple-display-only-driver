#include "stdafx.h"
#include "MdlGuard.h"

MdlGuard::MdlGuard()
    : m_mdl(nullptr)
    , m_locked(false)
    , m_addr(nullptr)
    , m_buffer0Byte(0)
{
}

MdlGuard::~MdlGuard()
{
    if (m_locked)
    {
        MmUnlockPages(m_mdl);
    }
    if (m_mdl)
    {
        IoFreeMdl(m_mdl);
    }
}

NTSTATUS MdlGuard::Init(void* buf, ULONG size, bool write /*= false*/)
{
    ASSERT(m_mdl == nullptr);

    if (size == 0)
    {
        m_addr = &m_buffer0Byte;
    }
    else if (buf)
    {
        m_mdl = IoAllocateMdl(buf, size, FALSE, FALSE, NULL);
        if (m_mdl == nullptr)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        __try
        {
            KPROCESSOR_MODE mode = static_cast<KPROCESSOR_MODE>(
                buf <= reinterpret_cast<const void*>(MM_USER_PROBE_ADDRESS) ? UserMode : KernelMode);

            MmProbeAndLockPages(m_mdl, mode, write ? IoModifyAccess : IoReadAccess);
            m_locked = true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return GetExceptionCode();
        }

        m_addr = MmGetSystemAddressForMdlSafe(m_mdl, NormalPagePriority);
    }

    return (m_addr == nullptr ? STATUS_INSUFFICIENT_RESOURCES : STATUS_SUCCESS);
}

void* MdlGuard::Get()
{
    ASSERT(m_addr != nullptr);
    return m_addr;
}
