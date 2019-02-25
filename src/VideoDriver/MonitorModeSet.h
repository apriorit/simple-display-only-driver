#pragma once
#include "BaseModeSet.h"

namespace detail
{
    struct MonitorSetDetail
    {
        typedef DXGK_MONITORSOURCEMODESET_INTERFACE    Interface;
        typedef D3DKMDT_HMONITORSOURCEMODESET          Set;
        typedef D3DKMDT_MONITOR_SOURCE_MODE            Mode;

        static const char name[];
    };
}

class MonitorModeSet : public BaseModeSet<detail::MonitorSetDetail>
{
public:
    NTSTATUS GetPreferredMode(CMode& mode);

private:
    NTSTATUS AcquirePreferredModeInfo(const D3DKMDT_MONITOR_SOURCE_MODE** mode);
};