# libqcperf

A lightweight, open-source performance profiling library for Qualcomm chipsets.

## Table of Contents
- [Introduction](#introduction)
- [Framework Features](#framework-features)
- [Build System](#build-system)
- [Compilation Instructions](#compilation-instructions)
- [Design Diagrams](#design-diagrams)
- [Documentation](#documentation)

## Introduction

libqcperf is a performance profiling library designed for Qualcomm chipsets across different SoCs and subsystems. It provides a unified framework for monitoring and analyzing system performance metrics in real-time, enabling developers to optimize their applications for Qualcomm platforms.

The library is designed with modularity and extensibility in mind, featuring a core API that interfaces with multiple specialized backends. Each backend focuses on specific performance aspects such as thermal monitoring, memory bandwidth, or power consumption. This architecture allows developers to collect precisely the metrics they need without unnecessary overhead.

Originally inspired by the Qualcomm Profiler tool, libqcperf is now available as an open-source project, empowering the developer community to build from source, customize functionality, and extend capabilities through new backends or integrations with other tools.

## Framework Features

### Core Capabilities
- Modular backend architecture for different monitoring needs
- Configurable sampling and streaming rates
- Dynamic capability discovery
- Cross-platform support (Windows and Linux)
- Asynchronous data collection via background threads
- Non-blocking callback-based data delivery

### Available Backends

| Backend | Description | Metrics & Configuration Reference |
|---------|-------------|-----------------------------------|
| **Thermal Backend** | Monitors temperature and passive cooling metrics across 22 thermal zones | [wos_thermal_info.h](qcperf/backends/wos-thermal/inc/wos_thermal_info.h) |
| **Power Backend** | Measures power consumption across various system components | [wos_power_backend_info.h](qcperf/backends/wos-power-backend/inc/wos_power_backend_info.h) |
| **Dummy Backend** | Reference implementation for testing and development | [dummy_info.h](qcperf/backends/dummy/inc/dummy_info.h) |

> **Note:** Each backend's info.h file contains detailed information about supported capability, metrics, sampling rates, and streaming rates.

### Technical Foundation
- Relies on publicly available Windows driver APIs (ETW, PEP, IOCTL)
- Comprehensive error handling and resource management
- Thread-safe operations for multi-threaded applications

## Build System

- **CMake**
- **Compiler**
  - Windows on Snapdragon - Visual Studio 2022

## Compilation Instructions

### Windows ARM64

To build the library for Windows on ARM64 platforms:

```bash
# Clone the repository
git clone https://github.com/qualcomm/libqcperf.git
cd libqcperf

# Clone third-party dependencies
git submodule update --init --recursive

# Sample test app path
qcperf\test_app

# Compilation for Windows ARM64
## Generate build files
cmake -B <build_dir_path> -G "Visual Studio 17 2022" -A ARM64 -DProjectVersion="0.1.0.0"

## Compile the project
cmake --build <build_dir_path> --config Release
```

## Design Diagrams

### Sequence Diagram
- [QcPerf Sequence Diagram](./assets/libqcperf_sequence.mmd)

### Flow Diagram
- [QcPerf Flow Diagram](./assets/libqcperf_flow.mmd)

### Backend Architecture
- [QcPerf Backend Architecture](./assets/libqcperf_backend_architecture.mmd)

## Documentation for backend developers

For detailed information about backend development, coding standards, and extending the library, please refer to the [DEVELOPMENT-GUIDE.md](./DEVELOPMENT-GUIDE.md) file in the root of the repository.

## Contributing

We welcome contributions to libqcperf! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) file for details on our code of conduct and the process for submitting pull requests.

## Security

For information on reporting security vulnerabilities, please see our [SECURITY.md](SECURITY.md) file.

## License

This project is licensed under the BSD 3-Clause License. See the [LICENSE.txt](LICENSE.txt) file for full details.

Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
