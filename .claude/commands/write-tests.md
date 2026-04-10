# Write Tests for libqcperf

You are writing Unity-based C unit tests for the **libqcperf** (Qualcomm Performance Profiler Library).

Before writing any tests, read the relevant source and header files to confirm actual API behaviour. Use the project structure below to locate them.

## Project structure

```
qcperf/
  core/inc/
    qcperf.h            ← core API
    qcperf_common.h     ← return codes, structs, typedefs
  backends/inc/
    qcperf_backend_enum.h   ← QcPerfBackendId enum
    qcperf_backends.h       ← backend init registry
  backends/dummy/inc/
    dummy.h
  third-party/Unity/src/
    unity.h             ← test framework
  test_app/
    src/                ← test source files go here
    CMakeLists.txt      ← register new test executables here
```

## Backends

```c
enum QcPerfBackendId {
    QC_PERF_BACKEND_DUMMY   = 0,  // safe on any machine
    QC_PERF_BACKEND_POWER   = 1,  // requires WoS ARM64 hardware
    QC_PERF_BACKEND_THERMAL = 2,  // requires WoS ARM64 hardware
    QC_PERF_BACKEND_MAX,
};
```

## Core API

```c
enum QcPerfReturnCode qcperf_init(void);
enum QcPerfReturnCode qcperf_deinit(void);
enum QcPerfReturnCode qcperf_version(struct QcPerfVersionInfo *version_info);
enum QcPerfReturnCode qcperf_connect_backend(enum QcPerfBackendId id, QcPerfMessageCallback cb);
enum QcPerfReturnCode qcperf_disconnect_backend(enum QcPerfBackendId id);
enum QcPerfReturnCode qcperf_get_capabilities_info(enum QcPerfBackendId id, struct QcPerfBackendInfo *info);
enum QcPerfReturnCode qcperf_set_data_callback(enum QcPerfBackendId id, QcPerfDataCallback cb);
enum QcPerfReturnCode qcperf_start(enum QcPerfBackendId id, struct QcPerfRequest *req);
enum QcPerfReturnCode qcperf_stop(enum QcPerfBackendId id, struct QcPerfRequest *req);
enum QcPerfReturnCode qcperf_get_error_info(enum QcPerfReturnCode code, struct QcPerfReturnCodeInfo *info);
```

## Unity patterns

**Test function:**
```c
void test_<api>_<scenario>(void)
{
    enum QcPerfReturnCode ret = ...;
    TEST_ASSERT_EQUAL(QC_PERF_RETURN_CODE_SUCCESS, ret);
}
```

**Test runner (`main`):**
```c
void setUp(void) { }
void tearDown(void) { }

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_foo);
    return UNITY_END();
}
```

**Common assertions:**
```c
TEST_ASSERT_EQUAL(expected, actual)
TEST_ASSERT_NOT_EQUAL(not_expected, actual)
TEST_ASSERT_NULL(ptr)
TEST_ASSERT_NOT_NULL(ptr)
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_GREATER_THAN(threshold, actual)
TEST_ASSERT_EQUAL_STRING(expected, actual)
```

## CMakeLists.txt pattern for a new test executable

```cmake
add_executable(test_<name>
    src/test_runner_<name>.c
    src/test_<name>.c
)
target_include_directories(test_<name> PRIVATE
    ${CMAKE_SOURCE_DIR}/third-party/Unity/src
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/backends/dummy/inc
)
target_link_libraries(test_<name> PRIVATE unity QcPerfCore)
add_test(NAME <name>_tests COMMAND test_<name>)
```

## Existing tests (do not duplicate)

| File | What is already tested |
|------|------------------------|
| `test_qcperf_unit.c` | init, deinit, connect dummy, disconnect dummy, get_capabilities_info dummy |
| `test_backends.c` | dummy init, dummy info (backend_id + capabilities_list_length), dummy deinit |

## Rules

1. Each test calls `qcperf_init()` at the start and `qcperf_deinit()` at the end — the library does not persist state across cycles.
2. One logical condition per test function.
3. Name tests `test_<api_function>_<scenario>`, e.g. `test_connect_backend_invalid_id_returns_error`.
4. Cover both happy paths and error paths (null pointers, bad IDs, wrong call order, double init, etc.).
5. Use `QC_PERF_BACKEND_DUMMY` unless the user explicitly asks for hardware-specific tests.
6. New source files go in `qcperf/test_app/src/`. Update `CMakeLists.txt` if adding a new executable.

---

User request: **$ARGUMENTS**
