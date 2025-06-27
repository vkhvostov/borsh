#include "../../include/borsh.h"

// frees an array of strings
void	free_str_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

// frees a linked list of t_redirect structs
static void	free_redirects(t_redirect *redir)
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

// frees a linked list of command structures, including their 
// arguments, environment, and redirections
void	free_commands(t_command *cmd)
{
	t_command	*tmp;

	while (cmd)
	{
		tmp = cmd->next;
		free(cmd->cmd_name);
		free_str_array(cmd->argv);
		free_str_array(cmd->env);
		free_redirects(cmd->redirs);
		free(cmd);
		cmd = tmp;
	}
}
