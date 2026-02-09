#pragma once

#include "../include/system_monitor.h"
#include <windows.h>

class MemoryMonitor {
public:
    MemoryMonitor();
    bool initialize();
    void update();
    MemoryInfo getInfo() const;

private:
    MemoryInfo info;
    bool initialized;
};
