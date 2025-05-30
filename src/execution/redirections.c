#include "execution.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h> // For readline
#include <readline/history.h> // For rl_clear_history or other history control if used

// Assuming t_redirect and t_command are defined in a header included by execution.h
// e.g., in borsh.h:
/*
typedef enum e_redir_type {
    T_REDIR_IN,     // <
    T_REDIR_OUT,    // >
    T_REDIR_APPEND, // >>
    T_HEREDOC       // <<
} t_redir_type;

typedef struct s_redirect {
    char                *file;      // Filename or heredoc delimiter
    t_redir_type        type;
    struct s_redirect   *next;
} t_redirect;

typedef struct s_command {
    char                *cmd_name;
    char                **argv;
    t_redirect          *in_redir;
    t_redirect          *out_redir;
    // ... other fields
} t_command;
*/


int handle_heredoc(t_redirect *redir, int *heredoc_pipe_fd) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe for heredoc failed");
        return -1;
    }

    // The prompt mentions readline "doesn't have to update history".
    // `rl_prep_terminal_for_heredoc ()` or similar might be needed if readline
    // is deeply integrated. For now, standard readline usage.
    // To avoid history, one might use `add_history(NULL)` before and restore,
    // or more simply, just not call `add_history` if `readline` doesn't do it by default
    // when not explicitly called. `readline` itself does add to history.
    // A simple solution is to clear history for this specific use if that's acceptable.
    // Or, use `rl_clear_signals()` if signals interfere.

    // For now, let's assume standard readline behavior is acceptable,
    // and we can refine history management later if needed.

    while (1) {
        char *line = readline("> "); // Standard heredoc prompt
        if (line == NULL) { // EOF (Ctrl+D)
            // This means the input stream ended before the delimiter was found.
            // Minishell behavior here is often to warn but proceed with what was gathered.
            fprintf(stderr, "borsh: warning: here-document delimited by end-of-file (wanted `%s')\n", redir->file);
            break;
        }

        if (strcmp(line, redir->file) == 0) {
            free(line);
            break; // Delimiter found
        }

        // Write the line and a newline character to the pipe
        if (write(pipefd[1], line, strlen(line)) == -1) {
            perror("write to heredoc pipe failed");
            free(line);
            close(pipefd[0]);
            close(pipefd[1]);
            return -1;
        }
        if (write(pipefd[1], "\n", 1) == -1) {
            perror("write newline to heredoc pipe failed");
            free(line);
            close(pipefd[0]);
            close(pipefd[1]);
            return -1;
        }
        free(line);
    }

    close(pipefd[1]); // Close write end, child will read from read end
    *heredoc_pipe_fd = pipefd[0]; // Return read end

    return 0;
}

int handle_redirections(t_command *command, int *in_fd, int *out_fd) {
    t_redirect *current_redir;
    int temp_fd;

    // Initialize with standard FDs. These will be updated if redirections exist.
    *in_fd = STDIN_FILENO;
    *out_fd = STDOUT_FILENO;

    // Process input redirections
    current_redir = command->in_redir;
    while (current_redir != NULL) {
        if (*in_fd != STDIN_FILENO) { // If a file/pipe was already opened for input
            close(*in_fd);
        }

        if (current_redir->type == T_REDIR_IN) {
            temp_fd = open(current_redir->file, O_RDONLY);
            if (temp_fd == -1) {
                fprintf(stderr, "borsh: %s: %s\n", current_redir->file, strerror(errno));
                // If an input file fails to open, we might need to close any opened output FDs too
                if (*out_fd != STDOUT_FILENO) close(*out_fd);
                return -1;
            }
            *in_fd = temp_fd;
        } else if (current_redir->type == T_HEREDOC) {
            if (handle_heredoc(current_redir, &temp_fd) == -1) {
                if (*out_fd != STDOUT_FILENO) close(*out_fd);
                // handle_heredoc should have printed an error
                return -1;
            }
            *in_fd = temp_fd; // This is the read end of the pipe
        }
        current_redir = current_redir->next;
    }

    // Process output redirections
    current_redir = command->out_redir;
    while (current_redir != NULL) {
        if (*out_fd != STDOUT_FILENO) { // If a file was already opened for output
            close(*out_fd);
        }

        if (current_redir->type == T_REDIR_OUT) {
            temp_fd = open(current_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        } else if (current_redir->type == T_REDIR_APPEND) {
            temp_fd = open(current_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            // Should not happen if parser is correct
            fprintf(stderr, "borsh: unknown output redirection type\n");
            if (*in_fd != STDIN_FILENO) close(*in_fd); // Clean up input FD if opened
            return -1;
        }
        
        if (temp_fd == -1) {
            fprintf(stderr, "borsh: %s: %s\n", current_redir->file, strerror(errno));
            if (*in_fd != STDIN_FILENO) close(*in_fd); // Clean up input FD if opened
            return -1;
        }
        *out_fd = temp_fd;
        current_redir = current_redir->next;
    }

    return 0; // Success
}
