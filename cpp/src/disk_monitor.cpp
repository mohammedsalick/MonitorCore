#include "disk_monitor.h"
#include <psapi.h>

DiskMonitor::DiskMonitor() : initialized(false) {}

bool DiskMonitor::initialize() {
    initialized = true;
    return true;
}

void DiskMonitor::update() {
    if (!initialized) return;

    disks.clear();

    DWORD drives = GetLogicalDrives();
    char driveLetter = 'A';

    for (int i = 0; i < 26; ++i) {
        if (drives & (1 << i)) {
            DiskInfo disk;
            disk.mountPoint = std::string(1, driveLetter) + ":\\";
            
            ULARGE_INTEGER freeBytes, totalBytes;
            if (GetDiskFreeSpaceExA(disk.mountPoint.c_str(), &freeBytes, &totalBytes, nullptr)) {
                // Convert bytes to GB
                disk.total = totalBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                disk.free = freeBytes.QuadPart / (1024.0 * 1024.0 * 1024.0);
                disk.used = disk.total - disk.free;
                disk.name = "Drive " + std::string(1, driveLetter);
                disk.readSpeed = 0.0; // Would require performance counters
                disk.writeSpeed = 0.0; // Would require performance counters
                
                disks.push_back(disk);
            }
        }
        driveLetter++;
    }
}

std::vector<DiskInfo> DiskMonitor::getInfo() const {
    return disks;
}
