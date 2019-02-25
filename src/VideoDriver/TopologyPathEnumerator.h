#pragma once
#include "TopologyPathGuard.h"

class TopologyWrapper;

class TopologyPathEnumerator
{
public:
    TopologyPathEnumerator(TopologyWrapper* wrapper);

    bool End() const;
    void Next();

    TopologyPathGuard& GetPath();

    NTSTATUS GetStatus() const;

private:
    void Acquire(const D3DKMDT_VIDPN_PRESENT_PATH* path);

private:
    TopologyWrapper* m_wrapper;
    TopologyPathGuard m_path;
    NTSTATUS m_status;
};