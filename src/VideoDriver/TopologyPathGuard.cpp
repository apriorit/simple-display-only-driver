#include "stdafx.h"
#include "TopologyPathGuard.h"
#include "TopologyWrapper.h"

TopologyPathGuard::TopologyPathGuard(
    TopologyWrapper* wrapper /*= nullptr*/, const D3DKMDT_VIDPN_PRESENT_PATH* path /*= nullptr*/)
    : m_wrapper(wrapper)
    , m_path(path)
{
    ASSERT((m_wrapper == nullptr && m_path == nullptr) || (m_wrapper != nullptr && m_path != nullptr));
}

TopologyPathGuard::~TopologyPathGuard()
{
    Clean();
}

const D3DKMDT_VIDPN_PRESENT_PATH* TopologyPathGuard::operator->() const
{
    ASSERT(m_path != nullptr);
    return Get();
}

const D3DKMDT_VIDPN_PRESENT_PATH& TopologyPathGuard::operator*() const
{
    ASSERT(m_path != nullptr);
    return *Get();
}

const D3DKMDT_VIDPN_PRESENT_PATH* TopologyPathGuard::Get() const
{
    return m_path;
}

const D3DKMDT_VIDPN_PRESENT_PATH* TopologyPathGuard::Release()
{
    const D3DKMDT_VIDPN_PRESENT_PATH* path = m_path;

    m_wrapper = nullptr;
    m_path = nullptr;

    return path;
}

void TopologyPathGuard::Reset(
    TopologyWrapper* wrapper /*= nullptr*/, const D3DKMDT_VIDPN_PRESENT_PATH* path /*= nullptr*/)
{
    Clean();
    m_wrapper = wrapper;
    m_path = path;

    ASSERT((m_wrapper == nullptr && m_path == nullptr) || (m_wrapper != nullptr && m_path != nullptr));
}

void TopologyPathGuard::Clean()
{
    if (m_path != nullptr)
    {
        ASSERT(m_wrapper != nullptr);
        m_wrapper->ReleasePathInfo(m_path);
    }
}
