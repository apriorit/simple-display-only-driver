#pragma once
#include "TopologyWrapper.h"
#include "TopologyPathEnumerator.h"

class Topology : public TopologyWrapper
{
public:
    Topology();

    void Reset(const DXGK_VIDPNTOPOLOGY_INTERFACE* tinterface, D3DKMDT_HVIDPNTOPOLOGY topology);

    NTSTATUS AcquirePathInfo(
        D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
        D3DDDI_VIDEO_PRESENT_TARGET_ID targetId,
        TopologyPathGuard& path);

    TopologyPathEnumerator EnumeratePaths();

    NTSTATUS CreateNewPathInfo(TopologyPathGuard& path);
};

