# ILI9341 System Monitor

**Version:** 4.0.0  
**Architecture:** Model-View-Controller (MVC)  
**Platform:** Raspberry Pi 3B+ with ILI9341 TFT Display  
**Framework:** Qt6 QML + C++17

---

## Project Structure

```
ili9341-system-v2/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
│
├── src/                        # Source code (MVC)
│   ├── main.cpp               # Application entry point
│   │
│   ├── model/                 # MODEL - Data & Business Logic
│   │   ├── CpuMonitor.cpp/h   # CPU metrics collection
│   │   ├── GpuMonitor.cpp/h   # GPU metrics (VideoCore)
│   │   ├── MemoryMonitor.cpp/h # RAM metrics
│   │   ├── StorageMonitor.cpp/h # Disk/Swap/IO metrics
│   │   ├── NetworkMonitor.cpp/h # Network traffic
│   │   └── SettingsManager.cpp/h # User settings & persistence
│   │
│   ├── controller/            # CONTROLLER - Application Logic
│   │   ├── SystemController.cpp/h # Main controller
│   │   └── NavigationController.cpp/h # Navigation state
│   │
│   └── common/                # COMMON - Utilities
│       ├── Constants.h        # App-wide constants
│       ├── FileReader.cpp/h   # /proc, /sys file reading
│       └── Logger.cpp/h       # Logging system
│
├── qml/                       # VIEW - User Interface
│   ├── Main.qml              # Root window & navigation
│   └── views/
│       ├── pages/            # Screen pages
│       │   ├── Dashboard.qml
│       │   ├── CpuDetail.qml
│       │   ├── MemoryDetail.qml
│       │   ├── StorageDetail.qml
│       │   ├── NetworkDetail.qml
│       │   └── Settings.qml
│       └── components/       # Reusable UI components
│           ├── BottomNav.qml
│           ├── TopBar.qml
│           ├── CircularGauge.qml
│           ├── LineChart.qml
│           ├── SpinBox.qml      # NEW: Touch-friendly input
│           ├── ConfirmDialog.qml # NEW: Confirmation modal
│           └── ...
│
├── resources/
│   └── qml.qrc               # Qt resource file
│
├── drivers/                  # Yocto recipes for kernel drivers
│   ├── recipes-kernel/       # ILI9341 framebuffer driver
│   └── recipes-bsp/          # Boot configuration
│
```

---

## Architecture

### MVC Pattern

```
┌─────────────────────────────────────────────────────────────┐
│                         VIEW (QML)                          │
│  Main.qml → Dashboard.qml, CpuDetail.qml, Settings.qml      │
│  Components: CircularGauge, LineChart, SpinBox, etc.        │
└─────────────────────────────┬───────────────────────────────┘
                              │ Q_PROPERTY bindings
                              │ signals/slots
┌─────────────────────────────▼───────────────────────────────┐
│                    CONTROLLER (C++)                         │
│                    SystemController                         │
│  - Aggregates all monitors                                  │
│  - Manages update timer                                     │
│  - Exposes data via Q_PROPERTY                              │
│  - Handles user actions                                     │
└─────────────────────────────┬───────────────────────────────┘
                              │ calls
┌─────────────────────────────▼───────────────────────────────┐
│                       MODEL (C++)                           │
│  CpuMonitor │ GpuMonitor │ MemoryMonitor │ StorageMonitor   │
│  NetworkMonitor │ SettingsManager                           │
│  - Parse /proc/stat, /proc/meminfo, /sys/class/*           │
│  - Business logic & data persistence                        │
└─────────────────────────────────────────────────────────────┘
```

---

## Build Instructions

### Prerequisites

- Qt6 (Core, Gui, Quick, Qml)
- CMake 3.16+
- GCC with C++17 support

### Desktop Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./ili9341-system
```

### Raspberry Pi Build (Cross-compile with Yocto)

```bash
# In your Yocto build environment
bitbake ili9341-system
```

---

## Features

### Monitoring
- **CPU:** Usage %, per-core, temperature, frequency, load average
- **GPU:** Temperature, memory allocation (VideoCore)
- **RAM:** Usage, used/free, cache, buffers
- **Storage:** Disk usage, swap, I/O read/write speeds
- **Network:** Upload/download, IP/MAC, connections, packets

### UI Features
- Real-time charts (temperature, I/O, network)
- Touch-friendly navigation
- Dark theme optimized for ILI9341
- Configurable warning thresholds
- System logs

### Settings
- Update interval (1-10 seconds)
- CPU/RAM warning thresholds
- Dark mode toggle
- Reboot/Shutdown with confirmation

---

## License

MIT License
