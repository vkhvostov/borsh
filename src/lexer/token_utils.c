#include "../../include/borsh.h"

// Helper function to check if a quote at position i is escaped
static int	is_escaped_quote(char *input, int i)
{
	int	escape_count;

	escape_count = 0;
	while (i > 0 && input[i - 1] == '\\')
	{
		escape_count++;
		i--;
	}
	return (escape_count % 2);
}

void	add_token(t_token **token_list, t_token *new_token)
{
	t_token	*temp;

	if (!*token_list)
		*token_list = new_token;
	else
	{
		temp = *token_list;
		while (temp->next)
			temp = temp->next;
		temp->next = new_token;
	}
}

t_token	*parse_combined_word(char *input, int *i, int *exit_status)
{
	char	*result;

	result = NULL;
	while (input[*i])
	{
		if (input[*i] == '"' && !is_escaped_quote(input, *i))
		{
			if (!handle_double_quotes(input, i, &result, exit_status))
				return (NULL);
		}
		else if (input[*i] == '\'' && !is_escaped_quote(input, *i))
		{
			if (!handle_single_quotes(input, i, &result, exit_status))
				return (NULL);
		}
		else if (is_word_char(input[*i]) || input[*i] == '\\')
		{
			if (!handle_unquoted_word(input, i, &result))
				return (NULL);
		}
		else
			break ;
	}
	return (create_word_token(result));
}

// checks the type of the next token and calls the appropriate parser
void	handle_token(char *input, t_token **current_token, int *i,
	int *exit_status)
{
	if (input[*i] == '|' || input[*i] == '<' || input[*i] == '>')
	{
		if (input[*i] == '|')
			*current_token = parse_pipe(i);
		else
			*current_token = parse_redirection(input, i, exit_status);
	}
	else
	{
		*current_token = parse_combined_word(input, i, exit_status);
		if (*current_token)
			expand_tilde(*current_token);
	}
}

int	redir_token_check(char *input, int *i, int *exit_status, t_token *token)
{
	int	tmp;

	tmp = *i;
	skip_whitespace(input, &tmp);
	if (!input[tmp] || input[tmp] == '<' || input[tmp] == '>'
		|| input[tmp] == '|')
	{
		ft_putstr_fd("borsh: syntax error near unexpected token `",
			STDERR_FILENO);
		if (!input[tmp])
			ft_putstr_fd("newline", STDERR_FILENO);
		else
			ft_putstr_fd(&input[tmp], STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		*exit_status = 2;
		free(token);
		return (-1);
	}
	return (1);
}
