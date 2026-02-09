#include "gpu_monitor.h"
#include <comutil.h>
#include <iostream>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")

GPUMonitor::GPUMonitor() : pLocator(nullptr), pServices(nullptr), initialized(false) {
    info.name = "Unknown";
}

GPUMonitor::~GPUMonitor() {
    if (pServices) {
        pServices->Release();
    }
    if (pLocator) {
        pLocator->Release();
    }
    CoUninitialize();
}

bool GPUMonitor::initialize() {
    HRESULT hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres)) return false;

    hres = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_NONE,
                                RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
        CoUninitialize();
        return false;
    }

    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                            IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hres)) {
        CoUninitialize();
        return false;
    }

    hres = pLocator->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, 0, 0, nullptr, 0, &pServices);
    if (FAILED(hres)) {
        pLocator->Release();
        CoUninitialize();
        return false;
    }

    hres = CoSetProxyBlanket(pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                             RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hres)) {
        pServices->Release();
        pLocator->Release();
        CoUninitialize();
        return false;
    }

    // Get GPU name
    std::wstring gpuName;
    if (queryWMI(L"SELECT Name FROM Win32_VideoController", gpuName)) {
        char buffer[256];
        WideCharToMultiByte(CP_UTF8, 0, gpuName.c_str(), -1, buffer, sizeof(buffer), nullptr, nullptr);
        info.name = buffer;
    }

    initialized = true;
    return true;
}

bool GPUMonitor::queryWMI(const std::wstring& query, std::wstring& result) {
    if (!pServices) return false;

    IEnumWbemClassObject* pEnumerator = nullptr;
    HRESULT hres = pServices->ExecQuery(
        bstr_t("WQL"),
        bstr_t(query.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator);

    if (FAILED(hres)) return false;

    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    while (pEnumerator) {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (0 == uReturn) break;

        VARIANT vtProp;
        hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
            result = vtProp.bstrVal;
            VariantClear(&vtProp);
            pclsObj->Release();
            pEnumerator->Release();
            return true;
        }
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    if (pEnumerator) pEnumerator->Release();
    return false;
}

void GPUMonitor::update() {
    if (!initialized) return;

    // GPU usage via Performance Counters (if available)
    // Note: GPU usage requires specific counters that may not be available on all systems
    // This is a simplified version - full implementation would use DXGI or GPU-specific APIs
    
    // Try to get GPU memory via WMI
    std::wstring memTotal;
    if (queryWMI(L"SELECT AdapterRAM FROM Win32_VideoController WHERE AdapterRAM IS NOT NULL", memTotal)) {
        // Memory is in bytes, convert to MB
        // This is a simplified approach - actual implementation would parse the value
    }

    // GPU usage and temperature would require:
    // - AMD: ADL SDK
    // - NVIDIA: NVML SDK
    // - Or: Windows Performance Counters (if available)
    // For now, we'll set placeholder values
    info.usage = 0.0; // Would need SDK or specific counters
    info.memoryUsed = 0.0;
    info.memoryTotal = 0.0;
    info.temperature = 0.0;
}

GPUInfo GPUMonitor::getInfo() const {
    return info;
}
