# SimpleShell Implementation

## Summary
The SimpleShell program is designed to function as a basic command-line interpreter, allowing users to execute commands, manage background processes, and view command history. It supports features such as executing commands from a file, displaying command execution details, and handling interruptions gracefully.

## Features and Implementation Details

- **Command Execution**: Users can enter commands directly into the shell prompt, which are then executed using various Unix `exec` functions (`execlp`, `execvp`, etc.).

- **Background Processes**: The shell supports executing commands in the background by appending `&` at the end of the command.

- **Command History**: Implemented using a linked list, the shell keeps track of executed commands along with details like process ID, start time, and execution duration.

- **Termination Handling**: When terminated using `Ctrl-C`, the shell displays a summary of all executed commands during its session, including detailed information like process ID, start time, and total execution time.

## Supported Commands
The SimpleShell supports executing various commands including:
- `ls`
- `echo`
- `wc`
- `grep`
- Custom executables like `fib` and `helloworld`
- File manipulation commands like `sort`, `uniq`, and `cat`
- and many more...

Commands not directly implemented (e.g., `ls` functionalities) rely on existing Unix executables available in `/usr/bin`.

## Restrictions and Design Choices
- **Command Parsing**: Commands are parsed based on spaces; backslashes or quotes in commands are not supported to simplify implementation.

- **Error Handling**: Errors during command execution (e.g., incorrect command) are displayed to the user.

## Bonus Features
- **Background Processing**: Users can append `&` to commands to execute them in the background.

- **Script Execution**: The shell can read commands from a script file, executing them sequentially.

## Usage Instructions
To run the SimpleShell:
- Compile the `simple-shell.c` program.
- Execute with a script file as an argument (optional). If no file is provided, the shell operates interactively.

Example:
```bash
gcc -o simple-shell simple-shell.c
./simple-shell script_file.txt
