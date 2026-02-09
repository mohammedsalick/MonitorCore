#include "memory_monitor.h"
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

MemoryMonitor::MemoryMonitor() : initialized(false) {}

bool MemoryMonitor::initialize() {
    initialized = true;
    return true;
}

void MemoryMonitor::update() {
    if (!initialized) return;

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    // Convert bytes to MB
    info.total = memStatus.ullTotalPhys / (1024.0 * 1024.0);
    info.free = memStatus.ullAvailPhys / (1024.0 * 1024.0);
    info.used = info.total - info.free;
    info.usagePercent = (info.used / info.total) * 100.0;
}

MemoryInfo MemoryMonitor::getInfo() const {
    return info;
}
