#pragma once
#include "Topology.h"

class TopologyCreator
{
public:
    TopologyCreator(const DXGK_VIDPN_INTERFACE* vidPnInterface);

    NTSTATUS Get(D3DKMDT_HVIDPN vidPn);

    Topology* operator->();

private:
    const DXGK_VIDPN_INTERFACE* m_interface;
    Topology m_topology;
};