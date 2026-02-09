#pragma once

#include "../include/system_monitor.h"
// winsock2.h must come before windows.h
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <iphlpapi.h>
#include <netioapi.h>

class NetworkMonitor {
public:
    NetworkMonitor();
    ~NetworkMonitor();
    bool initialize();
    void update();
    NetworkInfo getInfo() const;

private:
    NetworkInfo info;
    bool initialized;
    ULONG64 lastBytesReceived;
    ULONG64 lastBytesSent;
    DWORD lastUpdateTime;
};
