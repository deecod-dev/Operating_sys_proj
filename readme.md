# Project Suite

This repository contains a suite of projects, each focusing on a different aspect of systems programming, parallel processing, and shell implementation. Below is an overview of each project:

## Projects Overview

1. [CPU_SCHEDULER](#cpu_scheduler-proj1)
2. [ELF Loader](#elf-loader-proj2)
3. [SimpleMultithreader](#simplemultithreader-proj3)
4. [SimpleShell Implementation](#simpleshell-implementation-proj4)
5. [SimpleSmartLoader](#simplesmartloader-proj5)

## CPU_SCHEDULER (proj1)

### SimpleScheduler

#### Overview

SimpleScheduler is a process scheduler implemented in C. It simulates an operating system scheduler that schedules processes across a limited number of CPUs using a round-robin scheduling policy. This scheduler supports priority scheduling and runs processes for a specified time slice.

#### Features

- Round-robin scheduling
- Priority-based scheduling
- Supports multiple CPUs
- Time quantum control
- Enqueues and dequeues processes
- Pauses and resumes processes
- Displays process queue

#### Requirements

- Linux operating system
- GCC compiler

#### Getting Started

1. **Clone the repository:**

    ```bash
    git clone <repository_url>
    cd SimpleScheduler
    ```

2. **Compile the source code:**

    ```bash
    make
    ```

3. **Launch the SimpleShell:**

    ```bash
    ./simpleshell
    ```

4. **Submit jobs and start the scheduler:**

    ```bash
    SimpleShell$ submit ./a.out
    SimpleShell$ startsch
    ```

#### Priority Scheduling

- Submit a job with a priority (range 1-10):

    ```bash
    SimpleShell$ submit ./a.out -p 2
    ```

#### Design Document

The design document is located in the `docs` directory, detailing implementation and contributions.

#### License

This project is licensed under the MIT License.

---

## ELF Loader (proj2)

### Overview

This project is an ELF (Executable and Linkable Format) loader implemented in C. The loader reads and parses ELF files, then loads the executable segments into memory and transfers control to the entry point specified in the ELF header.

### Features

- Parses ELF32 headers
- Loads and maps executable segments into memory
- Transfers control to the entry point of the ELF binary
- Prints detailed information about the ELF and program headers

### Requirements

- C compiler (GCC recommended)
- Standard C libraries
- Linux-based operating system

### Usage

1. **Clone the repository:**

    ```sh
    git clone https://github.com/yourusername/elf-loader.git
    cd elf-loader
    ```

2. **Compile the loader:**

    ```sh
    gcc -o elf_loader loader.c
    ```

3. **Run the loader with an ELF executable:**

    ```sh
    ./elf_loader <path_to_elf_executable>
    ```

#### Functions

- **print_elf32_ehdr(Elf32_Ehdr *ehdr):** Prints the ELF header information.
- **print_elf32_phdr(Elf32_Phdr *phdr):** Prints the program header information.

#### License

This project is licensed under the MIT License.

---

## SimpleMultithreader (proj3)

### Overview

SimpleMultithreader is a lightweight C++ header-only library for simple parallelism using Pthreads.

### Features

- **ThreadWrapper Class**: A wrapper around Pthreads, simplifying thread creation and management.
- **parallel_for Function**:
  - **Single-Dimensional Loop Parallelization**: Divides the loop range into segments for parallel execution using Pthreads.
  - **Two-Dimensional Loop Parallelization**: Parallelizes a nested loop structure with specified ranges for both dimensions using Pthreads.

### Requirements

- C++11 compatible compiler
- POSIX Threads (Pthreads) library

### Usage

1. **Include `simple_multithreader.h` in your project.**

2. **Example 1: Parallel for (1D loop)**

    ```cpp
    #include "simple_multithreader.h"

    void example_parallel_for_1d() {
        parallel_for(0, 100, [](int i) {
            // Your loop body code here
        }, 4); // 4 threads
    }
    ```

3. **Example 2: Parallel for (2D loop)**

    ```cpp
    #include "simple_multithreader.h"

    void example_parallel_for_2d() {
        parallel_for(0, 10, 0, 20, [](int i, int j) {
            // Your nested loop body code here
        }, 2); // 2 threads
    }
    ```

#### License

This project is licensed under the MIT License.

---

## SimpleShell Implementation (proj4)

### Summary

The SimpleShell program is a basic command-line interpreter that allows users to execute commands, manage background processes, and view command history.

### Features and Implementation Details

- **Command Execution**: Users can enter commands directly into the shell prompt.
- **Background Processes**: Execute commands in the background by appending `&` at the end.
- **Command History**: Tracks executed commands with details like process ID, start time, and execution duration.
- **Termination Handling**: Displays a summary of all executed commands when terminated using `Ctrl-C`.

### Supported Commands

The SimpleShell supports various commands including `ls`, `echo`, `wc`, `grep`, and custom executables.

### Usage Instructions

1. **Compile the `simple-shell.c` program.**
2. **Run the shell interactively or with a script file:**

    ```bash
    gcc -o simple-shell simple-shell.c
    ./simple-shell script_file.txt
    ```

#### License

This project is licensed under the MIT License.

---

## SimpleSmartLoader (proj5)

### Overview

SimpleSmartLoader implements a loader for ELF 32-bit executables, loading segments lazily similar to how Linux handles memory allocation.

### Features

- **Segment Loading on Demand**: Loads segments only when needed.
- **Handling Segmentation Faults**: Treats segmentation faults as page faults and allocates memory dynamically.
- **Statistics**: Reports page faults, page allocations, and internal fragmentation after execution.

### Usage

1. **Compile the loader using the provided Makefile.**
2. **Execute the loader with the ELF executable as an argument:**

    ```bash
    ./loader <ELF Executable>
    ```

### Bonus Feature

- **Page-by-Page Allocation**: Allows multiple-step memory allocation for segments.

#### License

This project is licensed under the MIT License.

---

This combined README provides an overview and instructions for each project. For detailed information, please refer to the individual project directories.
