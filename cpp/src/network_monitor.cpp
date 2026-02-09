#include "network_monitor.h"
#include <psapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

NetworkMonitor::NetworkMonitor() : initialized(false), lastBytesReceived(0), lastBytesSent(0), lastUpdateTime(0) {}

NetworkMonitor::~NetworkMonitor() = default;

bool NetworkMonitor::initialize() {
    lastUpdateTime = GetTickCount();
    initialized = true;
    return true;
}

void NetworkMonitor::update() {
    if (!initialized) return;

    MIB_IF_ROW2 ifRow;
    ZeroMemory(&ifRow, sizeof(ifRow));
    
    // Get primary network interface (simplified - would iterate all interfaces in production)
    PMIB_IF_TABLE2 pIfTable;
    if (GetIfTable2(&pIfTable) == NO_ERROR) {
        ULONG64 totalReceived = 0;
        ULONG64 totalSent = 0;
        
        for (ULONG i = 0; i < pIfTable->NumEntries; ++i) {
            // Skip loopback and non-active interfaces
            if (pIfTable->Table[i].Type == IF_TYPE_ETHERNET_CSMACD || 
                pIfTable->Table[i].Type == IF_TYPE_IEEE80211) {
                if (pIfTable->Table[i].OperStatus == IfOperStatusUp) {
                    totalReceived += pIfTable->Table[i].InOctets;
                    totalSent += pIfTable->Table[i].OutOctets;
                }
            }
        }

        DWORD currentTime = GetTickCount();
        DWORD timeDelta = currentTime - lastUpdateTime;
        
        if (timeDelta > 0 && lastUpdateTime > 0) {
            // Calculate speed in MB/s
            double timeDeltaSeconds = timeDelta / 1000.0;
            info.downloadSpeed = ((totalReceived - lastBytesReceived) / (1024.0 * 1024.0)) / timeDeltaSeconds;
            info.uploadSpeed = ((totalSent - lastBytesSent) / (1024.0 * 1024.0)) / timeDeltaSeconds;
        }

        lastBytesReceived = totalReceived;
        lastBytesSent = totalSent;
        lastUpdateTime = currentTime;

        FreeMibTable(pIfTable);
    }

    // Get active connections count (simplified)
    MIB_TCPTABLE_OWNER_PID* pTcpTable = nullptr;
    DWORD dwSize = 0;
    if (GetExtendedTcpTable(nullptr, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
        pTcpTable = (MIB_TCPTABLE_OWNER_PID*)malloc(dwSize);
        if (GetExtendedTcpTable(pTcpTable, &dwSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == NO_ERROR) {
            info.activeConnections = pTcpTable->dwNumEntries;
        }
        if (pTcpTable) free(pTcpTable);
    }
}

NetworkInfo NetworkMonitor::getInfo() const {
    return info;
}
