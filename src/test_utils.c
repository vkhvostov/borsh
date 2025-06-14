#include "../include/borsh.h"

void	print_tokens(t_token *token_list)
{
	t_token	*temp = token_list;

	while(temp)
	{
		printf("Token: Type: %d, Value: %s\n", temp->type, temp->value);
		temp = temp->next;
	}
}

void	print_redirects(t_redirect *redir_list, const char *label)
{
	while (redir_list)
	{
		printf("  %s: (%d) %s\n", label, redir_list->type, redir_list->file);
		redir_list = redir_list->next;
	}
}

void	print_commands(t_command *cmd_list)
{
	int	i;

	while (cmd_list)
	{
		printf("Command:\n");
		if (cmd_list->cmd_name)
			printf("  cmd_name: %s\n", cmd_list->cmd_name);
		if (cmd_list->argv)
		{
			i = 0;
			while (cmd_list->argv[i])
			{
				printf("  argv[%d]: %s\n", i, cmd_list->argv[i]);
				i++;
			}
		}
		print_redirects(cmd_list->redirs, "redirs");
		printf("----\n");
		cmd_list = cmd_list->next;
	}
}