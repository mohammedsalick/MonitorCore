cmake_minimum_required(VERSION 3.15)
project(SystemMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/system_monitor.cpp
    src/cpu_monitor.cpp
    src/gpu_monitor.cpp
    src/memory_monitor.cpp
    src/disk_monitor.cpp
    src/network_monitor.cpp
    src/process_monitor.cpp
)

# Include directories
include_directories(include)

# Windows-specific definitions
if(WIN32)
    # Set Windows SDK version if not specified
    if(NOT CMAKE_SYSTEM_VERSION)
        set(CMAKE_SYSTEM_VERSION 10.0)
    endif()
    
    # Windows-specific compiler definitions
    add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10+
    add_definitions(-DNTDDI_VERSION=0x0A000000)  # Windows 10 for netioapi.h
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    
    # For MinGW, ensure we link against required libraries
    if(MINGW)
        add_definitions(-D__USE_MINGW_ANSI_STDIO=0)
    endif()
endif()

# Create executable
add_executable(monitor ${SOURCES})

# Link libraries
target_link_libraries(monitor
    pdh
    psapi
    wbemuuid
    comsuppw
    iphlpapi
    ws2_32
)
