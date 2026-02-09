#include "cpu_monitor.h"
#include <pdhmsg.h>
#include <psapi.h>

CPUMonitor::CPUMonitor() : query(nullptr), initialized(false) {}

CPUMonitor::~CPUMonitor() {
    if (query) {
        PdhCloseQuery(query);
    }
}

bool CPUMonitor::initialize() {
    if (PdhOpenQuery(nullptr, 0, &query) != ERROR_SUCCESS) {
        return false;
    }

    // Total CPU usage (using ANSI version)
    if (PdhAddCounterA(query, "\\Processor(_Total)\\% Processor Time", 0, &totalCounter) != ERROR_SUCCESS) {
        PdhCloseQuery(query);
        return false;
    }

    // Get core count
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    info.coreCount = sysInfo.dwNumberOfProcessors;
    info.coreUsage.resize(info.coreCount);

    // Per-core counters
    for (int i = 0; i < info.coreCount; ++i) {
        PDH_HCOUNTER counter;
        char path[256];
        sprintf_s(path, "\\Processor(%d)\\%% Processor Time", i);
        if (PdhAddCounterA(query, path, 0, &counter) == ERROR_SUCCESS) {
            coreCounters.push_back(counter);
        }
    }

    // Initial collection
    PdhCollectQueryData(query);

    initialized = true;
    return true;
}

void CPUMonitor::update() {
    if (!initialized) return;

    PdhCollectQueryData(query);

    // Get total CPU usage
    PDH_FMT_COUNTERVALUE value;
    if (PdhGetFormattedCounterValue(totalCounter, PDH_FMT_DOUBLE, nullptr, &value) == ERROR_SUCCESS) {
        info.totalUsage = value.doubleValue;
    }

    // Get per-core usage
    for (size_t i = 0; i < coreCounters.size() && i < info.coreUsage.size(); ++i) {
        if (PdhGetFormattedCounterValue(coreCounters[i], PDH_FMT_DOUBLE, nullptr, &value) == ERROR_SUCCESS) {
            info.coreUsage[i] = value.doubleValue;
        }
    }

    // Get CPU frequency (simplified - actual frequency requires WMI)
    info.frequency = 0.0; // Will be set via WMI if needed
}

CPUInfo CPUMonitor::getInfo() const {
    return info;
}
