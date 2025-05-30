#include "borsh.h"   // For t_token, T_WORD, and common headers like stdio, stdlib, unistd, string, sys/stat, sys/wait, and libft.h

// Forward declaration for the function in process_launcher.c
int		launch_process(char **args, char **envp);

// Global variable for exit status (defined elsewhere)
extern int	g_exit_status;

// Helper function to free a NULL-terminated array of strings
static void	free_argv(char **argv)
{
	int	i;

	if (!argv)
		return ;
	i = 0;
	while (argv[i])
	{
		free(argv[i]);
		argv[i] = NULL;
		i++;
	}
	free(argv);
}

/**
 * @brief Builds an argv array from a linked list of tokens.
 *
 * Iterates through tokens starting from `start_token` as long as they are
 * of type T_WORD. Allocates and fills an argv array with duplicates of
 * token values.
 *
 * @param start_token The first token of the command sequence.
 * @param end_token Pointer to a t_token pointer, which will be updated to
 *                  point to the token that terminated argument collection.
 * @return A NULL-terminated array of strings (argv), or NULL on allocation failure.
 *         Each string in the array is a duplicate and must be freed.
 */
char	**build_argv_from_tokens(t_token *start_token, t_token **end_token)
{
	t_token	*current;
	int		count;
	char	**argv;
	int		i;

	count = 0;
	current = start_token;
	// Count valid T_WORD tokens with non-null values
	while (current && current->type == T_WORD && current->value)
	{
		count++;
		current = current->next;
	}
	// Set end_token to the token that stopped the loop or NULL
	if (end_token)
		*end_token = current;

	argv = (char **)malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);

	i = 0;
	current = start_token;
	while (i < count)
	{
		// Value check already done in the counting loop
		argv[i] = ft_strdup(current->value);
		if (!argv[i])
		{
			free_argv(argv); // Free previously allocated strings
			return (NULL);
		}
		current = current->next;
		i++;
	}
	argv[count] = NULL;
	return (argv);
}

/**
 * @brief Searches for a command in the directories specified by the PATH env variable.
 *
 * @param command The command name to search for.
 * @param envp The environment variables (currently unused, for future _getenv).
 * @return A dynamically allocated string containing the full path to the
 *         executable, or NULL if not found or an error occurs. The caller
 *         must free this string.
 */
char	*find_command_in_path(const char *command, char **envp)
{
	char		*path_env;
	char		**paths;
	char		*full_path;
	char		*temp_path;
	int			i;
	struct stat	path_stat;

	(void)envp; // To silence unused parameter warning if not using custom getenv
	if (!command || *command == '\0') // Do not search for empty command
		return (NULL);
	// If command contains a slash, it's a path, not to be searched in PATH
	if (ft_strchr(command, '/') != NULL)
		return (NULL);
	path_env = getenv("PATH");
	if (!path_env || *path_env == '\0')
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		temp_path = ft_strjoin(paths[i], "/");
		if (!temp_path)
		{
			free_argv(paths);
			return (NULL);
		}
		full_path = ft_strjoin(temp_path, command);
		free(temp_path);
		if (!full_path)
		{
			free_argv(paths);
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
		{
			// Check if it's a directory
			if (stat(full_path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
			{
				free(full_path); // It's a directory, skip
				i++;
				continue;
			}
			free_argv(paths); // Free the ft_split array
			return (full_path); // Found executable
		}
		free(full_path);
		i++;
	}
	free_argv(paths); // Free the ft_split array
	return (NULL);
}

static void	handle_exec_error(const char *cmd_name, const char *error_msg, int err_code)
{
	ft_putstr_fd("borsh: ", STDERR_FILENO);
	ft_putstr_fd(cmd_name, STDERR_FILENO);
	ft_putstr_fd(error_msg, STDERR_FILENO);
	g_exit_status = err_code;
}

/**
 * @brief Executes a command specified by tokens.
 *
 * @param command_token The first token of the command.
 * @param envp The environment variables.
 * @return 0 on successful launch attempt, 1 on pre-launch failure.
 *         Updates g_exit_status based on command execution or pre-launch errors.
 */
int	execute_command(t_token *command_token, char **envp)
{
	char		**argv;
	t_token		*next_token_after_args; // For future use if needed by caller
	char		*executable_path;
	char		*cmd_name;
	int			launch_ret;
	struct stat	path_stat;

	if (!command_token || command_token->type != T_WORD || !command_token->value)
	{
		g_exit_status = 0; // No command executed, often treated as success ($? = 0) or specific error.
                           // Bash returns 0 for an empty command line. Let's align.
		return (0); // Indicate success in "not doing anything", or change to error if required.
	}
	argv = build_argv_from_tokens(command_token, &next_token_after_args);
	if (!argv) // Malloc failure in build_argv
	{
		perror("borsh: build_argv_from_tokens");
		return (g_exit_status = 1, 1); // General error
	}
	if (!argv[0]) // No command from tokens (e.g. empty string token)
	{
		free_argv(argv);
		g_exit_status = 0; // Similar to empty command line
		return (0);
	}
	cmd_name = argv[0];
	executable_path = NULL;
	if (ft_strchr(cmd_name, '/') != NULL) // Command is a path
	{
		if (access(cmd_name, F_OK) == 0) // File exists
		{
			if (stat(cmd_name, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
				handle_exec_error(cmd_name, ": is a directory\n", 126);
			else if (access(cmd_name, X_OK) == 0) // File is executable
				executable_path = ft_strdup(cmd_name);
			else // File exists but not executable
				handle_exec_error(cmd_name, ": Permission denied\n", 126);
		}
		else // File does not exist
			handle_exec_error(cmd_name, ": No such file or directory\n", 127);
	}
	else // Command is not a path, search in PATH
	{
		executable_path = find_command_in_path(cmd_name, envp);
		if (!executable_path) // Not found in PATH
			handle_exec_error(cmd_name, ": command not found\n", 127);
	}

	if (!executable_path) // Path resolution failed or command not executable
	{
		free_argv(argv);
		return (1); // Indicate pre-launch failure; g_exit_status is already set
	}

	free(argv[0]); // Free the original command name string from build_argv
	argv[0] = executable_path; // Assign the resolved, executable path

	launch_ret = launch_process(argv, envp); // g_exit_status is set by launch_process

	free_argv(argv); // Frees all elements including executable_path, then argv itself

	if (launch_ret == -1) // Fork or other critical error in launch_process
	{
		// perror("borsh: launch_process"); // launch_process should perror
		g_exit_status = 1; // General error for failure to launch, if not already more specific
		return (1); // Indicate launch failure
	}
	return (0); // Successful attempt to launch (g_exit_status holds command's actual status)
}
