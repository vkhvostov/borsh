#include "execution.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h> // For strerror
#include <errno.h>  // For errno

// Ensure t_command is defined, typically in a header included by execution.h
// For example, in borsh.h:
// typedef struct s_command {
//     char *cmd_name; // Path to the command or command name
//     char **argv;    // Argument vector, including command name as argv[0]
//     // ... other fields like redirections, etc.
// } t_command;

extern char **environ; // For execve

pid_t launch_process(t_command *command, int in_fd, int out_fd, int pipe_fds[2], bool is_last_command) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) { // Child process
        // Redirect input if necessary
        if (in_fd != STDIN_FILENO) {
            if (dup2(in_fd, STDIN_FILENO) == -1) {
                perror("dup2 in_fd failed");
                exit(EXIT_FAILURE);
            }
            close(in_fd); // Close original after duplication
        }

        // Redirect output if necessary
        // This handles output to a file or the initial setup for piping.
        if (out_fd != STDOUT_FILENO) {
            if (dup2(out_fd, STDOUT_FILENO) == -1) {
                perror("dup2 out_fd failed");
                exit(EXIT_FAILURE);
            }
            close(out_fd); // Close original after duplication
        }

        // If there's a pipe and this command is not the last one,
        // it should write to the pipe.
        // The out_fd might have been set to pipe_fds[1] by the caller,
        // or we explicitly handle it here.
        // The current logic assumes out_fd would be pre-set to pipe_fds[1] if needed.
        // Let's refine based on typical pipe usage:
        if (pipe_fds != NULL) {
            if (!is_last_command) { // This process writes to the pipe
                if (out_fd != pipe_fds[1]) { // If out_fd wasn't already the pipe's write end
                    if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
                        perror("dup2 pipe_fds[1] failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            // Child closes both ends of the pipe it's directly involved with
            // or that were passed to it.
            close(pipe_fds[0]); // Close read end in child
            close(pipe_fds[1]); // Close write end in child (it's now STDOUT_FILENO or closed)
        }


        // Execute the command
        if (command->cmd_name == NULL || command->argv == NULL) {
            fprintf(stderr, "Error: command or arguments are NULL.\n");
            exit(EXIT_FAILURE);
        }
        
        execve(command->cmd_name, command->argv, environ);
        
        // If execve returns, it's an error
        fprintf(stderr, "Execution failed for %s: %s\n", command->cmd_name, strerror(errno));
        exit(EXIT_FAILURE); // Use 127 or other conventions later if needed

    } else { // Parent process
        // Close fds that were duplicated for the child and are no longer needed by parent.

        if (in_fd != STDIN_FILENO) {
            close(in_fd); // Parent closes its copy of the read end of the previous pipe or input file
        }

        if (out_fd != STDOUT_FILENO) {
            // If out_fd was a file descriptor for redirection, parent should close it.
            // If out_fd was pipe_fds[1] for the current command, it's handled by pipe_fds logic below.
            // This can be tricky. Let's assume caller manages specific redirection FDs.
            // For now, if it's not STDOUT, we close it.
            // This might prematurely close the read end of a pipe if out_fd was set to that.
            // This part needs careful review in context of the main loop.
            // The prompt stated: "If out_fd was duplicated for the child and is not STDOUT_FILENO... close it"
            // This is generally true for redirection files.
            // For pipes, the write end (pipe_fds[1]) is closed, read end (pipe_fds[0]) is kept for next command.
            // Let's stick to the prompt's specific wording for now.
             close(out_fd);
        }

        if (pipe_fds != NULL) {
            // Parent must close the write end of the pipe.
            // The read end (pipe_fds[0]) will be used as in_fd for the next command,
            // or closed by the main loop if this is the last command.
            close(pipe_fds[1]);
        }
        
        // Waiting for the child (waitpid) is typically done in the main execution loop
        // after all processes in a pipeline are launched.
        // This function's role is just to launch.
        return pid;
    }
    return -1; // Should not be reached
}
