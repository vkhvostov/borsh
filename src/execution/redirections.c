#include "borsh.h"   // For g_exit_status, ft_putstr_fd, and common headers (unistd, fcntl, stdio, stdlib, string).

// Global variable for exit status (defined elsewhere)
extern int	g_exit_status;

/**
 * @brief Handles input redirection (<).
 *
 * Opens the specified filename for reading and duplicates its file descriptor
 * to STDIN_FILENO.
 *
 * @param filename The name of the file to redirect input from.
 * @return 0 on success, -1 on failure. Sets g_exit_status on failure.
 */
int	handle_redir_in(const char *filename)
{
	int	fd;

	if (!filename)
	{
		ft_putstr_fd("minishell: missing name for redirect\n", STDERR_FILENO);
		g_exit_status = 2; // Syntax-like error, as per shell conventions
		return (-1);
	}
	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(filename); // e.g., "filename: No such file or directory"
		g_exit_status = 1;
		return (-1);
	}
	if (dup2(fd, STDIN_FILENO) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("dup2");   // e.g., "dup2: Bad file descriptor"
		close(fd);
		g_exit_status = 1;
		return (-1);
	}
	if (close(fd) == -1) // Close the original file descriptor
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("close");
		g_exit_status = 1; // Though redirection might have succeeded, an error occurred
		return (-1); 
	}
	return (0);
}

/**
 * @brief Handles output redirection (>).
 *
 * Opens the specified filename for writing, creating it if it doesn't exist,
 * and truncating it if it does. Duplicates its file descriptor to STDOUT_FILENO.
 *
 * @param filename The name of the file to redirect output to.
 * @return 0 on success, -1 on failure. Sets g_exit_status on failure.
 */
int	handle_redir_out(const char *filename)
{
	int	fd;

	if (!filename)
	{
		ft_putstr_fd("minishell: missing name for redirect\n", STDERR_FILENO);
		g_exit_status = 2;
		return (-1);
	}
	// Permissions: rw-r--r-- (0644)
	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(filename); // e.g., "filename: Permission denied"
		g_exit_status = 1;
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("dup2");
		close(fd);
		g_exit_status = 1;
		return (-1);
	}
	if (close(fd) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("close");
		g_exit_status = 1;
		return (-1);
	}
	return (0);
}

/**
 * @brief Handles output append redirection (>>).
 *
 * Opens the specified filename for writing, creating it if it doesn't exist,
 * and appending to it if it does. Duplicates its file descriptor to STDOUT_FILENO.
 *
 * @param filename The name of the file to append output to.
 * @return 0 on success, -1 on failure. Sets g_exit_status on failure.
 */
int	handle_redir_append(const char *filename)
{
	int	fd;

	if (!filename)
	{
		ft_putstr_fd("minishell: missing name for redirect\n", STDERR_FILENO);
		g_exit_status = 2;
		return (-1);
	}
	// Permissions: rw-r--r-- (0644)
	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(filename);
		g_exit_status = 1;
		return (-1);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("dup2");
		close(fd);
		g_exit_status = 1;
		return (-1);
	}
	if (close(fd) == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror("close");
		g_exit_status = 1;
		return (-1);
	}
	return (0);
}
