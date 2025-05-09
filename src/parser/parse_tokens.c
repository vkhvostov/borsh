#include "../../include/borsh.h"

t_command	*init_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->cmd_name = NULL;
	cmd->argv = NULL;
	cmd->in_redir = NULL;
	cmd->out_redir = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	add_arg(char ***argv, char *value)
{
	int		len;
	char	**new;
	int		i;

	len = 0;
	if (*argv)
		while ((*argv)[len])
			len++;
	new = malloc(sizeof(char *) * (len + 2));
	if (!new)
		return ;
	i = 0;
	while (i < len)
	{
		new[i] = (*argv)[i];
		i++;
	}
	new[len] = ft_strdup(value);
	new[len + 1] = NULL;
	free(*argv);
	*argv = new;
}

static void	add_redirect(t_redirect **list, t_redirect_type type, char *file)
{
	t_redirect	*new;
	t_redirect	*last;

	new = malloc(sizeof(t_redirect));
	if (!new)
		return ;
	new->type = type;
	new->file = ft_strdup(file);
	new->next = NULL;
	if (!*list)
		*list = new;
	else
	{
		last = *list;
		while (last->next)
			last = last->next;
		last->next = new;
	}
}

t_command	*parse_tokens(t_token *tokens)
{
	t_command	*head = NULL;
	t_command	*current = NULL;

	while (tokens)
	{
		if (!current)
		{
			current = init_command();
			if (!head)
				head = current;
		}
		if (tokens->type == T_PIPE)
		{
			if (tokens->next == NULL || tokens->next->type == T_PIPE) // Handling consecutive pipes
		{
			// Handle error: Consecutive pipes are invalid
			printf("Syntax error: consecutive pipes\n");
			return (NULL);
		}
			current->next = init_command();
			current = current->next;
			tokens = tokens->next;
			continue;
		}
		else if (tokens->type == T_WORD)
		{
			if (!current->cmd_name)
				current->cmd_name = ft_strdup(tokens->value);
			add_arg(&current->argv, tokens->value);
		}
		else if (tokens->type == T_REDIR_IN)
		{
			if (tokens->next == NULL || tokens->next->type != T_WORD) // Handle missing filename for redirection
		{
			printf("Syntax error: missing file for input redirection\n");
			return (NULL);
		}
			if (tokens->next)
				add_redirect(&current->in_redir, REDIR_IN, tokens->next->value);
			tokens = tokens->next;
		}
		else if (tokens->type == T_REDIR_OUT)
		{
			if (tokens->next == NULL || tokens->next->type != T_WORD)
		{
			printf("Syntax error: missing file for output redirection\n");
			return (NULL);
		}
			if (tokens->next)
				add_redirect(&current->out_redir, REDIR_OUT, tokens->next->value);
			tokens = tokens->next;
		}
		else if (tokens->type == T_REDIR_APPEND)
		{
			if (tokens->next == NULL || tokens->next->type != T_WORD)
		{
			printf("Syntax error: missing file for append redirection\n");
			return (NULL);
		}
			if (tokens->next)
				add_redirect(&current->out_redir, REDIR_APPEND, tokens->next->value);
			tokens = tokens->next;
		}
		else if (tokens->type == T_HEREDOC)
		{
			if (tokens->next == NULL || tokens->next->type != T_WORD)
		{
			printf("Syntax error: missing delimiter for heredoc\n");
			return (NULL);
		}
			if (tokens->next)
				add_redirect(&current->in_redir, REDIR_HEREDOC, tokens->next->value);
			tokens = tokens->next;
		}
		tokens = tokens->next;
	}
	return (head);
}
