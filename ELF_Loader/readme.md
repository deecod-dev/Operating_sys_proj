# ELF Loader

This project is an ELF (Executable and Linkable Format) loader implemented in C. The loader reads and parses ELF files, then loads the executable segments into memory and transfers control to the entry point specified in the ELF header. The primary use of this loader is to execute ELF binaries in a controlled environment.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Usage](#usage)
- [Functions](#functions)
- [Cleanup](#cleanup)
- [Contributing](#contributing)
- [License](#license)

## Introduction

An ELF file is a common standard file format for executables, object code, shared libraries, and core dumps. This ELF loader demonstrates how to manually load and execute such a file by:
1. Reading the ELF header.
2. Reading the program headers.
3. Mapping segments into memory.
4. Jumping to the entry point of the loaded program.

## Features

- Parses ELF32 headers.
- Loads and maps executable segments into memory.
- Transfers control to the entry point of the ELF binary.
- Prints detailed information about the ELF and program headers.

## Prerequisites

To compile and run this project, you need:
- A C compiler (GCC recommended).
- Standard C libraries.
- A Linux-based operating system (due to the use of `mmap` and ELF format specifics).

## Usage

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

   Example:
    ```sh
    ./elf_loader /path/to/your/elf/file
    ```

## Functions

- **print_elf32_ehdr(Elf32_Ehdr *ehdr):** Prints the ELF header information.
- **print_elf32_phdr(Elf32_Phdr *phdr):** Prints the program header information.
- **print_strings_in_form(char **strings, int num_strings):** Prints an array of strings.
- **loader_cleanup():** Cleans up allocated memory and closes file descriptors.
- **check_cond(Elf32_Phdr *phdr, Elf32_Ehdr *ehdr):** Checks if the entry point is within a loadable segment.
- **load_and_run_elf(char** exe):** Loads the ELF file, maps segments into memory, and jumps to the entry point.

## Cleanup

The `loader_cleanup()` function ensures that all resources are properly released. It closes the file descriptor and frees any allocated memory to prevent resource leaks.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request with your improvements.