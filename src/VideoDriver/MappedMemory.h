#pragma once

class MappedMemory
{
public:
    MappedMemory();
    ~MappedMemory();

    NTSTATUS Reset(const PHYSICAL_ADDRESS& addr, ULONG length);
    void* Get() const;

private:
    NTSTATUS Unmap();

private:
    void* m_memory;
    ULONG m_length;
};

