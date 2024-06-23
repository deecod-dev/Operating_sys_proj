# SimpleMultithreader

SimpleMultithreader is a lightweight C++ header-only library for simple parallelism using Pthreads.

## Features

- **ThreadWrapper Class**: A wrapper around Pthreads, simplifying thread creation and management.
- **parallel_for Function**:
  - **Single-Dimensional Loop Parallelization**: Divides the loop range into segments for parallel execution using Pthreads.
  - **Two-Dimensional Loop Parallelization**: Parallelizes a nested loop structure with specified ranges for both dimensions using Pthreads.

## Requirements

- C++11 compatible compiler.
- POSIX Threads (Pthreads) library.

## Usage

### Installation

No installation is required as SimpleMultithreader is header-only. Simply include `simple_multithreader.h` in your project.

### Example 1: Parallel for (1D loop)

```cpp
#include "simple_multithreader.h"

void example_parallel_for_1d() {
    parallel_for(0, 100, [](int i) {
        // Your loop body code here
        // This lambda function will be executed in parallel for i = 0 to 99
    }, 4); // 4 threads
}
