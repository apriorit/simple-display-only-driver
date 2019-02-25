#include "stdafx.h"
#include "TopologyCreator.h"

TopologyCreator::TopologyCreator(const DXGK_VIDPN_INTERFACE* vidPnInterface)
    : m_interface(vidPnInterface)
{
}

NTSTATUS TopologyCreator::Get(D3DKMDT_HVIDPN vidPn)
{
    D3DKMDT_HVIDPNTOPOLOGY topology = NULL;
    const DXGK_VIDPNTOPOLOGY_INTERFACE* tinterface = nullptr;
    NTSTATUS status = m_interface->pfnGetTopology(vidPn, &topology, &tinterface);
    if (NT_SUCCESS(status))
    {
        m_topology.Reset(tinterface, topology);
    }

    return status;
}

Topology* TopologyCreator::operator->()
{
    return &m_topology;
}
