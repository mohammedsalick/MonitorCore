# Build Instructions

## Prerequisites

- Windows 10/11
- C++ Compiler (MSVC or MinGW)
- Python 3.10+
- CMake 3.15+ (optional)

## Building C++ Monitor

### Option 1: Using CMake (Recommended)

```bash
cd cpp
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be at `cpp/build/monitor.exe` (or `Debug/monitor.exe` / `Release/monitor.exe`)

### Option 2: Manual Compilation (MinGW)

```bash
cd cpp
g++ -std=c++17 -o monitor.exe src/*.cpp -Iinclude -lpdh -lpsapi -lwbemuuid -lcomsuppw -liphlpapi
```

### Option 3: Manual Compilation (MSVC)

```bash
cd cpp
cl /EHsc /std:c++17 /Iinclude src\*.cpp /link pdh.lib psapi.lib wbemuuid.lib comsuppw.lib iphlpapi.lib /OUT:monitor.exe
```

## Setting Up Python Environment

```bash
cd python
pip install -r requirements.txt
```

## Running the System

### Step 1: Start C++ Monitor (Terminal 1)

```bash
cd cpp/build
monitor.exe
```

This will output JSON data to stdout every second.

### Step 2: Start Flask Server (Terminal 2)

```bash
cd python
python api/server.py
```

The server will start on `http://localhost:5000`

### Step 3: Open Dashboard

Open your browser and navigate to `http://localhost:5000`

### Step 4: Use CLI (Optional, Terminal 3)

```bash
cd python
python cli/monitor_cli.py
```

## Troubleshooting

### Monitor executable not found

Make sure you've built the C++ monitor first. The Python server looks for the executable in:
- `../../cpp/build/monitor.exe`
- `../../cpp/build/Debug/monitor.exe`
- `../../cpp/build/Release/monitor.exe`

You can also modify `python/api/server.py` to point to the correct path.

### Permission errors

Some system monitoring requires administrator privileges. Try running as administrator if you encounter permission issues.

### GPU information not showing

GPU usage and temperature require specific Windows Performance Counters or GPU SDKs. The current implementation shows GPU name and basic info via WMI. For full GPU monitoring, you would need to integrate AMD ADL or NVIDIA NVML SDKs.
