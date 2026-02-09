#include "process_monitor.h"
#include <tlhelp32.h>
#include <algorithm>

ProcessMonitor::ProcessMonitor() : initialized(false), lastUpdateTime(0) {}

bool ProcessMonitor::initialize() {
    lastUpdateTime = GetTickCount();
    initialized = true;
    return true;
}

void ProcessMonitor::update() {
    if (!initialized) return;

    processes.clear();

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            ProcessInfo proc;
            proc.pid = pe32.th32ProcessID;
            
            // Get process name
            char processName[MAX_PATH];
            strncpy_s(processName, pe32.szExeFile, MAX_PATH);
            proc.name = processName;

            // Get CPU and memory usage
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc.pid);
            if (hProcess) {
                // Memory usage
                PROCESS_MEMORY_COUNTERS_EX pmc;
                if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                    proc.memoryUsage = pmc.WorkingSetSize / (1024.0 * 1024.0); // MB
                }

                // CPU usage (simplified - would need more accurate timing)
                FILETIME creationTime, exitTime, kernelTime, userTime;
                if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
                    ULARGE_INTEGER kernel, user;
                    kernel.LowPart = kernelTime.dwLowDateTime;
                    kernel.HighPart = kernelTime.dwHighDateTime;
                    user.LowPart = userTime.dwLowDateTime;
                    user.HighPart = userTime.dwHighDateTime;

                    ULONG64 totalTime = kernel.QuadPart + user.QuadPart;
                    
                    if (lastProcessTimes.find(proc.pid) != lastProcessTimes.end()) {
                        DWORD timeDelta = GetTickCount() - lastUpdateTime;
                        if (timeDelta > 0) {
                            ULONG64 timeDelta100ns = (timeDelta * 10000ULL);
                            proc.cpuUsage = ((double)(totalTime - lastProcessTimes[proc.pid]) / timeDelta100ns) * 100.0;
                        }
                    }
                    lastProcessTimes[proc.pid] = totalTime;
                }

                CloseHandle(hProcess);
            }

            processes.push_back(proc);
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);
    lastUpdateTime = GetTickCount();

    // Sort by CPU usage
    std::sort(processes.begin(), processes.end(), 
              [](const ProcessInfo& a, const ProcessInfo& b) {
                  return a.cpuUsage > b.cpuUsage;
              });
}

std::vector<ProcessInfo> ProcessMonitor::getTopProcesses(int count) const {
    std::vector<ProcessInfo> result;
    int end = (count < processes.size()) ? count : processes.size();
    for (int i = 0; i < end; ++i) {
        result.push_back(processes[i]);
    }
    return result;
}
