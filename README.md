# Custom Unix Shell
This project is a custom implementation of a Unix-like shell, providing a command-line interface (CLI) with basic built-in commands, support for executing external programs, and directory navigation. The shell features a Read-Eval-Print Loop (REPL) that continuously reads user input, processes commands, and prints the results.

## Features
- REPL (Read-Eval-Print Loop): The shell operates in an interactive loop, reading commands from the user, evaluating them, and displaying the output.

- Built-in Commands:

   - exit: Terminates the shell session.
   - echo: Outputs the provided arguments to the terminal.
   - type: Displays information about how a command would be interpreted, indicating whether it's a built-in command or an external program.
   - pwd: Prints the current working directory.
   - cd: Changes the current directory, supporting absolute paths, relative paths, and the ~ character for the home directory.
   - External Command Execution: The shell can execute external programs by searching for the executable in the directories listed in the PATH environment variable.

- Command Parsing: The shell parses user input into commands and arguments, handling multiple command types and executing them appropriately.

- Error Handling: The shell includes basic error handling, such as reporting when a command is not found or when a directory change fails.

## How It Works
1. Initialization:

   - The shell begins by reading the PATH environment variable and storing the directories where executable files can be found.
   - It also initializes a set of built-in commands (exit, echo, type, pwd, cd) that the shell can handle internally.

2. Command Handling:

   - User input is read in a loop, and each command is parsed into its components.
   - Depending on the command, the shell either executes a built-in function or searches for an external executable in the specified PATH directories.

3. Built-in Command Functions:

   - on_echo: Implements the echo command, printing the arguments passed to it.
   - on_type: Implements the type command, checking if a command is a built-in or an external program, and  displaying the appropriate message.
   - on_pwd: Implements the pwd command, displaying the current working directory.
   - on_cd: Implements the cd command, handling directory changes with support for absolute and relative paths, as well as the home directory.

4. External Program Execution:

   - on_exec: Handles execution of external programs by searching for the command in the PATH directories and invoking it with the provided arguments.

5. Process Management:

   - The shell uses fork() to create a new process for executing external commands and execv() to replace the child process with the new program. The parent process waits for the child to finish before continuing.
## Usage

To run the shell, compile and execute the program in a Unix-like environment. The shell will present a prompt ($) where you can enter commands. Use exit to terminate the shell session.

```shell
$ ./my_shell
$ echo Hello, World!
Hello, World!
$ pwd
/home/user
$ cd /usr/local/bin
$ type echo
echo is a shell builtin
$ exit
```

## Conclusion
This custom Unix shell project demonstrates fundamental concepts of command-line processing, built-in command implementation, external program execution, and process management in a Unix-like environment.

