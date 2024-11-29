# Project Preview #
This project was developed as part of the System Programming K24 course in the Department of Informatics and Telecommunications at the University of Athens. It received a perfect score of 100/100.

# Compile and Execute #
To compile the program run the command: make.
To execute the program, run the command: ./main

# Details #
- The symbols > , >>, <, |, & must have a space before and after them to be interpreted correctly. Note that the question mark at the end of the command e.g. ./count1 &; is ignored.
- According to the parser I have implemented, the " are ignored. 
That is, createalias lll "ls -las" means lll = ls -las not lll = "ls -las".
- To run a program, write ./name not name in the terminal.
E.g. ./count1 &? ./count2 &; instead of count1&? count2&?
- Commands like myHistory i are 0-based.
- The  command cannot be an intermediate command in piping, redirection or Background.

# Structure of the work #
The projects consists of:
- main.c containing the main program execution function and the mysh_loop() of the implemented shell.
- the modules folder containing the implementors of most of the functions.
- the include folder containing the definitions of the functions implemented in the modules folder.

# main() #
- main.c: Contains the main program execution function and the mysh_loop() function of the implemented shell.
  
# mysh_loop() #
The function mysh_loop() iteratively reads the user's commands, interprets them, and either executes them or creates a copy of the current process and assigns the command to the main_execution() function for execution. This iteration stops when the user issues the exit command.
The commands executed are
- exit, to exit from the process running the shell.
- createalias/destroyalias, to update the vector of the process running the shell. 
- Finding alias in the run command. Note that the destroyalias command is executed first and then the alias replacement in the command.
- myHistory i, here the i-th command is located, so that it can then be added to the history.
- cd path & cd .. & cd wildcard
- Update the history, so that the vector of the process running the shell is updated.

Parser:
The program receives a command from the user with getline(). 
Then, it splits the line into commands using strtok() and delimiter to ?
It then splits each command into tokens, using stringstream, and executes it. Substrings enclosed in double quotes (") are interpreted as single tokens, ignoring the quotes.

Signals: 
In the host process in the shell, SIG_IGN is applied to the control-C and control-Z signals, while when forking, SIG_DFL is applied to the child process in which the instructions are executed so that the signals can be applied to them.

# main_execution() #
This function checks if the command given 
- either needs to be executed in the backround, or contains a redirect, or contains a pipe, or contains a string of wild characters which it then forwards to the appropriate handler functions, 
- or checks if it's myHistory, which in this case executes it, 
- or checks if it is an aliasing command, in which case it ignores it.
In all such cases this function returns 0.
In the case where a simple execute command is given as an argument, the syntax returns 1, so that the calling function can choose whether to execute it with fork (execute function), or without (execute_without_fork function).

This function is not only used by mysh_loop(), but also by the execute_bg(), redirection_output(), redirection_output_double(), redirection_input(), handle_pipes() functions to handle the subcommands contained in the commands they received as arguments.

# Redirection #
- Redirection of entry  
It is implemented with the redirection_input function, which 
    - opens the requested file for reading only
    - stores STDIN_FILENO to restore it later
    - redirects the input to the requested file
    - performs the actions to be performed based on the part of the command that does not contain redirection. In the case of a simple command, execution with fork is chosen, because no fork is performed elsewhere in the context of that command
    - restores STDIN_FILENO to the input
    - closes the file
    
- Redirection of exit 
It is implemented with the redirection_output function, which 
    - opens the requested file for writing, without append
    - saves STDOUT_FILENO to restore it later
    - redirects the output to the requested file
    - performs the actions to be performed based on the part of the command that does not contain redirection. In the case of a simple command, execution with fork is chosen, because no fork is performed elsewhere within that command
    - restores STDOUT_FILENO to the output
    - closes the file

- Redirect the output of an addition to an existing file
It is implemented with the redirection_output_double function, which 
    - opens the requested file for writing, with append
    - saves STDOUT_FILENO to be restored later
    - redirects the output to the requested file
    - performs the actions to be performed based on the part of the command that does not contain redirection. In the case of a simple command, execution with fork is chosen, because no fork is performed elsewhere within that command
    - restores STDOUT_FILENO to the output
    - closes the file

# Pipes #
It is implemented with the handle_pipes function, which: 
- splits commands at the first | into "left" and "right" subcommands.
- starts the implementation of the pipe with the commands int fd[2]; pipe(fd);;
- then executes a fork 
- in the child process , retrieve the input of the pipe, close the ends of the pipe that are not needed and execute the "left" instruction
- in the parent process, it performs another fork 
- in the new child process, the output of the pipe is rolled back, the unneeded ends of the pipe are closed and the 'right' command is executed
- finally, the parent process closes the ends of the pipe and waits for the child processes to complete.
In the case of a simple instruction, execution without fork is chosen because it is forked earlier.

# Background #
It is implemented with the execute_bg() function, which receives the command from main_execution() without the &. It is quite similar to the simple fork execution function, but has the following differences: 
- the parent process does not wait for the child process to complete
- a new session and process group is created for the child process.
- it does not only execute simple commands (main_execution)

# Wild Characters #
Implemented with the functions
- handle_wildcards(), which finds how many files match the given pattern and the function, which uses the glob.h library
- exec_wildcards(), which for each result of handle_wildcards(), executes the command to be executed. In addition, it handles the special case of the cd wildcard command, because it requires the search for matches to be done in the folders of the current folder rather than in the files. If exactly one match is found, then the cd match command is executed, otherwise an error message is printed.

# Aliases #
To store the aliases I use a map, which I update in the host process in the shell. In addition, I choose to make the necessary substitutions in the commands given by the user to the hosting process in the shell.

# Signal Handling #
In the host process in the shell, SIG_IGN is applied to the control-C and control-Z signals, while when forking, SIG_DFL is applied to the child process in which the instructions are executed so that the signals can be applied to them.

# myHistory #
- To store the history I use a vector, which I update in the host process in the shell. 
- The myHistory command is implemented by the child process. When we execute this command, I do not add any command to the history.
- The interpretation of the myHistory i command, is done in the parent process, so that it can then be added to the history. Whereas, its execution is done at the point where this command should be executed based on the above. 

# execute & execute_without_fork #
To execute simple system commands (excluding , exit e.g. ls), either the execute() function can be used to execute on a native process, or the execute_without_fork() function can be used to execute on the same process.
