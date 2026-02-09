#pragma once

#include "../include/system_monitor.h"
#include <windows.h>
#include <pdh.h>

class CPUMonitor {
public:
    CPUMonitor();
    ~CPUMonitor();

    bool initialize();
    void update();
    CPUInfo getInfo() const;

private:
    PDH_HQUERY query;
    PDH_HCOUNTER totalCounter;
    std::vector<PDH_HCOUNTER> coreCounters;
    CPUInfo info;
    bool initialized;
};
