/**************************************************************
* Class::  CSC-415-04 Spring 2024
* Name:: Haley Park
* Student ID:: 923812276
* GitHub-Name:: jung-hyeon
* Project:: Assignment 3 – Simple Shell with Pipes
*
* File:: Park_Haley_HW3_main.c
*
* Description::
* This assignment is to implement a simple shell that runs on 
* top of the regular command-line interpreter for Linux. 
* The simple shell should read lines of user input, 
* then parse and execute the commands by forking/creating new processes. 
* If the simple shell encounters an error while reading a line of input 
* it should report the error and exit. 
* Also, the simple shell must support piping. 
* The pipe character | seperates different commands and 
* the output (stdout) of the program on the left becomes 
* the input (stdin) for the program on the right. 

**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 177
#define READEND 0
#define WRITEEND 1

int main(int argc, char* argv[]) {
    // Default prompt
    char *prompt = "> ";

    // Check if prefix prompt is provided as command line argument
    if (argc > 1) {
        prompt = argv[1];
    }

    // Declare arrays to store user input and parsed commands
    char line[BUFFER_SIZE];     // Array to store user input
    char* files[BUFFER_SIZE];   // Array to store individual commands separated by pipes
    char* arguments[BUFFER_SIZE][BUFFER_SIZE];  // 2D array to store arguments for each command
    int count_file; // Variable to store the number of commands parsed from user input

    while (1) {
        // Print shell prompt
        printf("%s", prompt);
        fflush(stdout);

        // Read user input
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // Check for End of File
            if (feof(stdin)) {
                printf("End of input.\n");
                break;
            } 
            else {
                printf("Error: fgets\n");
                exit(1);
            }
        }
        
        // Check for empty line
        if (strcmp(line, "\n") == 0) {
            printf("Error: Empty command. Please enter a command.\n");
            continue;
        }
        
        // Check for exit command
        if (strcmp(line, "exit\n") == 0) {
            printf("Exiting shell...\n");
            break;
        }

        // Parse input
        char* file_token;
        int file_index = 0;

        file_token = strtok(line, "|");
        // Check for empty line
        if (file_token == NULL) {
            printf("Error: Empty command. Please enter a command.\n");
            continue;
        }

        // Split commands by pipe and store them in an array
        files[file_index] = file_token;
        for (file_index = 1; file_index < BUFFER_SIZE; file_index++){
            file_token = strtok(NULL, "|"); 
            if (file_token == NULL){
                break;
            }
            files[file_index] = file_token;
        }
        count_file = file_index;

        // Parse arguments for each command
        int count = 0;
        for (count = 0; count < file_index; count++){
            char* arg_token;
            int arg_index = 0;

            arg_token = strtok(files[count], " \t\n");
            // Check for empty line
            if (arg_token == NULL) {
                printf("Error: Empty command. Please enter a command.\n");
                continue;
            }
            arguments[count][arg_index] = arg_token;
            for (arg_index = 1; arg_index < BUFFER_SIZE - 1; arg_index++){
                arg_token = strtok(NULL, " \t\n"); 
                if (arg_token == NULL){
                    break;
                }
                arguments[count][arg_index] = arg_token;
            }
            // Null-terminate the argument list
            arguments[count][arg_index] = NULL;
        }
/*
        //print arguments
        printf("\n****** start ******");
        for (int f_idx = 0; f_idx < file_index; f_idx++){
            if (arguments[f_idx] == NULL) break;
            printf("\n------ inputs --------\n");
            int count =0;
            while (count < BUFFER_SIZE){
                if (arguments[f_idx][count] == NULL) break;
                printf("%d : %s\n", count, arguments[f_idx][count]);
                count++;
            }
            printf("----------------------\n");
        }
*/
        // Create pipes
        // Array to store file descriptors for pipes
        int pipes[count_file - 1][2];
        for (int i = 0; i < count_file - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        // Fork processes
        for (int i = 0; i < count_file; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } 
            else if (pid == 0) {
                // Child process

                // Redirect input and output for appropriate commands
                if (i == 0) {
                    // Redirect output for the first command
                    close(pipes[i][READEND]);
                    dup2(pipes[i][WRITEEND], STDOUT_FILENO);
                    close(pipes[i][WRITEEND]);

                }
                else if (i < count_file - 1){  
                    // Redirect input and output for intermediate commands
                    close(pipes[i-1][WRITEEND]);
                    close(pipes[i][READEND]);

                    dup2(pipes[i - 1][READEND], STDIN_FILENO);
                    dup2(pipes[i][WRITEEND], STDOUT_FILENO);

                    close(pipes[i-1][READEND]);
                    close(pipes[i][WRITEEND]);
                }
                else if (i == count_file - 1) {
                    // Redirect output for the last command
                    close(pipes[i - 1][WRITEEND]);
                    dup2(pipes[i - 1][READEND], STDIN_FILENO);
                    close(pipes[i - 1][READEND]);
                }

                // Execute command
                execvp(arguments[i][0], arguments[i]);
                // If execvp returns, it must have failed
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }

        // Close all pipes in parent process and wait for child processes to finish
        for (int i = 0; i < count_file; i++) {
            close(pipes[i][READEND]);
            close(pipes[i][WRITEEND]);
            int status;
            wait(&status);
            printf("Child %d, exited with %d\n", i, WEXITSTATUS(status));
        }
    }
    
    return 0;
}
