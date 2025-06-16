#include "../../include/borsh.h"

static char	**create_extended_argv(char **argv, char *value)
{
	int		len;
	char	**new;
	int		i;

	len = 0;
	while (argv && argv[len])
		len++;
	new = malloc(sizeof(char *) * (len + 2));
	if (!new)
		return (NULL);
	i = 0;
	while (i < len)
	{
		new[i] = argv[i];
		i++;
	}
	new[len] = ft_strdup(value);
	if (!new[len])
	{
		free(new);
		return (NULL);
	}
	new[len + 1] = NULL;
	return (new);
}

int	add_arg(char ***argv, char *value)
{
	char	**new;

	new = create_extended_argv(*argv, value);
	if (!new)
	{
		set_last_exit_status(1);
		return (0);
	}
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

int	fill_command_list(t_command *cmd_list, t_token *tokens, char **env)
{
	t_command	*curr;
	t_token		*curr_t;

	curr = cmd_list;
	curr_t = tokens;
	while (curr_t)
	{
		if (curr_t->type == T_PIPE)
		{
			handle_pipe_tokens(&curr_t, &curr, env);
			continue ;
		}
		else if (is_redirect(curr_t))
			handle_redir_tokens(&curr->redirs, &curr_t, curr_t->type);
		else if (curr_t->type == T_WORD)
		{
			if (!handle_word_tokens(curr, curr_t))
			{
				set_last_exit_status(1);
				return (0);
			}
		}
		curr_t = curr_t->next;
	}
	return (1);
}

t_command	*parse_tokens(t_token *tokens, char **env)
{
	t_command	*cmd_list;

	if (!tokens)
	{
		set_last_exit_status(2);
		return (NULL);
	}
	cmd_list = init_command(env);
	if (!cmd_list)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	if (!fill_command_list(cmd_list, tokens, env))
	{
		free_commands(cmd_list);
		return (NULL);
	}
	return (cmd_list);
}
