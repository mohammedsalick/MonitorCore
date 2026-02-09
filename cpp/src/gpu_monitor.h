#pragma once

#include "../include/system_monitor.h"
#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include <string>

class GPUMonitor {
public:
    GPUMonitor();
    ~GPUMonitor();

    bool initialize();
    void update();
    GPUInfo getInfo() const;

private:
    IWbemLocator* pLocator;
    IWbemServices* pServices;
    GPUInfo info;
    bool initialized;

    bool queryWMI(const std::wstring& query, std::wstring& result);
};
