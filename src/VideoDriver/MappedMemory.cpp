#include "stdafx.h"
#include "MappedMemory.h"
#include "Mapping.h"

MappedMemory::MappedMemory()
    : m_memory(nullptr)
    , m_length(0)
{
}

MappedMemory::~MappedMemory()
{
    Unmap();
}

NTSTATUS MappedMemory::Reset(const PHYSICAL_ADDRESS& addr, ULONG length)
{
    NTSTATUS status = Unmap();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    m_length = length;
    return m_length == 0 ? STATUS_SUCCESS : MapFrameBuffer(addr, length, &m_memory);
}

void* MappedMemory::Get() const
{
    return m_memory;
}

NTSTATUS MappedMemory::Unmap()
{
    NTSTATUS status = STATUS_SUCCESS;

    if (m_memory != nullptr)
    {
        status = UnmapFrameBuffer(m_memory, m_length);
        m_memory = nullptr;
    }

    return status;
}
