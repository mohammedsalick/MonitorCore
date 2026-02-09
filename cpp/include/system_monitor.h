#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations
struct CPUInfo;
struct GPUInfo;
struct MemoryInfo;
struct DiskInfo;
struct NetworkInfo;
struct ProcessInfo;

class SystemMonitor {
public:
    SystemMonitor();
    ~SystemMonitor();

    bool initialize();
    void update();

    // Getters
    CPUInfo getCPUInfo() const;
    GPUInfo getGPUInfo() const;
    MemoryInfo getMemoryInfo() const;
    std::vector<DiskInfo> getDiskInfo() const;
    NetworkInfo getNetworkInfo() const;
    std::vector<ProcessInfo> getTopProcesses(int count = 10) const;

    // JSON export
    std::string toJSON() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// Data structures
struct CPUInfo {
    double totalUsage = 0.0;
    std::vector<double> coreUsage;
    int coreCount = 0;
    double frequency = 0.0; // MHz
};

struct GPUInfo {
    std::string name;
    double usage = 0.0; // Percentage
    double memoryUsed = 0.0; // MB
    double memoryTotal = 0.0; // MB
    double temperature = 0.0; // Celsius (if available)
};

struct MemoryInfo {
    double total = 0.0; // MB
    double used = 0.0; // MB
    double free = 0.0; // MB
    double usagePercent = 0.0;
};

struct DiskInfo {
    std::string name;
    std::string mountPoint;
    double total = 0.0; // GB
    double used = 0.0; // GB
    double free = 0.0; // GB
    double readSpeed = 0.0; // MB/s
    double writeSpeed = 0.0; // MB/s
};

struct NetworkInfo {
    double downloadSpeed = 0.0; // MB/s
    double uploadSpeed = 0.0; // MB/s
    int activeConnections = 0;
};

struct ProcessInfo {
    std::string name;
    int pid = 0;
    double cpuUsage = 0.0;
    double memoryUsage = 0.0; // MB
};
