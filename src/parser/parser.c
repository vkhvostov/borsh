#include "../include/borsh.h"

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

int	add_arg(char ***argv, char *value)
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
		return (0);
	i = 0;
	while (i < len)
	{
		new[i] = (*argv)[i];
		i++;
	}
	new[len] = ft_strdup(value);
	if (!new[len])
	{
		free(new);
		return (0);
	}
	new[len + 1] = NULL;
	free(*argv);
	*argv = new;
	return (1);
}

int	is_redirect(t_token *tokens)
{
	return (tokens->type == T_REDIR_IN
				|| tokens->type == T_REDIR_OUT
				|| tokens->type == T_REDIR_APPEND
				|| tokens->type == T_HEREDOC);
}

void	handle_redirs(t_token **tokens, t_command	*current)
{
	if ((*tokens)->type == T_REDIR_IN)
		handle_redir_tokens(&current->in_redir, tokens, T_REDIR_IN);
	else if ((*tokens)->type == T_REDIR_OUT)
		handle_redir_tokens(&current->out_redir, tokens, T_REDIR_OUT);
	else if ((*tokens)->type == T_REDIR_APPEND)
		handle_redir_tokens(&current->out_redir, tokens, T_REDIR_APPEND);
	else if ((*tokens)->type == T_HEREDOC)
		handle_redir_tokens(&current->in_redir, tokens, T_HEREDOC);
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
			if (tokens->next == NULL || tokens->next->type == T_PIPE)
				break ;
			handle_pipe_tokens(&tokens, &current);
			continue;
		}
		else if (tokens->type == T_WORD)
			handle_word_tokens(current, tokens);
		else if (is_redirect(tokens))
			handle_redirs(&tokens, current);
		tokens = tokens->next;
	}
	return (head);
}
