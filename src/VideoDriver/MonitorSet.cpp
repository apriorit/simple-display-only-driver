#include "stdafx.h"
#include "MonitorSet.h"

MonitorSet::MonitorSet(
    const DXGK_MONITORSOURCEMODESET_INTERFACE* minterface, D3DKMDT_HMONITORSOURCEMODESET set)
{
    SetParams(minterface, set);
}
