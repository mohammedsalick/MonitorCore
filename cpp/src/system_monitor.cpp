#include "../include/system_monitor.h"
#include "cpu_monitor.h"
#include "gpu_monitor.h"
#include "memory_monitor.h"
#include "disk_monitor.h"
#include "network_monitor.h"
#include "process_monitor.h"
#include <sstream>
#include <iomanip>

// Simple JSON string escaper for safe output
static std::string escapeJson(const std::string& input) {
    std::string out;
    out.reserve(input.size() + 8);
    for (char c : input) {
        switch (c) {
        case '\"': out += "\\\""; break;
        case '\\': out += "\\\\"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            // Only escape control chars; leave UTF-8 bytes as-is
            if (static_cast<unsigned char>(c) < 0x20) {
                // Skip other control characters
                continue;
            }
            out += c;
        }
    }
    return out;
}

class SystemMonitor::Impl {
public:
    CPUMonitor cpuMonitor;
    GPUMonitor gpuMonitor;
    MemoryMonitor memoryMonitor;
    DiskMonitor diskMonitor;
    NetworkMonitor networkMonitor;
    ProcessMonitor processMonitor;

    bool initialized = false;
};

SystemMonitor::SystemMonitor() : pImpl(std::make_unique<Impl>()) {}

SystemMonitor::~SystemMonitor() = default;

bool SystemMonitor::initialize() {
    if (!pImpl->cpuMonitor.initialize()) return false;
    if (!pImpl->gpuMonitor.initialize()) return false;
    if (!pImpl->memoryMonitor.initialize()) return false;
    if (!pImpl->diskMonitor.initialize()) return false;
    if (!pImpl->networkMonitor.initialize()) return false;
    if (!pImpl->processMonitor.initialize()) return false;

    pImpl->initialized = true;
    return true;
}

void SystemMonitor::update() {
    if (!pImpl->initialized) return;

    pImpl->cpuMonitor.update();
    pImpl->gpuMonitor.update();
    pImpl->memoryMonitor.update();
    pImpl->diskMonitor.update();
    pImpl->networkMonitor.update();
    pImpl->processMonitor.update();
}

CPUInfo SystemMonitor::getCPUInfo() const {
    return pImpl->cpuMonitor.getInfo();
}

GPUInfo SystemMonitor::getGPUInfo() const {
    return pImpl->gpuMonitor.getInfo();
}

MemoryInfo SystemMonitor::getMemoryInfo() const {
    return pImpl->memoryMonitor.getInfo();
}

std::vector<DiskInfo> SystemMonitor::getDiskInfo() const {
    return pImpl->diskMonitor.getInfo();
}

NetworkInfo SystemMonitor::getNetworkInfo() const {
    return pImpl->networkMonitor.getInfo();
}

std::vector<ProcessInfo> SystemMonitor::getTopProcesses(int count) const {
    return pImpl->processMonitor.getTopProcesses(count);
}

std::string SystemMonitor::toJSON() const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);

    json << "{\n";
    
    // CPU
    auto cpu = getCPUInfo();
    json << "  \"cpu\": {\n";
    json << "    \"usage\": " << cpu.totalUsage << ",\n";
    json << "    \"cores\": " << cpu.coreCount << ",\n";
    json << "    \"frequency\": " << cpu.frequency << ",\n";
    json << "    \"coreUsage\": [";
    for (size_t i = 0; i < cpu.coreUsage.size(); ++i) {
        if (i > 0) json << ", ";
        json << cpu.coreUsage[i];
    }
    json << "]\n";
    json << "  },\n";

    // GPU
    auto gpu = getGPUInfo();
    json << "  \"gpu\": {\n";
    json << "    \"name\": \"" << escapeJson(gpu.name) << "\",\n";
    json << "    \"usage\": " << gpu.usage << ",\n";
    json << "    \"memoryUsed\": " << gpu.memoryUsed << ",\n";
    json << "    \"memoryTotal\": " << gpu.memoryTotal << ",\n";
    json << "    \"temperature\": " << gpu.temperature << "\n";
    json << "  },\n";

    // Memory
    auto mem = getMemoryInfo();
    json << "  \"memory\": {\n";
    json << "    \"total\": " << mem.total << ",\n";
    json << "    \"used\": " << mem.used << ",\n";
    json << "    \"free\": " << mem.free << ",\n";
    json << "    \"usagePercent\": " << mem.usagePercent << "\n";
    json << "  },\n";

    // Disk
    auto disks = getDiskInfo();
    json << "  \"disks\": [\n";
    for (size_t i = 0; i < disks.size(); ++i) {
        json << "    {\n";
        json << "      \"name\": \"" << escapeJson(disks[i].name) << "\",\n";
        json << "      \"mountPoint\": \"" << escapeJson(disks[i].mountPoint) << "\",\n";
        json << "      \"total\": " << disks[i].total << ",\n";
        json << "      \"used\": " << disks[i].used << ",\n";
        json << "      \"free\": " << disks[i].free << ",\n";
        json << "      \"readSpeed\": " << disks[i].readSpeed << ",\n";
        json << "      \"writeSpeed\": " << disks[i].writeSpeed << "\n";
        json << "    }";
        if (i < disks.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ],\n";

    // Network
    auto net = getNetworkInfo();
    json << "  \"network\": {\n";
    json << "    \"downloadSpeed\": " << net.downloadSpeed << ",\n";
    json << "    \"uploadSpeed\": " << net.uploadSpeed << ",\n";
    json << "    \"activeConnections\": " << net.activeConnections << "\n";
    json << "  },\n";

    // Processes
    auto processes = getTopProcesses(10);
    json << "  \"processes\": [\n";
    for (size_t i = 0; i < processes.size(); ++i) {
        json << "    {\n";
        json << "      \"name\": \"" << escapeJson(processes[i].name) << "\",\n";
        json << "      \"pid\": " << processes[i].pid << ",\n";
        json << "      \"cpuUsage\": " << processes[i].cpuUsage << ",\n";
        json << "      \"memoryUsage\": " << processes[i].memoryUsage << "\n";
        json << "    }";
        if (i < processes.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ]\n";

    json << "}\n";
    return json.str();
}
