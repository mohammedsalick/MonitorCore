#pragma once

#include "../include/system_monitor.h"
#include <windows.h>
#include <psapi.h>
#include <vector>
#include <map>

class ProcessMonitor {
public:
    ProcessMonitor();
    bool initialize();
    void update();
    std::vector<ProcessInfo> getTopProcesses(int count) const;

private:
    std::vector<ProcessInfo> processes;
    bool initialized;
    std::map<DWORD, ULONG64> lastProcessTimes;
    DWORD lastUpdateTime;
};
