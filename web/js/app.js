// Initialize WebSocket connection
const socket = io();

// Create gradient function
function createGradient(ctx, colorStart, colorEnd) {
    const gradient = ctx.createLinearGradient(0, 0, 0, 400);
    gradient.addColorStop(0, colorStart);
    gradient.addColorStop(1, colorEnd);
    return gradient;
}

// Chart configurations
const chartConfig = {
    type: 'line',
    options: {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: { display: false },
            tooltip: { 
                enabled: true,
                backgroundColor: 'rgba(0, 0, 0, 0.9)',
                titleColor: '#ffffff',
                bodyColor: '#ffffff',
                borderColor: '#dc2626',
                borderWidth: 1
            }
        },
        scales: {
            y: {
                beginAtZero: true,
                max: 100,
                grid: { 
                    color: 'rgba(255, 255, 255, 0.05)',
                    lineWidth: 1
                },
                ticks: { 
                    color: '#888888',
                    font: { size: 11 }
                },
                border: { color: '#333333' }
            },
            x: {
                grid: { display: false },
                ticks: { display: false },
                border: { color: '#333333' }
            }
        }
    }
};

// Initialize charts with gradients
const cpuCtx = document.getElementById('cpu-chart').getContext('2d');
const cpuGradient = createGradient(cpuCtx, 'rgba(220, 38, 38, 0.3)', 'rgba(220, 38, 38, 0.05)');

const cpuChart = new Chart(cpuCtx, {
    ...chartConfig,
    data: {
        labels: [],
        datasets: [{
            label: 'CPU Usage',
            data: [],
            borderColor: '#dc2626',
            backgroundColor: cpuGradient,
            borderWidth: 2,
            fill: true,
            tension: 0.4,
            pointRadius: 0,
            pointHoverRadius: 4
        }]
    }
});

const gpuCtx = document.getElementById('gpu-chart').getContext('2d');
const gpuGradient = createGradient(gpuCtx, 'rgba(220, 38, 38, 0.25)', 'rgba(220, 38, 38, 0.05)');

const gpuChart = new Chart(gpuCtx, {
    ...chartConfig,
    data: {
        labels: [],
        datasets: [{
            label: 'GPU Usage',
            data: [],
            borderColor: '#ef4444',
            backgroundColor: gpuGradient,
            borderWidth: 2,
            fill: true,
            tension: 0.4,
            pointRadius: 0,
            pointHoverRadius: 4
        }]
    }
});

const memCtx = document.getElementById('mem-chart').getContext('2d');
const memGradient = createGradient(memCtx, 'rgba(220, 38, 38, 0.3)', 'rgba(220, 38, 38, 0.05)');

const memChart = new Chart(memCtx, {
    ...chartConfig,
    data: {
        labels: [],
        datasets: [{
            label: 'Memory Usage',
            data: [],
            borderColor: '#dc2626',
            backgroundColor: memGradient,
            borderWidth: 2,
            fill: true,
            tension: 0.4,
            pointRadius: 0,
            pointHoverRadius: 4
        }]
    }
});

const netCtx = document.getElementById('net-chart').getContext('2d');
const downloadGradient = createGradient(netCtx, 'rgba(220, 38, 38, 0.3)', 'rgba(220, 38, 38, 0.05)');
const uploadGradient = createGradient(netCtx, 'rgba(255, 255, 255, 0.2)', 'rgba(255, 255, 255, 0.05)');

const netChart = new Chart(netCtx, {
    ...chartConfig,
    options: {
        ...chartConfig.options,
        scales: {
            ...chartConfig.options.scales,
            y: {
                ...chartConfig.options.scales.y,
                max: undefined
            }
        }
    },
    data: {
        labels: [],
        datasets: [
            {
                label: 'Download',
                data: [],
                borderColor: '#dc2626',
                backgroundColor: downloadGradient,
                borderWidth: 2,
                fill: true,
                tension: 0.4,
                pointRadius: 0,
                pointHoverRadius: 4
            },
            {
                label: 'Upload',
                data: [],
                borderColor: '#ffffff',
                backgroundColor: uploadGradient,
                borderWidth: 2,
                fill: true,
                tension: 0.4,
                pointRadius: 0,
                pointHoverRadius: 4
            }
        ]
    }
});

// Chart data management
const maxDataPoints = 30;

function addDataPoint(chart, value) {
    const now = new Date();
    const timeLabel = now.getMinutes() + ':' + now.getSeconds();
    
    chart.data.labels.push(timeLabel);
    chart.data.datasets[0].data.push(value);
    
    if (chart.data.labels.length > maxDataPoints) {
        chart.data.labels.shift();
        chart.data.datasets[0].data.shift();
    }
    
    chart.update('none');
}

function addNetworkDataPoint(download, upload) {
    const now = new Date();
    const timeLabel = now.getMinutes() + ':' + now.getSeconds();
    
    netChart.data.labels.push(timeLabel);
    netChart.data.datasets[0].data.push(download);
    netChart.data.datasets[1].data.push(upload);
    
    if (netChart.data.labels.length > maxDataPoints) {
        netChart.data.labels.shift();
        netChart.data.datasets[0].data.shift();
        netChart.data.datasets[1].data.shift();
    }
    
    netChart.update('none');
}

// Update UI functions
function updateCPU(data) {
    const usage = data.usage || 0;
    document.getElementById('cpu-usage').textContent = usage.toFixed(1) + '%';
    document.getElementById('cpu-progress').style.width = usage + '%';
    document.getElementById('cpu-cores').textContent = data.cores || 0;
    document.getElementById('cpu-freq').textContent = (data.frequency || 0).toFixed(0) + ' MHz';
    
    addDataPoint(cpuChart, usage);
}

function updateGPU(data) {
    const usage = data.usage || 0;
    document.getElementById('gpu-usage').textContent = usage.toFixed(1) + '%';
    document.getElementById('gpu-progress').style.width = usage + '%';
    document.getElementById('gpu-name').textContent = data.name || 'Unknown';
    
    const memUsed = data.memoryUsed || 0;
    const memTotal = data.memoryTotal || 0;
    document.getElementById('gpu-memory').textContent = 
        memUsed.toFixed(0) + ' / ' + memTotal.toFixed(0) + ' MB';
    
    if (data.temperature > 0) {
        document.getElementById('gpu-temp').textContent = data.temperature.toFixed(1) + '°C';
        document.getElementById('gpu-temp-item').style.display = 'flex';
    }
    
    addDataPoint(gpuChart, usage);
}

function updateMemory(data) {
    const usage = data.usagePercent || 0;
    document.getElementById('mem-usage').textContent = usage.toFixed(1) + '%';
    document.getElementById('mem-progress').style.width = usage + '%';
    document.getElementById('mem-total').textContent = (data.total || 0).toFixed(0) + ' MB';
    document.getElementById('mem-used').textContent = (data.used || 0).toFixed(0) + ' MB';
    document.getElementById('mem-free').textContent = (data.free || 0).toFixed(0) + ' MB';
    
    addDataPoint(memChart, usage);
}

function updateNetwork(data) {
    const download = data.downloadSpeed || 0;
    const upload = data.uploadSpeed || 0;
    
    document.getElementById('net-download').textContent = download.toFixed(2) + ' MB/s';
    document.getElementById('net-upload').textContent = upload.toFixed(2) + ' MB/s';
    document.getElementById('net-connections').textContent = data.activeConnections || 0;
    
    addNetworkDataPoint(download, upload);
}

function updateDisks(disks) {
    const diskList = document.getElementById('disk-list');
    diskList.innerHTML = '';
    
    disks.forEach(disk => {
        const usagePercent = disk.total > 0 ? (disk.used / disk.total) * 100 : 0;
        
        const diskItem = document.createElement('div');
        diskItem.className = 'disk-item';
        diskItem.innerHTML = `
            <div class="disk-item-header">
                <span>${disk.name} (${disk.mountPoint})</span>
                <span>${usagePercent.toFixed(1)}%</span>
            </div>
            <div class="metric-item">
                <span class="label">Used:</span>
                <span>${disk.used.toFixed(2)} GB</span>
            </div>
            <div class="metric-item">
                <span class="label">Free:</span>
                <span>${disk.free.toFixed(2)} GB</span>
            </div>
            <div class="disk-progress">
                <div class="disk-progress-fill" style="width: ${usagePercent}%"></div>
            </div>
        `;
        diskList.appendChild(diskItem);
    });
}

function updateProcesses(processes) {
    const tbody = document.getElementById('process-tbody');
    tbody.innerHTML = '';
    
    processes.forEach(proc => {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${proc.name}</td>
            <td>${proc.pid}</td>
            <td>${proc.cpuUsage.toFixed(1)}</td>
            <td>${proc.memoryUsage.toFixed(1)}</td>
        `;
        tbody.appendChild(row);
    });
}

// WebSocket event handlers
socket.on('connect', () => {
    console.log('✅ Connected to server');
    document.getElementById('status').classList.add('connected');
    document.querySelector('#status span:last-child').textContent = 'Connected';
});

socket.on('disconnect', () => {
    console.log('❌ Disconnected from server');
    document.getElementById('status').classList.remove('connected');
    document.querySelector('#status span:last-child').textContent = 'Disconnected';
});

socket.on('system_update', (data) => {
    console.log('📊 Received system_update:', data);
    try {
        if (data.cpu) {
            console.log('Updating CPU:', data.cpu);
            updateCPU(data.cpu);
        }
        if (data.gpu) {
            console.log('Updating GPU:', data.gpu);
            updateGPU(data.gpu);
        }
        if (data.memory) {
            console.log('Updating Memory:', data.memory);
            updateMemory(data.memory);
        }
        if (data.network) {
            console.log('Updating Network:', data.network);
            updateNetwork(data.network);
        }
        if (data.disks) {
            console.log('Updating Disks:', data.disks);
            updateDisks(data.disks);
        }
        if (data.processes) {
            console.log('Updating Processes:', data.processes);
            updateProcesses(data.processes);
        }
    } catch (error) {
        console.error('Error updating UI:', error);
    }
});

socket.on('error', (data) => {
    console.error('❌ Error:', data);
    if (data && data.message) {
        alert('Error: ' + data.message);
    }
});

// Debug: Log all socket events
socket.onAny((event, ...args) => {
    console.log('🔔 Socket event:', event, args);
});
