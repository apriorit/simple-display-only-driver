#include "stdafx.h"
#include "TopologyWrapper.h"

namespace
{
    template <class F, class P>
    NTSTATUS CallTopology(
        const DXGK_VIDPNTOPOLOGY_INTERFACE* ti,
        D3DKMDT_HVIDPNTOPOLOGY to,
        F DXGK_VIDPNTOPOLOGY_INTERFACE::*func,
        P p,
        const char* /*operation*/)
    {
        return (ti->*func)(to, p);
    }

    template <class F, class P1, class P2>
    NTSTATUS CallTopology(
        const DXGK_VIDPNTOPOLOGY_INTERFACE* ti,
        D3DKMDT_HVIDPNTOPOLOGY to,
        F DXGK_VIDPNTOPOLOGY_INTERFACE::*func,
        P1 p1,
        P2 p2,
        const char* /*operation*/)
    {
        return (ti->*func)(to, p1, p2);
    }

    template <class F, class P1, class P2, class P3>
    NTSTATUS CallTopology(
        const DXGK_VIDPNTOPOLOGY_INTERFACE* ti,
        D3DKMDT_HVIDPNTOPOLOGY to,
        F DXGK_VIDPNTOPOLOGY_INTERFACE::*func,
        P1 p1,
        P2 p2,
        P3 p3,
        const char* /*operation*/)
    {
        return (ti->*func)(to, p1, p2, p3);
    }
}

TopologyWrapper::TopologyWrapper()
    : m_interface(nullptr)
    , m_topology(NULL)
{
}

void TopologyWrapper::SetParams(
    const DXGK_VIDPNTOPOLOGY_INTERFACE* tinterface, D3DKMDT_HVIDPNTOPOLOGY topology)
{
    m_interface = tinterface;
    m_topology = topology;
}

NTSTATUS TopologyWrapper::GetNumPaths(SIZE_T* numPaths)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnGetNumPaths,
        numPaths,
        "GetNumPaths");
}

NTSTATUS TopologyWrapper::GetNumPathsFromSource(
    D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId, SIZE_T* numPathsFromSource)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnGetNumPathsFromSource,
        sourceId,
        numPathsFromSource,
        "GetNumPathsFromSource");
}

NTSTATUS TopologyWrapper::EnumPathsTargetsFromSource(
    D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
    D3DKMDT_VIDPN_PRESENT_PATH_INDEX pathIndex,
    D3DDDI_VIDEO_PRESENT_TARGET_ID* targetId)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnEnumPathTargetsFromSource,
        sourceId,
        pathIndex,
        targetId,
        "EnumPathsTargetsFromSource");
}

NTSTATUS TopologyWrapper::GetPathsSourceFromTarget(
    D3DDDI_VIDEO_PRESENT_TARGET_ID targetId, D3DDDI_VIDEO_PRESENT_SOURCE_ID* sourceId)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnGetPathSourceFromTarget,
        targetId,
        sourceId,
        "GetPathsSourceFromTarget");
}

NTSTATUS TopologyWrapper::AcquirePathInfo(
    D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
    D3DDDI_VIDEO_PRESENT_TARGET_ID targetId,
    const D3DKMDT_VIDPN_PRESENT_PATH** pathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnAcquirePathInfo,
        sourceId,
        targetId,
        pathInfo,
        "AcquirePathInfo");
}

NTSTATUS TopologyWrapper::AcquireFirstPathInfo(const D3DKMDT_VIDPN_PRESENT_PATH** firstPathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnAcquireFirstPathInfo,
        firstPathInfo,
        "AcquireFirstPathInfo");
}

NTSTATUS TopologyWrapper::AcquireNextPathInfo(
    const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo, const D3DKMDT_VIDPN_PRESENT_PATH** nextPathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnAcquireNextPathInfo,
        pathInfo,
        nextPathInfo,
        "AcquireNextPathInfo");
}

NTSTATUS TopologyWrapper::UpdatePathSupportInfo(const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnUpdatePathSupportInfo,
        pathInfo,
        "UpdatePathSupportInfo");
}

NTSTATUS TopologyWrapper::ReleasePathInfo(const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnReleasePathInfo,
        pathInfo,
        "ReleasePathInfo");
}

NTSTATUS TopologyWrapper::CreateNewPathInfo(D3DKMDT_VIDPN_PRESENT_PATH** pathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnCreateNewPathInfo,
        pathInfo,
        "CreateNewPathInfo");
}

NTSTATUS TopologyWrapper::AddPath(D3DKMDT_VIDPN_PRESENT_PATH* pathInfo)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnAddPath,
        pathInfo,
        "AddPath");
}

NTSTATUS TopologyWrapper::RemovePath(
    D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId, D3DDDI_VIDEO_PRESENT_TARGET_ID targetId)
{
    return CallTopology(
        m_interface,
        m_topology,
        &DXGK_VIDPNTOPOLOGY_INTERFACE::pfnRemovePath,
        sourceId,
        targetId,
        "RemovePath");
}
