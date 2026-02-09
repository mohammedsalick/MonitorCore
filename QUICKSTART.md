# Quick Start Guide

## Step 1: Build C++ Monitor

Open PowerShell or Command Prompt in the project root:

```bash
cd cpp
.\build.bat
```

Or manually:
```bash
cd cpp
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be at `cpp/build/monitor.exe` (or in `Debug/` or `Release/` subfolder).

## Step 2: Install Python Dependencies

```bash
cd python
pip install -r requirements.txt
```

## Step 3: Run the System

### Terminal 1: Start C++ Monitor
```bash
cd cpp\build
monitor.exe
```

### Terminal 2: Start Flask Server
```bash
cd python
python api\server.py
```

### Terminal 3: Open Browser
Navigate to: **http://localhost:5000**

### Terminal 4 (Optional): CLI Interface
```bash
cd python
python cli\monitor_cli.py
```

## Troubleshooting

### "Monitor executable not found"
- Make sure you built the C++ monitor first (Step 1)
- Check that `monitor.exe` exists in `cpp/build/` or subfolders
- The Python server will show the path it's looking for

### "Permission denied" or monitoring not working
- Some features may require administrator privileges
- Try running as administrator

### Port 5000 already in use
- Change the port in `python/api/server.py`:
  ```python
  socketio.run(app, host='0.0.0.0', port=5001, debug=False)
  ```

### GPU info shows "Unknown" or 0%
- GPU usage requires specific Windows Performance Counters
- GPU name should work via WMI
- For full GPU monitoring, you'd need AMD ADL or NVIDIA NVML SDKs

## What You'll See

- **Web Dashboard**: Real-time charts for CPU, GPU, Memory, Network
- **CLI**: Live-updating terminal interface with colored output
- **Data Updates**: Every 1 second

Enjoy monitoring your system! 🚀
