# SimpleScheduler

## Overview

SimpleScheduler is a simple process scheduler implemented in C. It simulates an operating system scheduler that schedules processes across a limited number of CPUs using a round-robin scheduling policy. This scheduler supports priority scheduling and runs processes for a specified time slice.

## Features

- Round-robin scheduling
- Priority-based scheduling
- Supports multiple CPUs
- Time quantum control
- Enqueues and dequeues processes
- Pauses and resumes processes
- Displays process queue

## Requirements

- Linux operating system
- GCC compiler

## Getting Started

### Prerequisites

Ensure you have the following installed on your Linux system:

- GCC compiler
- Make utility

### Installation

1. Clone the repository:

    ```bash
    git clone <repository_url>
    cd SimpleScheduler
    ```

2. Compile the source code:

    ```bash
    make
    ```

### Running SimpleScheduler

1. Launch the SimpleShell:

    ```bash
    ./simpleshell
    ```

2. Enter the number of CPU cores and the time quantum (in milliseconds):

    ```text
    Enter the number of CPU's:
    4
    Enter the time quantum:
    1000
    ```

3. Submit jobs using the `submit` command:

    ```bash
    SimpleShell$ submit ./a.out
    ```

4. To start the scheduler, enter:

    ```bash
    SimpleShell$ startsch
    ```

5. To print the current queue, enter:

    ```bash
    SimpleShell$ pqueue
    ```

6. To exit the SimpleShell, use `Ctrl+C`.

### Example Usage

1. Start the shell:

    ```bash
    ./simpleshell
    ```

2. Enter the number of CPUs and time slice:

    ```text
    Enter the number of CPU's:
    2
    Enter the time quantum:
    1000
    ```

3. Submit a job:

    ```bash
    SimpleShell$ submit ./myprogram
    ```

4. Start the scheduler:

    ```bash
    SimpleShell$ startsch
    ```

5. Print the queue:

    ```bash
    SimpleShell$ pqueue
    ```

## Priority Scheduling

- Submit a job with a priority (range 1-10):

    ```bash
    SimpleShell$ submit ./a.out -p 2
    ```

- Default priority is 1 if not specified.

## Handling Interrupts

- Terminate the SimpleShell with `Ctrl+C`.
- The scheduler will terminate after all jobs are completed.

## Design Document

The design document can be found in the `docs` directory. It contains detailed information on the implementation, contributions of each member, and the project repository link.

## Contribution

This project was developed by a group. Contributions are detailed in the design document.

## License

This project is licensed under the MIT License.

---

## Appendix: User Executable (Jobs)

To run any user executable with the SimpleScheduler:

1. Include `dummy_main.h` at the top of the user program:

    ```c
    #include "dummy_main.h"
    ```

2. Use the following structure in the user program:

    ```c
    int dummy_main(int argc, char **argv);
    int main(int argc, char **argv) {
        /* You can add any code here you want to support your SimpleScheduler implementation*/
        int ret = dummy_main(argc, argv);
        return ret;
    }
    #define main dummy_main
    ```
