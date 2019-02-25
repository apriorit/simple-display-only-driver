#pragma once

class MdlGuard
{
public:
    MdlGuard();
    ~MdlGuard();

    NTSTATUS Init(void* buf, ULONG size, bool write = false);

    void* Get();

private:
    MDL* m_mdl;
    bool m_locked;
    void* m_addr;
    char m_buffer0Byte;
};
