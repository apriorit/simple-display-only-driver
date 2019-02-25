#pragma once

class TopologyWrapper
{
public:
    NTSTATUS GetNumPaths(SIZE_T* numPaths);
    NTSTATUS GetNumPathsFromSource(
        D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId, SIZE_T* numPathsFromSource);

    NTSTATUS EnumPathsTargetsFromSource(
        D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
        D3DKMDT_VIDPN_PRESENT_PATH_INDEX  pathIndex,
        D3DDDI_VIDEO_PRESENT_TARGET_ID* targetId);

    NTSTATUS GetPathsSourceFromTarget(
        D3DDDI_VIDEO_PRESENT_TARGET_ID targetId, D3DDDI_VIDEO_PRESENT_SOURCE_ID* sourceId);

    NTSTATUS AcquirePathInfo(
        D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId,
        D3DDDI_VIDEO_PRESENT_TARGET_ID targetId,
        const D3DKMDT_VIDPN_PRESENT_PATH** pathInfo);

    NTSTATUS AcquireFirstPathInfo(const D3DKMDT_VIDPN_PRESENT_PATH** firstPathInfo);
    NTSTATUS AcquireNextPathInfo(
        const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo,
        const D3DKMDT_VIDPN_PRESENT_PATH** nextPathInfo);

    NTSTATUS UpdatePathSupportInfo(const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo);
    NTSTATUS ReleasePathInfo(const D3DKMDT_VIDPN_PRESENT_PATH* pathInfo);
    NTSTATUS CreateNewPathInfo(D3DKMDT_VIDPN_PRESENT_PATH** pathInfo);
    NTSTATUS AddPath(D3DKMDT_VIDPN_PRESENT_PATH* pathInfo);
    NTSTATUS RemovePath(
        D3DDDI_VIDEO_PRESENT_SOURCE_ID sourceId, D3DDDI_VIDEO_PRESENT_TARGET_ID targetId);

protected:
    TopologyWrapper();

    void SetParams(const DXGK_VIDPNTOPOLOGY_INTERFACE* tinterface, D3DKMDT_HVIDPNTOPOLOGY topology);

private:
    const DXGK_VIDPNTOPOLOGY_INTERFACE* m_interface;
    D3DKMDT_HVIDPNTOPOLOGY m_topology;
};