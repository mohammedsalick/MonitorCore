#pragma once

#include "../include/system_monitor.h"
#include <windows.h>
#include <vector>

class DiskMonitor {
public:
    DiskMonitor();
    bool initialize();
    void update();
    std::vector<DiskInfo> getInfo() const;

private:
    std::vector<DiskInfo> disks;
    bool initialized;
};
