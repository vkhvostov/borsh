#include "../../include/borsh.h"

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
		free_redirects(cmd->in_redir);
		free_redirects(cmd->out_redir);
		free(cmd);
		cmd = tmp;
	}
}