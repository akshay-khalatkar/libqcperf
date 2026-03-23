# QcPerf Utilities CMake configuration
# This file includes all utility libraries used by the QcPerf framework

# Thread utilities for cross-platform thread management
add_subdirectory(qthread)

# Sleep utilities for cross-platform sleep operations
add_subdirectory(qsleep)

# Condition variable utilities for cross-platform condition variable management
add_subdirectory(qcondition-variable)

# Mutex utilities for cross-platform mutex management
add_subdirectory(qmutex)

# List utilities for doubly linked list implementation
add_subdirectory(qlist)

# Time utilities for high-precision timing with nanosecond resolution
add_subdirectory(qtime)
