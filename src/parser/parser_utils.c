#include "../../include/borsh.h"

// Helper function to copy environment variables
static char **copy_environment(char **system_env)
{
	char **env = NULL;
	int env_count = 0;
	int i;

	if (!system_env)
		return NULL;

	// Count environment variables
	while (system_env[env_count])
		env_count++;

	// Allocate space for environment array
	env = malloc(sizeof(char *) * (env_count + 1));
	if (!env)
		return NULL;

	// Copy each environment variable
	for (i = 0; i < env_count; i++)
	{
		env[i] = ft_strdup(system_env[i]);
		if (!env[i])
		{
			// If allocation fails, free everything allocated so far
			while (--i >= 0)
				free(env[i]);
			free(env);
			return NULL;
		}
	}
	env[env_count] = NULL;

	return env;
}

void	free_argv(char **argv)
{
	int	i = 0;

	if (!argv)
		return;
	while (argv[i])
	{
		free(argv[i]);
		i++;
	}
	free(argv);
}

void	free_redirects(t_redirect *redir)
{
	t_redirect	*tmp;

	while (redir)
	{
		tmp = redir->next;
		free(redir->file);
		free(redir);
		redir = tmp;
	}
}

void	free_str_array(char **arr)
{
	int	i = 0;

	if (!arr)
		return;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

void	free_commands(t_command *cmd)
{
	t_command	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free(cmd->cmd_name);
		free_str_array(cmd->argv);
		free_str_array(cmd->env);  // Free environment variables
		free_redirects(cmd->in_redir);
		free_redirects(cmd->out_redir);
		free(cmd);
		cmd = tmp;
	}
}

t_command	*init_command(char **env)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->cmd_name = NULL;
	cmd->argv = NULL;
	cmd->env = copy_environment(env);  // Initialize environment
	if (!cmd->env)
	{
		free(cmd);
		return (NULL);
	}
	cmd->in_redir = NULL;
	cmd->out_redir = NULL;
	cmd->next = NULL;
	return (cmd);
}