#include "stdafx.h"
#include "TopologyPathEnumerator.h"
#include "TopologyWrapper.h"

TopologyPathEnumerator::TopologyPathEnumerator(TopologyWrapper* wrapper)
    : m_wrapper(wrapper)
{
    ASSERT(m_wrapper != nullptr);

    const D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr;
    m_status = m_wrapper->AcquireFirstPathInfo(&path);
    Acquire(path);
}

bool TopologyPathEnumerator::End() const
{
    return !NT_SUCCESS(m_status) ||
        m_status == STATUS_GRAPHICS_NO_MORE_ELEMENTS_IN_DATASET ||
        m_status == STATUS_GRAPHICS_DATASET_IS_EMPTY;
}

void TopologyPathEnumerator::Next()
{
    ASSERT(m_path.Get() != nullptr);

    const D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr;
    m_status = m_wrapper->AcquireNextPathInfo(m_path.Get(), &path);
    Acquire(path);
}

TopologyPathGuard& TopologyPathEnumerator::GetPath()
{
    return m_path;
}

NTSTATUS TopologyPathEnumerator::GetStatus() const
{
    return m_status;
}

void TopologyPathEnumerator::Acquire(const D3DKMDT_VIDPN_PRESENT_PATH* path)
{
    if (NT_SUCCESS(m_status) && path != nullptr)
    {
        m_path.Reset(m_wrapper, path);
    }
}
