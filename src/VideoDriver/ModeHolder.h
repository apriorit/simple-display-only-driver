#pragma once
#include "Mode.h"

class ModeValidator;
class SourceModeSetCreator;
class TargetModeSetCreator;
class SourceModeSet;
class TargetModeSet;
class MonitorModeSet;
class ISourceMode;

class ModeHolder
{
public:
    explicit ModeHolder(ISourceMode* sourceMode);

    NTSTATUS CreateNewSet(SourceModeSetCreator& sources) const;
    NTSTATUS CreateNewSet(TargetModeSetCreator& targets) const;

    NTSTATUS AddModes(SourceModeSet& sources) const;
    NTSTATUS AddModes(TargetModeSet& targets) const;
    NTSTATUS AddModes(MonitorModeSet& monitors) const;

private:
    template <class T>
    NTSTATUS CreateAndAddModes(T& set) const;

    template <class T>
    NTSTATUS AddMode(T& set) const;

private:
    ISourceMode* m_sourceMode;
};
