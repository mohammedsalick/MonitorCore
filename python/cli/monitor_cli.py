import subprocess
import json
import sys
import os
from rich.console import Console
from rich.table import Table
from rich.live import Live
from rich.layout import Layout
from rich.panel import Panel
from rich.text import Text
import time

console = Console()

def find_monitor_exe():
    """Find the monitor executable"""
    possible_paths = [
        '../../cpp/build/monitor.exe',
        '../../cpp/build/Debug/monitor.exe',
        '../../cpp/build/Release/monitor.exe',
        './monitor.exe',
        '../cpp/build/monitor.exe'
    ]
    
    for path in possible_paths:
        abs_path = os.path.abspath(path)
        if os.path.exists(abs_path):
            return abs_path
    
    return None

def format_bytes(bytes_val):
    """Format bytes to human readable"""
    for unit in ['B', 'KB', 'MB', 'GB', 'TB']:
        if bytes_val < 1024.0:
            return f"{bytes_val:.2f} {unit}"
        bytes_val /= 1024.0
    return f"{bytes_val:.2f} PB"

def create_layout(data):
    """Create the layout with system information"""
    layout = Layout()
    
    # Top section - CPU and GPU
    top_section = Layout()
    top_section.split_row(
        create_cpu_panel(data.get('cpu', {})),
        create_gpu_panel(data.get('gpu', {}))
    )
    
    # Middle section - Memory and Network
    middle_section = Layout()
    middle_section.split_row(
        create_memory_panel(data.get('memory', {})),
        create_network_panel(data.get('network', {}))
    )
    
    # Bottom section - Processes
    processes_panel = create_processes_panel(data.get('processes', []))
    
    layout.split_column(
        top_section,
        middle_section,
        processes_panel
    )
    
    return layout

def create_cpu_panel(cpu_data):
    """Create CPU information panel"""
    usage = cpu_data.get('usage', 0)
    cores = cpu_data.get('cores', 0)
    frequency = cpu_data.get('frequency', 0)
    
    content = f"Usage: {usage:.1f}%\n"
    content += f"Cores: {cores}\n"
    if frequency > 0:
        content += f"Frequency: {frequency:.0f} MHz\n"
    
    # Core usage bars
    core_usage = cpu_data.get('coreUsage', [])
    if core_usage:
        content += "\nPer-Core Usage:\n"
        for i, core_usage_val in enumerate(core_usage[:8]):  # Show first 8 cores
            bar_length = int(core_usage_val / 2)
            bar = "█" * bar_length + "░" * (50 - bar_length)
            content += f"Core {i}: {bar} {core_usage_val:.1f}%\n"
    
    return Panel(content, title="[bold cyan]CPU[/bold cyan]", border_style="cyan")

def create_gpu_panel(gpu_data):
    """Create GPU information panel"""
    name = gpu_data.get('name', 'Unknown')
    usage = gpu_data.get('usage', 0)
    mem_used = gpu_data.get('memoryUsed', 0)
    mem_total = gpu_data.get('memoryTotal', 0)
    temp = gpu_data.get('temperature', 0)
    
    content = f"Name: {name}\n"
    content += f"Usage: {usage:.1f}%\n"
    if mem_total > 0:
        content += f"Memory: {mem_used:.0f} MB / {mem_total:.0f} MB\n"
    if temp > 0:
        content += f"Temperature: {temp:.1f}°C\n"
    
    return Panel(content, title="[bold magenta]GPU[/bold magenta]", border_style="magenta")

def create_memory_panel(mem_data):
    """Create memory information panel"""
    total = mem_data.get('total', 0)
    used = mem_data.get('used', 0)
    free = mem_data.get('free', 0)
    usage = mem_data.get('usagePercent', 0)
    
    content = f"Total: {total:.0f} MB\n"
    content += f"Used: {used:.0f} MB\n"
    content += f"Free: {free:.0f} MB\n"
    content += f"Usage: {usage:.1f}%\n"
    
    # Memory bar
    bar_length = int(usage / 2)
    bar = "█" * bar_length + "░" * (50 - bar_length)
    content += f"\n[{bar}]"
    
    return Panel(content, title="[bold green]Memory[/bold green]", border_style="green")

def create_network_panel(net_data):
    """Create network information panel"""
    download = net_data.get('downloadSpeed', 0)
    upload = net_data.get('uploadSpeed', 0)
    connections = net_data.get('activeConnections', 0)
    
    content = f"Download: {download:.2f} MB/s\n"
    content += f"Upload: {upload:.2f} MB/s\n"
    content += f"Active Connections: {connections}\n"
    
    return Panel(content, title="[bold yellow]Network[/bold yellow]", border_style="yellow")

def create_processes_panel(processes):
    """Create processes table"""
    table = Table(title="[bold]Top Processes[/bold]", show_header=True, header_style="bold")
    table.add_column("Name", style="cyan", no_wrap=True)
    table.add_column("PID", justify="right", style="magenta")
    table.add_column("CPU %", justify="right", style="green")
    table.add_column("Memory (MB)", justify="right", style="yellow")
    
    for proc in processes[:10]:
        table.add_row(
            proc.get('name', 'Unknown'),
            str(proc.get('pid', 0)),
            f"{proc.get('cpuUsage', 0):.1f}",
            f"{proc.get('memoryUsage', 0):.1f}"
        )
    
    return Panel(table, border_style="blue")

def main():
    monitor_exe = find_monitor_exe()
    
    if not monitor_exe:
        console.print("[bold red]ERROR:[/bold red] Monitor executable not found!")
        console.print("Please build the C++ monitor first:")
        console.print("  cd cpp")
        console.print("  mkdir build && cd build")
        console.print("  cmake ..")
        console.print("  cmake --build .")
        sys.exit(1)
    
    console.print(f"[green]Starting monitor from:[/green] {monitor_exe}")
    console.print("[yellow]Press Ctrl+C to exit[/yellow]\n")
    
    try:
        process = subprocess.Popen(
            [monitor_exe],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        
        with Live(create_layout({}), refresh_per_second=2, screen=True) as live:
            for line in process.stdout:
                try:
                    data = json.loads(line.strip())
                    live.update(create_layout(data))
                except json.JSONDecodeError:
                    continue
                except KeyboardInterrupt:
                    break
                except Exception as e:
                    console.print(f"[red]Error:[/red] {e}")
                    
    except KeyboardInterrupt:
        console.print("\n[yellow]Shutting down...[/yellow]")
        if 'process' in locals():
            process.terminate()
    except Exception as e:
        console.print(f"[bold red]Error:[/bold red] {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
