import subprocess
import json
import threading
import time
from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
from flask_socketio import SocketIO, emit
import os
import sys

# Get the project root directory (2 levels up from this file)
BASE_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
WEB_DIR = os.path.join(BASE_DIR, 'web')

# Debug: Print paths
print(f"BASE_DIR: {BASE_DIR}")
print(f"WEB_DIR: {WEB_DIR}")
print(f"index.html exists: {os.path.exists(os.path.join(WEB_DIR, 'index.html'))}")

app = Flask(__name__, 
            template_folder=WEB_DIR,
            static_folder=WEB_DIR,
            static_url_path='')
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

# Path to C++ monitor executable
MONITOR_EXE = None

def find_monitor_exe():
    """Find the monitor executable"""
    # Check common build locations relative to BASE_DIR
    possible_paths = [
        os.path.join(BASE_DIR, 'cpp', 'build', 'monitor.exe'),
        os.path.join(BASE_DIR, 'cpp', 'build', 'Debug', 'monitor.exe'),
        os.path.join(BASE_DIR, 'cpp', 'build', 'Release', 'monitor.exe'),
        os.path.join(BASE_DIR, 'cpp', 'build', 'x64', 'Debug', 'monitor.exe'),
        os.path.join(BASE_DIR, 'cpp', 'build', 'x64', 'Release', 'monitor.exe'),
    ]
    
    for path in possible_paths:
        if os.path.exists(path):
            return path
    
    return None

def monitor_worker():
    """Background worker that reads from C++ monitor"""
    global MONITOR_EXE
    
    if not MONITOR_EXE:
        MONITOR_EXE = find_monitor_exe()
    
    if not MONITOR_EXE:
        error_msg = f"ERROR: Monitor executable not found. Please build the C++ monitor first."
        print(error_msg)
        print(f"Looked in: {[os.path.join(BASE_DIR, 'cpp', 'build', 'monitor.exe'), os.path.join(BASE_DIR, 'cpp', 'build', 'Debug', 'monitor.exe'), os.path.join(BASE_DIR, 'cpp', 'build', 'Release', 'monitor.exe')]}")
        socketio.emit('error', {'message': error_msg})
        return
    
    print(f"Starting monitor from: {MONITOR_EXE}")
    
    try:
        process = subprocess.Popen(
            [MONITOR_EXE],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1,
            universal_newlines=True
        )
        
        # Start a thread to read stderr
        def read_stderr():
            for line in process.stderr:
                print(f"Monitor stderr: {line.strip()}")
        
        stderr_thread = threading.Thread(target=read_stderr, daemon=True)
        stderr_thread.start()
        
        line_count = 0
        json_buffer = ""
        brace_count = 0
        
        for line in process.stdout:
            # Count braces in this line
            open_braces = line.count('{')
            close_braces = line.count('}')
            brace_count += open_braces - close_braces
            
            # Add line to buffer
            json_buffer += line
            
            # Only try to parse when braces are balanced (complete JSON)
            # Check that buffer starts with { and ends with } (or }\n)
            if brace_count == 0 and json_buffer.strip():
                stripped = json_buffer.strip()
                if stripped.startswith('{') and stripped.endswith('}'):
                    try:
                        data = json.loads(stripped)
                        # Successfully parsed - emit and reset buffer
                        socketio.emit('system_update', data)
                        line_count += 1
                        if line_count <= 3:
                            print(f"✅ Sent update #{line_count} to clients")
                        json_buffer = ""  # Reset buffer
                    except json.JSONDecodeError as e:
                        # Only log first few errors to avoid spam
                        if line_count < 5:
                            print(f"❌ JSON decode error: {e}")
                            print(f"Buffer length: {len(stripped)}")
                            print(f"Buffer start: {stripped[:100]}")
                            print(f"Buffer end: {stripped[-100:]}")
                        json_buffer = ""  # Reset on error
                    except Exception as e:
                        print(f"❌ Error processing data: {e}")
                        import traceback
                        traceback.print_exc()
                        json_buffer = ""  # Reset on error
        
        print("Monitor process stdout ended")
                
    except Exception as e:
        print(f"Error starting monitor: {e}")
        import traceback
        traceback.print_exc()
        socketio.emit('error', {'message': f'Failed to start monitor: {str(e)}'})

@app.route('/')
def index():
    """Serve the dashboard"""
    return send_from_directory(WEB_DIR, 'index.html')

@app.route('/css/<path:filename>')
def serve_css(filename):
    """Serve CSS files"""
    return send_from_directory(os.path.join(WEB_DIR, 'css'), filename)

@app.route('/js/<path:filename>')
def serve_js(filename):
    """Serve JavaScript files"""
    return send_from_directory(os.path.join(WEB_DIR, 'js'), filename)

@app.route('/api/status')
def status():
    """API endpoint for status check"""
    return jsonify({
        'status': 'running',
        'monitor_exe': MONITOR_EXE if MONITOR_EXE else 'not found'
    })

if __name__ == '__main__':
    # Start monitor worker in background thread
    monitor_thread = threading.Thread(target=monitor_worker, daemon=True)
    monitor_thread.start()
    
    # Give it a moment to start
    time.sleep(0.5)
    
    print("=" * 50)
    print("MonitorCore - System Monitoring Platform")
    print("Design by Salick")
    print("=" * 50)
    print("Starting Flask server on http://localhost:5000")
    print("Open your browser to view the dashboard")
    
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)
