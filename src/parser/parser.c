#include "../../include/borsh.h"

// creates a new argv array with 
// an additional value appended
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

// Adds a new argument to the argv array,
// reallocating as needed
int	add_arg(char ***argv, char *value, int *exit_status)
{
	char	**new;

	new = create_extended_argv(*argv, value);
	if (!new)
	{
		*exit_status = 1;
		return (0);
	}
	free(*argv);
	*argv = new;
	return (1);
}

// Checks if a redirection token
static int	is_redirect(t_token *tokens)
{
	return (tokens->type == T_REDIR_IN
		|| tokens->type == T_REDIR_OUT
		|| tokens->type == T_REDIR_APPEND
		|| tokens->type == T_HEREDOC);
}

// Fills the command list with parsed tokens,
// handling pipes, redirections, and words
static int	fill_command_list(t_command *cmd_list, t_token *tokens, char **env,
	int *status)
{
	t_command	*curr;
	t_token		*curr_t;

	curr = cmd_list;
	curr_t = tokens;
	while (curr_t)
	{
		if (curr_t->type == T_PIPE)
		{
			handle_pipe_tokens(&curr_t, &curr, env, status);
			if (curr_t == NULL)
				break ;
			continue ;
		}
		else if (is_redirect(curr_t))
			handle_redir_tokens(&curr->redirs, &curr_t, curr_t->type, status);
		else if (curr_t->type == T_WORD)
		{
			if (!handle_word_token_logic(curr, curr_t, status))
				return (0);
		}
		curr_t = curr_t->next;
	}
	return (1);
}

// Parses the token list into a linked list of commands
t_command	*parse_tokens(t_token *tokens, char **env, int *exit_status)
{
	t_command	*cmd_list;

	if (!tokens)
	{
		*exit_status = 2;
		return (NULL);
	}
	cmd_list = init_command(env, exit_status);
	if (!cmd_list)
	{
		*exit_status = 1;
		return (NULL);
	}
	if (tokens && tokens->type == T_PIPE)
	{
		ft_putstr_fd("borsh: syntax error near unexpected token `|'\n", 2);
		return (exit_on_error(cmd_list, exit_status, 2));
	}
	if (!fill_command_list(cmd_list, tokens, env, exit_status))
		return (exit_on_error(cmd_list, exit_status, 1));
	return (cmd_list);
}
