#pragma once

class TopologyWrapper;
typedef struct _D3DKMDT_VIDPN_PRESENT_PATH D3DKMDT_VIDPN_PRESENT_PATH;

class TopologyPathGuard
{
public:
    TopologyPathGuard(
        TopologyWrapper* wrapper = nullptr, const D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr);
    ~TopologyPathGuard();

    const D3DKMDT_VIDPN_PRESENT_PATH* operator->() const;
    const D3DKMDT_VIDPN_PRESENT_PATH& operator*() const;

    const D3DKMDT_VIDPN_PRESENT_PATH* Get() const;
    const D3DKMDT_VIDPN_PRESENT_PATH* Release();
    void Reset(TopologyWrapper* wrapper = nullptr, const D3DKMDT_VIDPN_PRESENT_PATH* path = nullptr);

private:
    void Clean();

private:
    TopologyPathGuard(const TopologyPathGuard&);
    TopologyPathGuard& operator=(const TopologyPathGuard&);

private:
    TopologyWrapper* m_wrapper;
    const D3DKMDT_VIDPN_PRESENT_PATH* m_path;
};