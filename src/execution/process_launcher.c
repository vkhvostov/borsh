#include "borsh.h"

// Note: g_exit_status is declared as extern in borsh.h

/**
 * @brief Launches a new process to execute a command.
 *
 * This function forks the current process. The child process attempts to
 * execute the command specified by `args` using `execve`. The parent
 * process waits for the child to complete and stores its exit status.
 *
 * @param args Null-terminated array of strings representing the command and
 *             its arguments. `args[0]` is the command to execute.
 * @param envp Null-terminated array of strings representing the environment
 *             for the new process.
 * @return 0 on successful execution of the child (regardless of child's
 *         exit status) or if fork succeeded. Returns -1 if `fork()` fails.
 */
int	launch_process(char **args, char **envp)
{
	pid_t	pid;
	int		status;

	// Will be declared in a header file e.g. "borsh.h"
	extern int g_exit_status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	else if (pid == 0)
	{
		// Child process
		if (execve(args[0], args, envp) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
		// execve only returns on error, so we don't expect to reach here.
		// However, to be safe, exit if it somehow does.
		exit(EXIT_FAILURE);
	}
	else
	{
		// Parent process
		if (waitpid(pid, &status, 0) == -1)
		{
			perror("waitpid");
			// Even if waitpid fails, fork succeeded, so maybe not -1?
			// For now, let's stick to the plan of returning 0 from parent
			// if fork succeeded. The error is printed.
		}
		else
		{
			if (WIFEXITED(status))
				g_exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				g_exit_status = 128 + WTERMSIG(status); // Standard practice
			// else: child stopped or continued (not handled here)
		}
		return (0); // Fork succeeded, parent's job regarding this launch is done.
	}
	// Should not be reached
	return (-1);
}
