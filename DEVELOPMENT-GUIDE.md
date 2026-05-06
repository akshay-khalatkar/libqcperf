# libqcperf Development Guide

This document provides comprehensive information for developers who want to contribute to or extend the libqcperf library. It covers coding standards, architecture details, and guides for implementing new backends.

## Table of Contents
- [libqcperf Development Guide](#libqcperf-development-guide)
  - [Table of Contents](#table-of-contents)
  - [Project Architecture](#project-architecture)
    - [Core Library](#core-library)
    - [Backend Interface](#backend-interface)
    - [Data Flow](#data-flow)
    - [API Workflow](#api-workflow)
  - [Core Functionalities](#core-functionalities)
    - [Backend Abstraction and Extensibility](#backend-abstraction-and-extensibility)
    - [Dynamic Backend Management](#dynamic-backend-management)
    - [Data Collection](#data-collection)
    - [Error Handling and Reporting](#error-handling-and-reporting)
    - [Backend Implementations](#backend-implementations)
      - [Dummy Backend](#dummy-backend)
      - [Thermal Backend](#thermal-backend)
      - [Power Backend](#power-backend)
  - [Adding New Backends](#adding-new-backends)
    - [Step 1: Create Backend Header Files](#step-1-create-backend-header-files)
      - [Main Backend Header (`your_backend.h`)](#main-backend-header-your_backendh)
      - [Backend Info Header (`your_backend_info.h`)](#backend-info-header-your_backend_infoh)
    - [Step 2: Implement Backend Source Files](#step-2-implement-backend-source-files)
      - [Backend Info Implementation (`your_backend_info.c`)](#backend-info-implementation-your_backend_infoc)
      - [Main Backend Implementation (`your_backend.c`)](#main-backend-implementation-your_backendc)
    - [Step 3: Register the Backend with QcPerf](#step-3-register-the-backend-with-qcperf)
      - [Update Backend Enum (`qcperf_backend_enum.h`)](#update-backend-enum-qcperf_backend_enumh)
      - [Update Backend Registry (`qcperf_backends.h`)](#update-backend-registry-qcperf_backendsh)
    - [Step 4: Update CMake Build Files](#step-4-update-cmake-build-files)
      - [How `QCPERF_ENABLED_*` flags are derived](#how-qcperf_enabled_-flags-are-derived)
      - [4a. Register the Backend in `cmake/BuildConfig.cmake`](#4a-register-the-backend-in-cmakebuildconfigcmake)
      - [4b. Create `backends/your-backend/CMakeLists.txt`](#4b-create-backendsyour-backendcmakeliststxt)
      - [4c. Update `backends/CMakeLists.txt`](#4c-update-backendscmakeliststxt)
      - [4d. Update `backends/inc/CMakeLists.txt`](#4d-update-backendsinccmakeliststxt)
      - [CMake Files Summary](#cmake-files-summary)
    - [Testing Your Backend](#testing-your-backend)
      - [Best Practices](#best-practices)
  - [Coding Guidelines](#coding-guidelines)
    - [Naming Conventions](#naming-conventions)
      - [Variables and Functions](#variables-and-functions)
      - [Structures and Enums](#structures-and-enums)
      - [Defines and Macros](#defines-and-macros)
      - [Header Guards](#header-guards)
    - [Type Definitions](#type-definitions)
      - [Typedef Usage](#typedef-usage)
      - [Pointer Type Definitions](#pointer-type-definitions)
    - [File Organization](#file-organization)
      - [Include Order](#include-order)
      - [End of File](#end-of-file)
      - [Cross-Platform Implementation](#cross-platform-implementation)
    - [Code Structure](#code-structure)
      - [Variable Declaration](#variable-declaration)
      - [Code Blocks](#code-blocks)
    - [Documentation](#documentation)
      - [Doxygen Documentation](#doxygen-documentation)
      - [Inline Comments](#inline-comments)
      - [File Headers](#file-headers)
    - [Control Flow](#control-flow)
      - [Single Return (Single Exit Point)](#single-return-single-exit-point)
      - [No goto Statements](#no-goto-statements)
      - [Avoid Always-True Loops](#avoid-always-true-loops)
    - [Formatting](#formatting)
      - [Indentation](#indentation)
      - [Spacing](#spacing)
      - [Code Formatting Before Commit](#code-formatting-before-commit)
    - [Error Handling](#error-handling)
      - [Return Error Codes](#return-error-codes)
      - [Null Pointer Checks](#null-pointer-checks)
      - [Assertions for Debug Builds](#assertions-for-debug-builds)
    - [Best Practices](#best-practices-1)
      - [Pointer Comparisons](#pointer-comparisons)
      - [Const Correctness](#const-correctness)
      - [Memory Management](#memory-management)
      - [Magic Numbers](#magic-numbers)
      - [Function Length](#function-length)
      - [Header File Organization](#header-file-organization)
      - [Backend Header Organization](#backend-header-organization)
      - [Resource Cleanup](#resource-cleanup)
      - [Thread Safety](#thread-safety)
    - [Summary Checklist](#summary-checklist)

---

## Project Architecture

### Core Library

The core library provides the main API for initialization, backend connection, data collection, and cleanup. It's responsible for:

- Managing backend lifecycle
- Handling communication between applications and backends
- Providing error reporting and resource management
- Exposing a unified API for performance monitoring

Key files:
- `qcperf/core/inc/qcperf.h`: Main public API
- `qcperf/core/inc/qcperf_common.h`: Common data structures and definitions
- `qcperf/core/src/qcperf.c`: Implementation of the core API

### Backend Interface

The backend interface defines a common set of functions that all backends must implement. This enables the modular design where backends can be added without modifying core code.

Key files:
- `qcperf/core/inc/internal/qcperf_backend_interface.h`: Backend interface definition
- `qcperf/backends/inc/qcperf_backend_enum.h`: Backend identifier enumeration
- `qcperf/backends/inc/qcperf_backends.h`: Backend registration

The interface uses function pointers for:
- Initialization and deinitialization
- Data collection and delivery
- Capability discovery
- Callback registration

### Data Flow

The library implements an asynchronous data collection model:

1. Applications initialize the library and connect to backends
2. Applications register callbacks for receiving data and messages
3. Backends create background threads for data collection
4. Data is collected at specified sampling rates
5. Results are delivered to applications via callbacks at specified streaming rates
6. Applications process and display the data as needed

### API Workflow

The typical usage pattern follows these steps:

1. `qcperf_init()` - Initialize the library
2. `qcperf_connect_backend()` - Connect to a specific backend
3. `qcperf_get_capabilities_info()` - Discover available capabilities and metrics
4. `qcperf_set_data_callback()` - Register callback for receiving performance data
5. `qcperf_start()` - Begin performance monitoring with specified parameters
6. Process data via callbacks
7. `qcperf_stop()` - Stop performance monitoring
8. `qcperf_disconnect_backend()` - Disconnect from the backend
9. `qcperf_deinit()` - Clean up library resources

---

## Core Functionalities

### Backend Abstraction and Extensibility
- Unified interface for multiple backend types
- Easy addition of new backends without core modifications
- Backend-specific implementations hidden behind common API

### Dynamic Backend Management
- On-demand connection/disconnection to backends
- Runtime discovery of backend capabilities
- Support for multiple simultaneous backend connections

### Data Collection
- Asynchronous data collection via background threads
- Configurable sampling and streaming rates
- Support for various metric types (numeric, boolean, string)
- Non-blocking callback-based data delivery

### Error Handling and Reporting
- Detailed error codes with descriptive messages
- Consistent error propagation across layers
- Message callback system for warnings and information

### Backend Implementations

The library includes several specialized backend implementations:

#### Dummy Backend
- Reference implementation for testing and demonstration
- Generates synthetic performance metrics
- Useful for development and testing without hardware dependencies
- Simulates multiple capabilities with configurable metrics

#### Thermal Backend
- Monitors temperature and passive cooling metrics for various thermal zones
- Covers 22 thermal zones including CPU clusters, GPU, and critical thermal zones
- Provides real-time temperature data in degrees Celsius
- Interfaces with Windows thermal monitoring subsystem via ETW

#### Power Backend
- Monitors power consumption across various system components
- Provides metrics for CPU, GPU, and system power consumption
- All power metrics measured in milliwatts (mW)
- Provides average power consumption over sampling period

#### DSP NPU Backend (Linux ARM64)
- Collects performance metrics from the Qualcomm CDSP/NPU hardware via FastRPC
- Provides Q6 utilization, Q6 clock, HVX utilization, and HMX utilization metrics
- Communicates with the DSP using the `sysmonquery` interface over FastRPC

##### Runtime Dependency: `libcdsprpc.so`

The DSP NPU backend requires `libcdsprpc.so` to be present on the target device at runtime. This library is part of the Qualcomm BSP (Board Support Package) and provides the FastRPC transport used to communicate with the CDSP.

> **Note:** `libcdsprpc.so` is **not** built from source — the build system compiles a stub (`cdsprpc_stub.c`) that satisfies the linker at compile time. The real `libcdsprpc.so` must be present on the device at runtime, typically at `/usr/lib/libcdsprpc.so`.

**What happens if `libcdsprpc.so` is missing:**

| Scenario | Behavior |
|----------|----------|
| `libcdsprpc.so` absent from device | Dynamic linker fails to load the application — process does not start. No graceful fallback. |
| `libcdsprpc.so` present, but CDSP inaccessible | `qcom_dsp_init()` returns a non-success error code. `dsp_npu_init()` reports the failure via the message callback at `QC_PERF_MESSAGE_LEVEL_ERROR` and returns `QC_PERF_RETURN_CODE_FAILED`. |

**Recommendation:** Before running an application that uses the DSP NPU backend, verify the library is present on the device:
```bash
ls /usr/lib/libcdsprpc.so
```

---

## Adding New Backends

This section explains the process of adding a new backend to the QcPerf library. The modular architecture makes it straightforward to extend the library with new performance monitoring capabilities.

### Step 1: Create Backend Header Files

Create the following header files for your backend:

#### Main Backend Header (`your_backend.h`)

```c
#ifndef YOUR_BACKEND_H
#define YOUR_BACKEND_H

#include "your_backend_info.h"

struct QcPerfBackendPrivate;

/**
 * @brief Create and initialize your backend
 *
 * @param[in,out] backend Pointer to backend private structure to be configured
 *
 * @return QC_PERF_RETURN_CODE_SUCCESS on successful creation
 * @return QC_PERF_RETURN_CODE_NULL_POINTER if backend pointer is NULL
 */
enum QcPerfReturnCode qcperf_your_backend_create(struct QcPerfBackendPrivate* backend);

#endif  // YOUR_BACKEND_H
```

#### Backend Info Header (`your_backend_info.h`)

```c
#ifndef YOUR_BACKEND_INFO_H
#define YOUR_BACKEND_INFO_H

#include "qcperf_common.h"

// Define capability ID and length
#define YOUR_BACKEND_CAPABILITY_ID 0
#define YOUR_BACKEND_CAPABILITIES_LEN 1

// Define streaming and sampling rates
#define YOUR_BACKEND_STREAMING_RATES_LEN 2
#define YOUR_BACKEND_STREAMING_RATES 200, 1000

#define YOUR_BACKEND_SAMPLING_RATES_LEN 2
#define YOUR_BACKEND_SAMPLING_RATES 50, 100

// Define capability name and metric count
#define YOUR_BACKEND_CAPABILITY "your-capability"
#define YOUR_BACKEND_CAPABILITY_METRIC_COUNT 3  // Adjust based on your metrics

// Define metric indices
enum YourBackendMetricIndex {
    YOUR_BACKEND_METRIC_INDEX_1 = 0,
    YOUR_BACKEND_METRIC_INDEX_2,
    YOUR_BACKEND_METRIC_INDEX_3
};

// Define metrics
#define YOUR_BACKEND_METRIC_1_ID 0
#define YOUR_BACKEND_METRIC_1_NAME "Metric 1 Name"
#define YOUR_BACKEND_METRIC_1_DESCRIPTION "Description of metric 1"
#define YOUR_BACKEND_METRIC_1_UNIT "Unit"

#define YOUR_BACKEND_METRIC_2_ID 1
#define YOUR_BACKEND_METRIC_2_NAME "Metric 2 Name"
#define YOUR_BACKEND_METRIC_2_DESCRIPTION "Description of metric 2"
#define YOUR_BACKEND_METRIC_2_UNIT "Unit"

#define YOUR_BACKEND_METRIC_3_ID 2
#define YOUR_BACKEND_METRIC_3_NAME "Metric 3 Name"
#define YOUR_BACKEND_METRIC_3_DESCRIPTION "Description of metric 3"
#define YOUR_BACKEND_METRIC_3_UNIT "Unit"

/**
 * @brief Initialize your backend metrics data
 *
 * @param[out] metrics_data Array to be populated with metric information
 */
void your_backend_capability_init_metrics(struct QcPerfMetricInfo* metrics_data);

#endif /* YOUR_BACKEND_INFO_H */
```

### Step 2: Implement Backend Source Files

#### Backend Info Implementation (`your_backend_info.c`)

```c
#include "your_backend_info.h"
#include <string.h>

void your_backend_capability_init_metrics(struct QcPerfMetricInfo* metrics_data) {
    // Metric 1
    metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_id = YOUR_BACKEND_METRIC_1_ID;
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_name, YOUR_BACKEND_METRIC_1_NAME, METRIC_NAME_MAX_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_name_len = strlen(YOUR_BACKEND_METRIC_1_NAME);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_description, YOUR_BACKEND_METRIC_1_DESCRIPTION, MAX_METRIC_DESCRIPTION_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_description_len = strlen(YOUR_BACKEND_METRIC_1_DESCRIPTION);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_unit, YOUR_BACKEND_METRIC_1_UNIT, MAX_METRIC_UNIT_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_1].metric_unit_len = strlen(YOUR_BACKEND_METRIC_1_UNIT);

    // Metric 2
    metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_id = YOUR_BACKEND_METRIC_2_ID;
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_name, YOUR_BACKEND_METRIC_2_NAME, METRIC_NAME_MAX_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_name_len = strlen(YOUR_BACKEND_METRIC_2_NAME);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_description, YOUR_BACKEND_METRIC_2_DESCRIPTION, MAX_METRIC_DESCRIPTION_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_description_len = strlen(YOUR_BACKEND_METRIC_2_DESCRIPTION);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_unit, YOUR_BACKEND_METRIC_2_UNIT, MAX_METRIC_UNIT_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_2].metric_unit_len = strlen(YOUR_BACKEND_METRIC_2_UNIT);

    // Metric 3
    metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_id = YOUR_BACKEND_METRIC_3_ID;
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_name, YOUR_BACKEND_METRIC_3_NAME, METRIC_NAME_MAX_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_name_len = strlen(YOUR_BACKEND_METRIC_3_NAME);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_description, YOUR_BACKEND_METRIC_3_DESCRIPTION, MAX_METRIC_DESCRIPTION_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_description_len = strlen(YOUR_BACKEND_METRIC_3_DESCRIPTION);
    strncpy(metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_unit, YOUR_BACKEND_METRIC_3_UNIT, MAX_METRIC_UNIT_LEN);
    metrics_data[YOUR_BACKEND_METRIC_INDEX_3].metric_unit_len = strlen(YOUR_BACKEND_METRIC_3_UNIT);
}
```

#### Main Backend Implementation (`your_backend.c`)

```c
#include "your_backend.h"
#include "your_backend_info.h"
#include "qcperf_backend_interface.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Private data structures
static QcPerfMessageCallback g_message_callback = NULL;
static QcPerfDataCallback g_data_callback = NULL;
static bool g_is_thread_running = false;
static struct QcPerfBackendInfo g_backend_info;

// Forward declarations of private functions
static enum QcPerfReturnCode your_backend_set_message_callback(QcPerfMessageCallback message_callback);
static enum QcPerfReturnCode your_backend_init(void);
static enum QcPerfReturnCode your_backend_info(struct QcPerfBackendInfo* backend_info);
static enum QcPerfReturnCode your_backend_set_data_callback(QcPerfDataCallback data_callback);
static enum QcPerfReturnCode your_backend_start(struct QcPerfRequest* request);
static enum QcPerfReturnCode your_backend_stop(struct QcPerfRequest* request);
static enum QcPerfReturnCode your_backend_deinit(void);

// Implementation of backend interface functions
static enum QcPerfReturnCode your_backend_set_message_callback(QcPerfMessageCallback message_callback) {
    g_message_callback = message_callback;
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_init(void) {
    // Initialize your backend here
    // Allocate resources, connect to drivers, etc.
    
    // Initialize metrics
    struct QcPerfMetricInfo* metrics_data = (struct QcPerfMetricInfo*)calloc(YOUR_BACKEND_CAPABILITY_METRIC_COUNT, sizeof(struct QcPerfMetricInfo));
    if (NULL == metrics_data) {
        return QC_PERF_RETURN_CODE_CALLOC_FAILED;
    }
    
    your_backend_capability_init_metrics(metrics_data);
    
    // Initialize capability info
    g_backend_info.backend_id = YOUR_BACKEND_ID; // Define this in backend_enum.h
    g_backend_info.capabilities_list = (struct QcPerfCapabilityInfo*)calloc(YOUR_BACKEND_CAPABILITIES_LEN, sizeof(struct QcPerfCapabilityInfo));
    if (NULL == g_backend_info.capabilities_list) {
        free(metrics_data);
        metrics_data = NULL;
        return QC_PERF_RETURN_CODE_CALLOC_FAILED;
    }
    
    g_backend_info.capabilities_list_length = YOUR_BACKEND_CAPABILITIES_LEN;
    
    // Set up capability info
    g_backend_info.capabilities_list[0].capability_id = YOUR_BACKEND_CAPABILITY_ID;
    strncpy(g_backend_info.capabilities_list[0].capability_name, YOUR_BACKEND_CAPABILITY, CAPABILITY_NAME_MAX_LEN);
    g_backend_info.capabilities_list[0].capability_name_len = strlen(YOUR_BACKEND_CAPABILITY);
    g_backend_info.capabilities_list[0].metric_ids_list = metrics_data;
    g_backend_info.capabilities_list[0].metric_ids_list_len = YOUR_BACKEND_CAPABILITY_METRIC_COUNT;
    
    // Set up streaming rates
    g_backend_info.capabilities_list[0].streaming_rate_len = YOUR_BACKEND_STREAMING_RATES_LEN;
    uint16_t streaming_rates[] = {YOUR_BACKEND_STREAMING_RATES};
    memcpy_s(g_backend_info.capabilities_list[0].streaming_rate, MAX_SAMPLING_STREAMING_RATES_LEN * sizeof(uint16_t), 
           streaming_rates, YOUR_BACKEND_STREAMING_RATES_LEN * sizeof(uint16_t));
    
    // Set up sampling rates
    g_backend_info.capabilities_list[0].sampling_rate_len = YOUR_BACKEND_SAMPLING_RATES_LEN;
    uint16_t sampling_rates[] = {YOUR_BACKEND_SAMPLING_RATES};
    memcpy_s(g_backend_info.capabilities_list[0].sampling_rate, MAX_SAMPLING_STREAMING_RATES_LEN * sizeof(uint16_t), 
           sampling_rates, YOUR_BACKEND_SAMPLING_RATES_LEN * sizeof(uint16_t));
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_info(struct QcPerfBackendInfo* backend_info) {
    if (NULL == backend_info) {
        return QC_PERF_RETURN_CODE_NULL_POINTER;
    }
    
    *backend_info = g_backend_info;
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_set_data_callback(QcPerfDataCallback data_callback) {
    if (NULL == data_callback) {
        return QC_PERF_RETURN_CODE_NULL_POINTER;
    }
    
    g_data_callback = data_callback;
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_start(struct QcPerfRequest* request) {
    // Implement your backend start logic here
    // Create threads, start data collection, etc.
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_stop(struct QcPerfRequest* request) {
    // Implement your backend stop logic here
    // Stop threads, clean up resources, etc.
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}

static enum QcPerfReturnCode your_backend_deinit(void) {
    // Clean up all resources allocated by your backend
    
    // Free capability info
    if (g_backend_info.capabilities_list != NULL) {
        for (int i = 0; i < g_backend_info.capabilities_list_length; i++) {
            if (g_backend_info.capabilities_list[i].metric_ids_list != NULL) {
                free(g_backend_info.capabilities_list[i].metric_ids_list);
                g_backend_info.capabilities_list[i].metric_ids_list = NULL;
            }
        }
        free(g_backend_info.capabilities_list);
        g_backend_info.capabilities_list = NULL;
    }
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}

enum QcPerfReturnCode qcperf_your_backend_create(struct QcPerfBackendPrivate* backend) {
    if (NULL == backend) {
        return QC_PERF_RETURN_CODE_NULL_POINTER;
    }
    
    // Set up function pointers
    backend->set_message_callback = your_backend_set_message_callback;
    backend->qcperf_backend_init = your_backend_init;
    backend->qcperf_backend_info = your_backend_info;
    backend->set_data_callback = your_backend_set_data_callback;
    backend->qcperf_backend_start = your_backend_start;
    backend->qcperf_backend_stop = your_backend_stop;
    backend->qcperf_backend_deinit = your_backend_deinit;
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}
```

### Step 3: Register the Backend with QcPerf

#### Update Backend Enum (`qcperf_backend_enum.h`)

Add your backend's enum value to `QcPerfBackendId`. 

```c
enum QcPerfBackendId {
    QC_PERF_BACKEND_DUMMY = 0,
    QC_PERF_BACKEND_QCOM_LINUX_CPU,
    QC_PERF_BACKEND_DSP_NPU,
    QC_PERF_BACKEND_POWER,
    QC_PERF_BACKEND_THERMAL,
    QC_PERF_BACKEND_YOUR_BACKEND,  // Add your backend here
    QC_PERF_BACKEND_MAX,
};
```

> **Important:** The enum order must match the array order in `qcperf_backends.h` exactly.

#### Update Backend Registry (`qcperf_backends.h`)

Add your backend's include and a `NULL`/`&init_fn` entry in `backend_init_fns[]`, guarded by `QCPERF_ENABLED_YOUR_BACKEND`. The `NULL` entry ensures the array index stays aligned with the enum even when the backend is not compiled in:

```c
#if defined(QCPERF_ENABLED_YOUR_BACKEND)
#include "your_backend.h"
#endif

static backend_init_t backend_init_fns[] = {
    /* ... existing entries ... */
#if defined(QCPERF_ENABLED_YOUR_BACKEND)
    &qcperf_your_backend_create,    /* QC_PERF_BACKEND_YOUR_BACKEND */
#else
    NULL,                           /* QC_PERF_BACKEND_YOUR_BACKEND (disabled) */
#endif
};
```

### Step 4: Update CMake Build Files

The build system uses a **selective backend mechanism**: registering a backend name in `cmake/BuildConfig.cmake` automatically derives a `QCPERF_ENABLED_*` compile definition that gates both the CMake build targets and the C preprocessor includes in `qcperf_backends.h`. Four files must be updated.

#### How `QCPERF_ENABLED_*` flags are derived

The flag name is constructed from the backend name as registered in `QCPERF_PLATFORM_SUPPORTED_BACKENDS`:

- If no OS prefix is set, the flag is `QCPERF_ENABLED_<NAME>`.  
  Example: registering `QCOM_LINUX_CPU` → `QCPERF_ENABLED_QCOM_LINUX_CPU`
- If an OS prefix is configured via `QCPERF_BACKEND_OS_PREFIX_<NAME>`, the flag becomes `QCPERF_ENABLED_<OS_PREFIX>_<NAME>`.  
  Example: registering `THERMAL` with prefix `WOS` → `QCPERF_ENABLED_WOS_THERMAL`

Use this flag consistently across all four files below.

---

#### 4a. Register the Backend in `cmake/BuildConfig.cmake`

Each platform has its own `list(APPEND QCPERF_PLATFORM_SUPPORTED_BACKENDS ...)` call in `cmake/BuildConfig.cmake`. **Append your backend's nickname to the list for the target OS.** The table below shows the existing nicknames per platform and the `QCPERF_ENABLED_*` flag each one produces:

| Platform | CMake guard variable | Registered nicknames | Derived `QCPERF_ENABLED_*` flag |
|----------|---------------------|----------------------|----------------------------------|
| All platforms | *(always included)* | `DUMMY` | `QCPERF_ENABLED_DUMMY` |
| Linux ARM64 | `QCPERF_PLATFORM_LINUX_ARM64` | `QCOM_LINUX_CPU` | `QCPERF_ENABLED_QCOM_LINUX_CPU` |
| Linux ARM64 | `QCPERF_PLATFORM_LINUX_ARM64` | `QCOM_LINUX_NPU` | `QCPERF_ENABLED_QCOM_LINUX_NPU` |
| Windows ARM64 | `QCPERF_PLATFORM_WINDOWS_ARM64` | `WOS_THERMAL` | `QCPERF_ENABLED_WOS_THERMAL` |
| Windows ARM64 | `QCPERF_PLATFORM_WINDOWS_ARM64` | `WOS_POWER` | `QCPERF_ENABLED_WOS_POWER` |

Add your backend's nickname to the appropriate platform block. This is the **only change needed** in `BuildConfig.cmake` for most backends:

```cmake
# Example: register YOUR_BACKEND for Linux ARM64
# Append YOUR_BACKEND to the existing Linux ARM64 list
if(QCPERF_PLATFORM_LINUX_ARM64)
    list(APPEND QCPERF_PLATFORM_SUPPORTED_BACKENDS QCOM_LINUX_CPU QCOM_LINUX_NPU YOUR_BACKEND)
endif()

# Example: register YOUR_BACKEND for Windows ARM64
# Append YOUR_BACKEND to the existing Windows ARM64 list
if(QCPERF_PLATFORM_WINDOWS_ARM64)
    list(APPEND QCPERF_PLATFORM_SUPPORTED_BACKENDS WOS_THERMAL WOS_POWER YOUR_BACKEND)
endif()
```

The compile definition `QCPERF_ENABLED_YOUR_BACKEND` will be set automatically — no further changes to `BuildConfig.cmake` are needed.

> **Note:** If your backend requires an OS prefix (e.g., `WOS` for Windows on Snapdragon backends), set `QCPERF_BACKEND_OS_PREFIX_YOUR_BACKEND` to the appropriate prefix so the flag becomes `QCPERF_ENABLED_WOS_YOUR_BACKEND`. Existing WOS backends use this pattern.

---

#### 4b. Create `backends/your-backend/CMakeLists.txt`

Create a `CMakeLists.txt` in your backend directory. Follow the naming and include conventions used by existing backends:

```cmake
# ============================================================================
# QcPerf Your Backend
# ============================================================================
# Brief description of what this backend monitors.

message("Added your-backend backend")

# Create the backend library — use QcPerf<Name>Backend naming convention
add_library(QcPerfYourBackend STATIC
    src/your_backend.c
    src/your_backend_info.c
)

target_include_directories(QcPerfYourBackend PUBLIC
    inc
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/core/inc/internal
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc
)

target_link_libraries(QcPerfYourBackend
    QcPerfQThread
    QcPerfQTime
    ${_OS_M}
)
```

Key conventions to follow:
- **Library target name**: `QcPerf<Name>Backend` (e.g., `QcPerfDummyBackend`, `QcPerfQcomLinuxCpuBackend`)
- **Always include** `${CMAKE_SOURCE_DIR}/core/inc`, `${CMAKE_SOURCE_DIR}/core/inc/internal`, and `${CMAKE_SOURCE_DIR}/backends/inc` so the backend can access the core API and backend interface headers
- **Always link** `QcPerfQThread` and `QcPerfQTime` for threading and timing utilities
- **`${_OS_M}`** links the math library on Linux (set automatically by `BuildConfig.cmake`)
- Do **not** add `cmake_minimum_required()` — it is set once in the top-level `CMakeLists.txt`

---

#### 4c. Update `backends/CMakeLists.txt`

Add an `add_subdirectory` call guarded by the `QCPERF_ENABLED_*` flag. Place it in the appropriate platform section:

```cmake
# Linux ARM64 backends
if(QCPERF_ENABLED_YOUR_BACKEND)
    add_subdirectory(your-backend)
endif()
```

The directory name passed to `add_subdirectory` must match the actual directory name under `backends/`.

---

#### 4d. Update `backends/inc/CMakeLists.txt`

Add your backend's include directory and link its library into the `QcPerfBackends` interface target. The `QcPerfBackends` target is what the core library links against, so this step makes your backend available to the core:

```cmake
if(QCPERF_ENABLED_YOUR_BACKEND)
    target_include_directories(QcPerfBackends INTERFACE
        ${CMAKE_SOURCE_DIR}/backends/your-backend/inc
    )
    target_link_libraries(QcPerfBackends INTERFACE
        QcPerfYourBackend
    )
endif()
```

The library name in `target_link_libraries` must match the `add_library` target name defined in your backend's `CMakeLists.txt` (step 4b).

---

#### CMake Files Summary

The table below lists every file that must be modified when adding a new backend:

| File | What to add |
|------|-------------|
| `cmake/BuildConfig.cmake` | Add backend name to `QCPERF_PLATFORM_SUPPORTED_BACKENDS` for the target platform |
| `backends/your-backend/CMakeLists.txt` | **Create new file** — `add_library(QcPerfYourBackend ...)` with correct includes and link libraries |
| `backends/CMakeLists.txt` | Add `if(QCPERF_ENABLED_YOUR_BACKEND) add_subdirectory(your-backend) endif()` |
| `backends/inc/CMakeLists.txt` | Add `if(QCPERF_ENABLED_YOUR_BACKEND)` block with `target_include_directories` and `target_link_libraries` into `QcPerfBackends` |

### Testing Your Backend

1. Build the QcPerf library with your new backend
2. Modify the test application to use your backend
3. Run tests to verify functionality

#### Best Practices

1. **Error Handling**: Implement robust error handling in all functions
2. **Resource Management**: Ensure proper allocation and deallocation of resources
3. **Thread Safety**: Make your backend thread-safe if it will be used in multi-threaded applications
4. **Documentation**: Document your backend's API and metrics thoroughly
5. **Testing**: Create comprehensive tests for your backend

---

## Coding Guidelines

This section outlines the coding standards and best practices for the libqcperf project. All contributors must adhere to these guidelines to maintain code consistency, readability, and quality.

### Naming Conventions

#### Variables and Functions
- **Style**: Snake case (lowercase with underscores)
- **Function Naming Pattern**: Use `qcperf_` prefix for core API functions, `qcperf_<backend>_` prefix for backend creation functions
- **Examples**:
  ```c
  int metric_count;
  uint64_t timestamp_value;
  
  // Core API functions
  enum QcPerfReturnCode qcperf_init(void);
  enum QcPerfReturnCode qcperf_connect_backend(enum QcPerfBackendId backend_id, QcPerfMessageCallback message_callback);
  
  // Backend creation functions
  enum QcPerfReturnCode qcperf_dummy_create(struct QcPerfBackendPrivate* backend);
  enum QcPerfReturnCode qcperf_wos_thermal_create(struct QcPerfBackendPrivate* backend);
  ```

#### Structures and Enums
- **Style**: Pascal case (capitalize first letter of each word, no underscores)
- **Prefix**: Use `QcPerf` prefix for all public types
- **Examples**:
  ```c
  struct QcPerfRequest {
      uint8_t capability_id;
      uint16_t streaming_rate;
      uint16_t sampling_rate;
  };
  
  enum QcPerfReturnCode {
      QC_PERF_RETURN_CODE_SUCCESS = 0,
      QC_PERF_RETURN_CODE_INVALID_HANDLE,
      QC_PERF_RETURN_CODE_INVALID_ARGUMENTS,
  };
  
  enum QcPerfBackendId {
      QC_PERF_BACKEND_DUMMY = 0,
      QC_PERF_BACKEND_POWER,
      QC_PERF_BACKEND_THERMAL,
      QC_PERF_BACKEND_MAX,
  };
  ```

#### Defines and Macros
- **Style**: Upper case with underscores
- **Backend Metric Naming Pattern**: Use `<BACKEND>_METRIC_<COMPONENT>_<TYPE>_ID` for metric IDs
- **Backend Capability Naming Pattern**: Use `<BACKEND>_CAPABILITY` for capability names
- **Examples**:
  ```c
  // General defines
  #define MAX_METRICS_PER_CAPABILITY 256
  #define QC_PERF_RETURN_CODE_SUCCESS 0
  
  // Backend-specific defines
  #define WOS_THERMAL_CAPABILITY "thermal"
  #define WOS_THERMAL_CAPABILITY_METRIC_COUNT 44
  
  // Metric defines
  #define WOS_THERMAL_METRIC_CPU_CLUSTER_0_TEMP_ID 0
  #define WOS_THERMAL_METRIC_CPU_CLUSTER_0_TEMP_NAME "Zone id : 0 CPU Cluster 0 Thermal Zone Temperature"
  #define WOS_THERMAL_METRIC_CPU_CLUSTER_0_TEMP_DESCRIPTION "Temperature of CPU Cluster 0 Thermal Zone"
  #define WOS_THERMAL_METRIC_CPU_CLUSTER_0_TEMP_UNIT "deg C"
  
  // Sampling and streaming rates
  #define WOS_THERMAL_STREAMING_RATES_LEN 2
  #define WOS_THERMAL_STREAMING_RATES 200, 1000
  ```

#### Header Guards
- **Style**: Upper case with underscores, matching the filename
- **Format**: `<FILENAME>_H`
- **Examples**:
  ```c
  #ifndef QC_PERF_H
  #define QC_PERF_H
  
  // Header content here
  
  #endif  // QC_PERF_H
  ```

### Type Definitions

#### Typedef Usage
- **Rule**: Use `typedef` **ONLY** for function pointer declarations
- **Do NOT** use `typedef` for structures or enums
- **Rationale**: Explicit `struct` and `enum` keywords improve code clarity

**Correct:**
```c
/* Typedef for message callback function */
typedef enum QcPerfReturnCode (*QcPerfMessageCallback)(struct QcPerfMessage* message);

/* Typedef for result callback function */
typedef enum QcPerfReturnCode (*QcPerfDataCallback)(struct QcPerfData* data);
```

**Incorrect:**
```c
// DO NOT DO THIS
typedef struct QcPerfRequest QcPerfRequest;
typedef enum QcPerfReturnCode QcPerfReturnCode;
```

**Proper Usage:**
```c
// Always use explicit struct/enum keywords
struct QcPerfRequest request;
enum QcPerfReturnCode error;
```

#### Pointer Type Definitions
- **Rule**: Do not use typedefs for pointer types
- **Format**: Always use explicit struct pointers
- **Examples**:
  ```c
  // Instead of typedef struct MutexInfo* LpMutexInfo;
  // Use direct struct pointers:
  struct MutexInfo* mutex_info;
  struct List** pp_list;
  ```
- **Rationale**: Explicit struct pointers improve code clarity and maintainability

### File Organization

#### Include Order
1. Standard library headers (`<stdint.h>`, `<stdio.h>`, etc.)
2. Project headers (`"qcperf_common.h"`, `"qcperf_private.h"`, etc.)

**Example**:
```c
#include <stdint.h>
#include <stdio.h>

#include "qcperf_common.h"
```

#### End of File
- **Rule**: Always add a newline at the end of file (EOF)

#### Cross-Platform Implementation
- **Rule**: Organize platform-specific implementations in separate directories
- **Structure**:
  ```
  module/
  ├── inc/
  │   └── Module.h       # Common interface
  └── src/
      ├── windows/       # Windows implementation
      │   └── Module.c
      └── linux/         # Linux implementation
          └── Module.c
  ```
- **Conditional Compilation**: Use platform-specific macros for shared implementations
  ```c
  #ifdef _WIN32
  // Windows-specific code
  #else
  // Linux/POSIX code
  #endif
  ```

### Code Structure

#### Variable Declaration
- **Rule**: Declare all variables at the beginning of functions
- **Rule**: Initialize variables on the same line as declaration when possible
- **Rationale**: C89/C90 compatibility and improved readability

**Correct:**
```c
enum QcPerfReturnCode qcperf_init(void)
{
    enum QcPerfReturnCode error = QC_PERF_RETURN_CODE_SUCCESS;
    int result = 0;
    struct QcPerfBackendInfo* backend_info = NULL;
    
    // Function logic here
    
    // ... rest of function
    
    return error;
}
```

**Incorrect:**
```c
enum QcPerfReturnCode qcperf_init(void)
{
    // Some code
    
    int result = 0;  // DON'T declare variables in the middle
    
    // More code
    
    struct QcPerfBackendInfo* backend_info = NULL;  // DON'T do this
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}
```

#### Code Blocks
- **Rule**: Always enclose statements in code blocks, even for single-line statements
- **Rationale**: Prevents bugs when adding additional statements later

**Correct:**
```c
if (error != QC_PERF_RETURN_CODE_SUCCESS) {
    return error;
}

if (backend_info == NULL) {
    return QC_PERF_RETURN_CODE_NULL_POINTER;
}

for (int i = 0; i < count; i++) {
    process_metric(i);
}
```

**Incorrect:**
```c
if (error != QC_PERF_RETURN_CODE_SUCCESS)
    return error;  // DON'T omit braces

if (backend_info == NULL)
    return QC_PERF_RETURN_CODE_NULL_POINTER;  // DON'T omit braces
```

### Documentation

#### Doxygen Documentation
- **Rule**: Use Doxygen-style comments for all public APIs
- **Format**: Use `/** */` for Doxygen comments

**Example**:
```c
/**
 * @brief Initialize the qcperf library
 * 
 * This function must be called before any other qcperf functions.
 * It initializes internal data structures and prepares the library for use.
 * 
 * @return QC_PERF_RETURN_CODE_SUCCESS on success
 * @return QC_PERF_RETURN_CODE_ALREADY_INITIALIZED if already initialized
 * @return QC_PERF_RETURN_CODE_CALLOC_FAILED if memory allocation fails
 */
enum QcPerfReturnCode qcperf_init(void);
```

#### Inline Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line comments

**Example**:
```c
// Initialize backend connection
error = connect_to_backend(backend_id);

/* 
 * We need to validate the capability ID before proceeding
 * because invalid IDs can cause undefined behavior in the backend
 */
if (capability_id >= MAX_CAPABILITIES) {
    return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS;
}
```

#### File Headers
- **Rule**: Include a comprehensive file header with copyright notice, description, and author information
- **Format**:
  ```c
  /*
        Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
        Redistribution and use in source and binary forms, with or without
        modification, are permitted (subject to the limitations in the
        disclaimer below) provided that the following conditions are met:
                * Redistributions of source code must retain the above copyright
                  notice, this list of conditions and the following disclaimer.
                * Redistributions in binary form must reproduce the above
                  copyright notice, this list of conditions and the following
                  disclaimer in the documentation and/or other materials provided
                  with the distribution.
                * Neither the name of Qualcomm Technologies, Inc. nor the names of its
                  contributors may be used to endorse or promote products derived
                  from this software without specific prior written permission.
        NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
        GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
        HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
        WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
        MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
        IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
        ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
        DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
        GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
        INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
        IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
        OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
        IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  */

  /**
   * @file filename.c
   * @brief Brief description of the file
   * @author Author Name (email@address)
   *
   * Detailed description of the file's purpose and contents.
   * Include any important implementation details or notes.
   */
  ```

### Control Flow

#### Single Return (Single Exit Point)
- **Rule**: Functions should have a single return statement at the end
- **Rationale**: Easier debugging, cleanup, and resource management

**Correct:**
```c
enum QcPerfReturnCode qcperf_process_request(struct QcPerfRequest* request)
{
    enum QcPerfReturnCode error;
    int result;
    
    error = QC_PERF_RETURN_CODE_SUCCESS;
    result = 0;
    
    if (request == NULL) {
        error = QC_PERF_RETURN_CODE_NULL_POINTER;
    } else if (request->capability_id >= MAX_CAPABILITIES) {
        error = QC_PERF_RETURN_CODE_INVALID_ARGUMENTS;
    } else {
        result = process_capability(request->capability_id);
        if (result != 0) {
            error = QC_PERF_RETURN_CODE_FAILED;
        }
    }
    
    return error;  // Single exit point
}
```

**Incorrect:**
```c
enum QcPerfReturnCode qcperf_process_request(struct QcPerfRequest* request)
{
    if (request == NULL) {
        return QC_PERF_RETURN_CODE_NULL_POINTER;  // Multiple returns
    }
    
    if (request->capability_id >= MAX_CAPABILITIES) {
        return QC_PERF_RETURN_CODE_INVALID_ARGUMENTS;  // Multiple returns
    }
    
    int result = process_capability(request->capability_id);
    if (result != 0) {
        return QC_PERF_RETURN_CODE_FAILED;  // Multiple returns
    }
    
    return QC_PERF_RETURN_CODE_SUCCESS;
}
```

#### No goto Statements
- **Rule**: Do not use `goto` statements
- **Rationale**: Improves code readability and debugging
- **Alternative**: Use proper control structures (if/else, loops, functions)

#### Avoid Always-True Loops
- **Rule**: Avoid infinite loops like `while(1)` or `for(;;)`
- **Alternative**: Use explicit loop conditions

**Correct:**
```c
while (true == is_running && NULL != error_occurred) {
    process_data();
}
```

**Incorrect:**
```c
while (1) {  // DON'T use always-true conditions
    process_data();
    if (should_exit) {
        break;
    }
}
```

### Formatting

#### Indentation
- **Rule**: Use spaces for indentation, not tabs
- **Tab Size**: 4 spaces
- **Rationale**: Consistent appearance across all editors

**Example**:
```c
enum QcPerfReturnCode qcperf_init(void)
{
    enum QcPerfReturnCode error;
    
    error = QC_PERF_RETURN_CODE_SUCCESS;
    
    if (is_initialized) {
        error = QC_PERF_RETURN_CODE_ALREADY_INITIALIZED;
    } else {
        error = initialize_backends();
        if (error == QC_PERF_RETURN_CODE_SUCCESS) {
            is_initialized = true;
        }
    }
    
    return error;
}
```

#### Spacing
- Add space after keywords: `if (`, `while (`, `for (`
- Add space around operators: `a = b + c`, `x == y`
- No space before semicolons
- Add blank lines to separate logical sections

#### Code Formatting Before Commit
- **Rule**: Always format code before committing
- Use a consistent code formatter, .clang-format from repository
- Ensure formatting matches project style

### Error Handling

#### Return Error Codes
- All public API functions should return `enum QcPerfReturnCode`
- Use appropriate error codes from the `QcPerfReturnCode` enum

**Example**:
```c
enum QcPerfReturnCode qcperf_validate_input(struct QcPerfRequest* request)
{
    enum QcPerfReturnCode error;
    
    error = QC_PERF_RETURN_CODE_SUCCESS;
    
    if (request == NULL) {
        error = QC_PERF_RETURN_CODE_NULL_POINTER;
    } else if (request->capability_id >= MAX_CAPABILITIES) {
        error = QC_PERF_RETURN_CODE_INVALID_ARGUMENTS;
    }
    
    return error;
}
```

#### Null Pointer Checks
- Always validate pointer arguments
- Return `QC_PERF_RETURN_CODE_NULL_POINTER` for null pointers

#### Assertions for Debug Builds
- **Rule**: Add assertions for debug builds to catch programming errors early
- Use `assert()` from `<assert.h>`

**Example**:
```c
#include <assert.h>

enum QcPerfReturnCode qcperf_process(struct QcPerfRequest* request)
{
    enum QcPerfReturnCode error;
    
    assert(NULL != request);  // Debug-only check
    assert(is_initialized);   // Debug-only check
    
    error = QC_PERF_RETURN_CODE_SUCCESS;
    
    // Production null check
    if (NULL == request) {
        error = QC_PERF_RETURN_CODE_NULL_POINTER;
    } else {
        // Process request
    }
    
    return error;
}
```

### Best Practices

#### Pointer Comparisons
- **Rule**: Prefer `NULL == ptr` over `!ptr`
- **Rationale**: More explicit and prevents accidental assignment

**Correct:**
```c
if (NULL == backend_info) {
    return QC_PERF_RETURN_CODE_NULL_POINTER;
}

if (NULL != callback) {
    callback("Processing complete");
}
```

**Incorrect:**
```c
if (!backend_info) {  // Less explicit
    return QC_PERF_RETURN_CODE_NULL_POINTER;
}

if (callback) {  // Less explicit
    callback("Processing complete");
}
```

#### Const Correctness
- Use `const` for pointer parameters that won't be modified
- Use `const` for variables that shouldn't change

**Example**:
```c
typedef enum QcPerfReturnCode (*QcPerfMessageCallback)(struct QcPerfMessage* message);

enum QcPerfReturnCode qcperf_validate_name(const char* name, size_t length);
```

#### Memory Management
- Always check return values of memory allocation functions
- Return `QC_PERF_RETURN_CODE_CALLOC_FAILED` on allocation failure
- Free allocated memory in reverse order of allocation
- Set pointers to NULL after freeing
- Always use `calloc()` instead of `malloc()` to ensure zero-initialization
- Clean up all allocated resources in error paths before returning

**Example**:
```c
enum QcPerfReturnCode qcperf_allocate_buffer(void)
{
    enum QcPerfReturnCode error;
    struct QcPerfBuffer* buffer;
    
    error = QC_PERF_RETURN_CODE_SUCCESS;
    buffer = NULL;
    
    buffer = (struct QcPerfBuffer*)calloc(1, sizeof(struct QcPerfBuffer));
    if (NULL == buffer) {
        error = QC_PERF_RETURN_CODE_CALLOC_FAILED;
    } else {
        // Initialize buffer
    }
    
    return error;
}
```

#### Magic Numbers
- Avoid magic numbers in code
- Use named constants or enums

**Correct:**
```c
#define MAX_RETRY_COUNT 3
#define DEFAULT_TIMEOUT_MS 1000

if (retry_count >= MAX_RETRY_COUNT) {
    return QC_PERF_RETURN_CODE_FAILED;
}
```

**Incorrect:**
```c
if (retry_count >= 3) {  // What does 3 mean?
    return QC_PERF_RETURN_CODE_FAILED;
}
```

#### Function Length
- Keep functions focused and concise
- If a function exceeds ~50 lines, consider breaking it into smaller functions
- Each function should do one thing well

#### Header File Organization
- Public API headers should only contain necessary declarations
- Keep implementation details in private headers or source files
- Use forward declarations when possible to reduce dependencies

#### Backend Header Organization
- **Rule**: Separate backend headers into distinct files for better organization
- **Main Backend Header** (`<backend_name>.h`): Contains backend creation function and minimal interface
- **Metrics Header** (`<backend_name>_metrics.h`): Contains all metric definitions, sampling rates, and streaming rates

#### Resource Cleanup
- **Rule**: Always clean up resources in the reverse order of acquisition
- **Rule**: Set pointers to NULL after freeing to prevent use-after-free bugs
- **Example**:
  ```c
  if (error != SUCCESS) {
      if (buffer != NULL) {
          free(buffer);
          buffer = NULL;
      }
      
      if (handle != NULL) {
          close_handle(handle);
          handle = NULL;
      }
  }
  ```

#### Thread Safety
- **Rule**: Document thread safety guarantees for each module
- **Rule**: Use appropriate synchronization primitives (mutexes, condition variables)
- **Rule**: Minimize critical sections to reduce contention
- **Rule**: Always check return codes from synchronization operations
- **Rule**: Protect shared data structures with appropriate locks
- **Example**:
  ```c
  enum QMutexReturnCode lock_result = mutex_lock(list->mutex_info);
  if (RETURN_CODE_MUTEX_LOCK_SUCCESS == lock_result) {
      // Critical section - keep as short as possible
      list->size++;
      mutex_unlock(list->mutex_info);
  } else {
      // Handle lock failure
      return RETURN_CODE_LIST_MUTEX_LOCK_FAILED;
  }
  ```

### Summary Checklist

Before committing code, verify:

- [ ] Naming conventions followed (snake_case, PascalCase, UPPER_CASE)
- [ ] Typedef used only for function pointers
- [ ] Header guards present and correctly formatted
- [ ] Variables declared at beginning of functions
- [ ] Doxygen documentation for public APIs
- [ ] Single return statement per function
- [ ] No goto statements
- [ ] EOF newline present
- [ ] Indentation uses 4 spaces (no tabs)
- [ ] No always-true loops
- [ ] Code formatted before commit
- [ ] Assertions added for debug builds
- [ ] All statements enclosed in code blocks
- [ ] `NULL == ptr` used instead of `!ptr`
- [ ] Copyright header present
- [ ] Error handling implemented correctly
- [ ] Resources properly cleaned up
- [ ] Thread safety considerations documented
- [ ] Platform-specific code properly organized
- [ ] Return codes checked for all function calls
- [ ] Synchronization primitives properly used
- [ ] Memory allocated with calloc() for zero-initialization
- [ ] File naming follows project conventions
- [ ] Function naming follows module_action pattern
- [ ] Return code enums follow consistent naming pattern
