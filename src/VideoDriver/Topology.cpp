#include "stdafx.h"
#include "Topology.h"

Topology::Topology()
{ //TopologyWrapper has protected constructor
}

void Topology::Reset(const DXGK_VIDPNTOPOLOGY_INTERFACE* tinterface, D3DKMDT_HVIDPNTOPOLOGY topology)
{
    SetParams(tinterface, topology);
}

NTSTATUS Topology::AcquirePathInfo(
    D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
    D3DDDI_VIDEO_PRESENT_TARGET_ID targetId,
    TopologyPathGuard& guard)
{
    const D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr;
    NTSTATUS status = TopologyWrapper::AcquirePathInfo(sourceId, targetId, &path);
    if (NT_SUCCESS(status) && path != nullptr)
    {
        guard.Reset(this, path);
    }

    return status;
}

TopologyPathEnumerator Topology::EnumeratePaths()
{
    return TopologyPathEnumerator(this);
}

NTSTATUS Topology::CreateNewPathInfo(TopologyPathGuard& guard)
{
    D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr;
    NTSTATUS status = TopologyWrapper::CreateNewPathInfo(&path);
    if (NT_SUCCESS(status) && path != nullptr)
    {
        guard.Reset(this, path);
    }

    return status;
}
