#pragma once
#include "MonitorModeSet.h"

class MonitorSet : public MonitorModeSet
{
public:
    MonitorSet(
        const DXGK_MONITORSOURCEMODESET_INTERFACE* minterface, D3DKMDT_HMONITORSOURCEMODESET set);
};