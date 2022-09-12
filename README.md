# Shell Scripting in C

This code implements a shell program that repeatedly reads commands from the user, interprets them, and translates them into a sequence of actions that involve an interaction with the Operating System through system calls. 

## Instructions
Download all the files into a directory called “project.” 

Compile the shell.c file 

    $ make

Run the shell program 

    $ ./shell

The shell prints a ‘$’ prompt when it is ready to receive a new command from the user. It prints out the path of the current working directory along with the prompt. 

When a command is launched in the background using the suffix ‘&’, the shell prints the pid of the process corresponding to the last sub-command. 

    $ ls -l | wc & 
    [10969]
    $ 
Sub commands are separated by pipes. The shell forks multiple child processes using multiple pipes opening and closing file descriptors. 

When a proper sub-command is not found, error message is displayed. 

    $ hello 
    hello: Command not found
    $

The input redirection operator '<' redirects the standard input of the first sub-command of a command. If the input file is not found, error message is displayed. 
    
    $ wc < test.txt 
    223 551 5288 
    $ wc < invalid_file 
    invalid_file: File not found
    
The output redirection operator '>' redirects the standard output of the last sub-command of a command. 'cat' displays the content of the file. 
    
    $ ls -l > hello.txt
    $ cat hello.txt
    -rw-r-----   1 aj  staff      0 May  9 13:36 hello.txt
    -rw-r--r--@  1 aj  staff    215 May  9 15:31 makefile
    -rwxr-xr-x   1 aj  staff  51640 May  9 18:39 shell
    -rw-r--r--@  1 aj  staff   6290 May  9 23:34 shell.c
    drwxr-xr-x@ 10 aj  staff    320 May  9 18:42 test
    -rw-r--r--   1 aj  staff      1 May  9 13:00 test.txt

'ls' displays all the contents of the directory 
    
    $ ls

'mkdir' to create a new directory. 

    $ mkdir test

'cd' to change directories. 'cd ..' to move back to the previous directory. 
    
    $ cd test
    $ cd .. 
    
The shell prints a message whenever a background process finishes execution. Press Enter. The message should print, in brackets, the pid of the process that finished.
    
    $ sleep 2 & 
    [10969] 
    $ 
    [10969] finished 
    $ 
    
