# MonitorCore

> A platform to monitor your System Metrics of CPU, GPU, Memory, Disk, Network, and Processes in real-time.

MonitorCore is a comprehensive system monitoring solution built with a high-performance C++ backend and a modern web dashboard. It provides real-time insights into your system's performance metrics with minimal overhead.

---

## ✨ Features

### 📊 Real-Time Monitoring
- **CPU Monitoring**: Per-core usage, frequency, and detailed process analysis
- **GPU Monitoring**: Usage, memory (VRAM), temperature, and device information
- **Memory Monitoring**: Total/used/free RAM, usage percentage, and per-process breakdown
- **Disk Monitoring**: Space usage, I/O rates, and health metrics across all drives
- **Network Monitoring**: Download/upload speeds, active connections, and bandwidth tracking
- **Process Monitoring**: Top processes by CPU and memory usage with detailed metrics

### 🎨 Modern Interface
- **Web Dashboard**: Beautiful black & white theme with red accents and gradient charts
- **CLI Interface**: Rich terminal output with live-updating tables and color-coded metrics
- **Real-Time Updates**: WebSocket-based live data streaming (1-second refresh rate)
- **Responsive Design**: Works seamlessly on desktop and mobile devices

### ⚡ Performance
- **Low Overhead**: <1% CPU usage during monitoring
- **High Performance**: C++ core for efficient system API calls
- **Scalable Architecture**: Modular design for easy extension

---

## 🏗️ Architecture

MonitorCore follows a modular architecture with clear separation of concerns:

```
┌─────────────────┐
│   Web Dashboard │  ← Vanilla JavaScript + Chart.js
│   (Port 5000)   │
└────────┬────────┘
         │ WebSocket
         ▼
┌─────────────────┐
│  Python API     │  ← Flask + Flask-SocketIO
│  (Flask Server) │
└────────┬────────┘
         │ JSON (stdout)
         ▼
┌─────────────────┐
│  C++ Monitor    │  ← Windows APIs (PDH, WMI, etc.)
│  (Core Engine) │
└─────────────────┘
```

### Components

1. **C++ Core** (`cpp/`)
   - Low-level system monitoring using Windows APIs
   - Performance Counters (PDH) for CPU metrics
   - WMI for GPU and system information
   - Direct memory and disk access
   - Network interface monitoring

2. **Python Service Layer** (`python/`)
   - Flask REST API server
   - WebSocket server for real-time updates
   - CLI interface with rich formatting
   - Data aggregation and processing

3. **Web Dashboard** (`web/`)
   - Vanilla JavaScript (no frameworks)
   - Real-time charts with Chart.js
   - Modern black & white design with red accents
   - Responsive CSS Grid layout

---

## 🚀 Quick Start

### Prerequisites

- **Windows 10/11**
- **C++ Compiler**: MSVC 2019+ or MinGW-w64
- **Python 3.10+**
- **CMake 3.15+** (optional, for automated builds)

### Installation

#### 1. Clone the Repository

```bash
git clone https://github.com/mohammedsalick/MonitorCore.git
cd MonitorCore
```

#### 2. Build C++ Monitor

**Option A: Using CMake (Recommended)**

```bash
cd cpp
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be located at:
- `cpp/build/Debug/monitor.exe` (Debug build)
- `cpp/build/Release/monitor.exe` (Release build)

**Option B: Using Build Script**

```bash
cd cpp
.\build.bat
```

**Option C: Manual Compilation (MinGW)**

```bash
cd cpp
g++ -std=c++17 -o monitor.exe src/*.cpp -Iinclude -lpdh -lpsapi -lwbemuuid -lcomsuppw -liphlpapi -lws2_32
```

#### 3. Install Python Dependencies

```bash
cd python
pip install -r requirements.txt
```

---

## 📖 Usage

### Running MonitorCore

#### Step 1: Start the C++ Monitor

```bash
cd cpp\build\Debug
monitor.exe
```

The monitor will output JSON data to stdout every second.

#### Step 2: Start the Flask Server

```bash
cd python
python api\server.py
```

You should see:
```
BASE_DIR: C:\path\to\MonitorCore
WEB_DIR: C:\path\to\MonitorCore\web
Starting monitor from: C:\path\to\MonitorCore\cpp\build\Debug\monitor.exe
✅ Sent update #1 to clients
Starting Flask server on http://localhost:5000
```

#### Step 3: Access the Web Dashboard

Open your browser and navigate to:
```
http://localhost:5000
```

#### Step 4: Use CLI (Optional)

In a separate terminal:

```bash
cd python
python cli\monitor_cli.py
```

---

## 🛠️ Tech Stack

### Backend
- **C++17**: Core monitoring engine
- **Windows APIs**: 
  - PDH (Performance Data Helper) for CPU metrics
  - WMI (Windows Management Instrumentation) for system info
  - PSAPI for process and memory information
  - IP Helper API for network statistics

### Middleware
- **Python 3.10+**: Service layer
- **Flask 3.0**: REST API framework
- **Flask-SocketIO**: WebSocket server for real-time updates
- **Rich**: Beautiful terminal formatting for CLI

### Frontend
- **Vanilla JavaScript**: No framework dependencies
- **Chart.js**: Real-time data visualization
- **Socket.IO Client**: WebSocket communication
- **Modern CSS**: Grid layout with custom properties

---

## 📁 Project Structure

```
MonitorCore/
├── cpp/                      # C++ Monitoring Core
│   ├── include/              # Header files
│   │   └── system_monitor.h
│   ├── src/                  # Source files
│   │   ├── main.cpp         # Entry point
│   │   ├── system_monitor.cpp
│   │   ├── cpu_monitor.cpp/h
│   │   ├── gpu_monitor.cpp/h
│   │   ├── memory_monitor.cpp/h
│   │   ├── disk_monitor.cpp/h
│   │   ├── network_monitor.cpp/h
│   │   └── process_monitor.cpp/h
│   ├── CMakeLists.txt       # Build configuration
│   └── build.bat            # Windows build script
│
├── python/                   # Python Service Layer
│   ├── api/
│   │   └── server.py        # Flask server + WebSocket
│   ├── cli/
│   │   └── monitor_cli.py   # CLI interface
│   ├── requirements.txt     # Python dependencies
│   └── run.bat              # Quick start script
│
├── web/                      # Web Dashboard
│   ├── index.html           # Main HTML
│   ├── css/
│   │   └── styles.css       # Styling
│   └── js/
│       └── app.js           # Frontend logic
│
├── README.md                 # This file
├── BUILD.md                  # Detailed build instructions
├── QUICKSTART.md             # Quick start guide
└── .gitignore                # Git ignore rules
```

---

## 🎯 Key Features Explained

### CPU Monitoring
- **Per-Core Usage**: Individual core utilization percentages
- **Total CPU Usage**: Aggregate CPU usage across all cores
- **Frequency**: CPU clock speed (when available)
- **Process Analysis**: Top processes sorted by CPU usage

### GPU Monitoring
- **Device Information**: GPU name and model via WMI
- **Memory Usage**: VRAM utilization (requires GPU SDK for full support)
- **Temperature**: GPU temperature monitoring (requires GPU SDK)
- **Usage Percentage**: GPU utilization (requires Performance Counters or SDK)

> **Note**: Full GPU monitoring (usage, temperature) requires GPU-specific SDKs:
> - **AMD**: ADL SDK
> - **NVIDIA**: NVML SDK
> 
> The current implementation uses Windows APIs for basic GPU information. The architecture supports easy SDK integration.

### Memory Monitoring
- **Total/Used/Free**: Real-time RAM statistics
- **Usage Percentage**: Memory utilization percentage
- **Per-Process Memory**: Memory usage breakdown by process

### Disk Monitoring
- **Space Usage**: Total, used, and free space per drive
- **I/O Rates**: Read/write speeds (basic implementation)
- **Multi-Drive Support**: Monitors all available drives

### Network Monitoring
- **Bandwidth**: Download and upload speeds in MB/s
- **Active Connections**: Number of active TCP connections
- **Real-Time Tracking**: Continuous network activity monitoring

---

## 🔧 Configuration

### Customizing Update Frequency

The default update interval is 1 second. To change it, modify `cpp/src/main.cpp`:

```cpp
std::this_thread::sleep_for(std::chrono::seconds(1)); // Change 1 to desired seconds
```

### Changing Web Server Port

Edit `python/api/server.py`:

```python
socketio.run(app, host='0.0.0.0', port=5000, debug=False)  # Change port number
```

### Adjusting Chart History

Modify `web/js/app.js`:

```javascript
const maxDataPoints = 30; // Change to desired number of data points
```

---

## 🐛 Troubleshooting

### Monitor Executable Not Found

**Problem**: Python server can't find the C++ monitor executable.

**Solution**: 
1. Ensure you've built the C++ monitor first
2. Check that the executable exists in one of these locations:
   - `cpp/build/monitor.exe`
   - `cpp/build/Debug/monitor.exe`
   - `cpp/build/Release/monitor.exe`
3. The server will print the paths it's checking on startup

### Permission Errors

**Problem**: Some metrics show as 0 or fail to update.

**Solution**: 
- Run the C++ monitor and Python server as Administrator
- Some system APIs require elevated privileges

### GPU Information Shows as 0%

**Problem**: GPU usage and temperature display as 0.

**Solution**: 
- This is expected with the current Windows API implementation
- GPU name should still display correctly
- For full GPU monitoring, integrate AMD ADL or NVIDIA NVML SDKs
- See `docs/EXTENDING.md` (if available) for SDK integration guide

### Port Already in Use

**Problem**: Flask server fails to start with "port already in use" error.

**Solution**: 
- Change the port in `python/api/server.py`
- Or close the application using port 5000

### JSON Parse Errors

**Problem**: Console shows JSON decode errors.

**Solution**: 
- Ensure the C++ monitor is outputting valid JSON
- Check that no other process is interfering with stdout
- Restart both the monitor and server

---

## 🚧 Future Enhancements

- [ ] Historical data storage (SQLite/InfluxDB)
- [ ] Alert system with configurable thresholds
- [ ] Export data to CSV/JSON
- [ ] AMD ADL SDK integration for full GPU monitoring
- [ ] NVIDIA NVML SDK integration
- [ ] Linux support
- [ ] Docker containerization
- [ ] REST API endpoints for external integration
- [ ] Customizable dashboard widgets
- [ ] Dark/Light theme toggle

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 👤 Author

**Mohammed Salick**
- GitHub: [@mohammedsalick](https://github.com/mohammedsalick)
- Repository: [MonitorCore](https://github.com/mohammedsalick/MonitorCore)

---

## 🙏 Acknowledgments

- Windows API documentation
- Flask and Flask-SocketIO communities
- Chart.js for excellent charting library
- Rich library for beautiful terminal output

---

## ⭐ Show Your Support

If you find this project useful, please give it a ⭐ on GitHub!

---

**Built with ❤️ for system administrators and developers**
#   M o n i t o r C o r e  
 