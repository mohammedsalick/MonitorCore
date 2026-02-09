#include "../include/system_monitor.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    SystemMonitor monitor;
    
    if (!monitor.initialize()) {
        std::cerr << "Failed to initialize system monitor" << std::endl;
        return 1;
    }

    // Initial update
    monitor.update();

    // Main loop - output JSON every second
    while (true) {
        monitor.update();
        std::cout << monitor.toJSON() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
