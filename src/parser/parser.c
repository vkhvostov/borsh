#include "../../include/borsh.h"

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
	{
		set_last_exit_status(1);  // Memory error
		return (0);
	}
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
		set_last_exit_status(1);  // Memory error
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
	handle_redir_tokens(&current->redirs, tokens, (*tokens)->type);
}

t_command	*parse_tokens(t_token *tokens, char **env)
{
	t_command	*cmd_list;
	t_command	*current;
	t_token		*current_token;

	if (!tokens)
	{
		set_last_exit_status(2);  // Syntax error
		return (NULL);
	}
	current_token = tokens;
	cmd_list = init_command(env);
	if (!cmd_list)
	{
		set_last_exit_status(1);  // Memory error
		return (NULL);
	}
	current = cmd_list;
	while (current_token)
	{
		if (current_token->type == T_PIPE)
		{
			handle_pipe_tokens(&current_token, &current, env);
			continue ;
		}
		else if (is_redirect(current_token))
			handle_redir_tokens(&current->redirs, &current_token, current_token->type);
		else if (current_token->type == T_WORD)
		{
			if (!handle_word_tokens(current, current_token))
			{
				free_commands(cmd_list);
				set_last_exit_status(1);  // Memory error
				return (NULL);
			}
		}
		current_token = current_token->next;
	}
	return (cmd_list);
}
