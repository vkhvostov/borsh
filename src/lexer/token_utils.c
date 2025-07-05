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

// checks the type of the next token and calls the appropriate parser
void	handle_token(char *input, t_token **current_token, int *i,
	int *exit_status)
{
	if (input[*i] == '\\' || is_word_char(input[*i]))
	{
		*current_token = parse_word(input, i, exit_status);
		expand_tilde(*current_token);
	}
	else if (input[*i] == '\'' && !is_escaped_quote(input, *i))
		*current_token = parse_quote(input, i, exit_status);
	else if (input[*i] == '"' && !is_escaped_quote(input, *i))
		*current_token = parse_quote(input, i, exit_status);
	else if (input[*i] == '|')
		*current_token = parse_pipe(i);
	else if (input[*i] == '<' || input[*i] == '>')
		*current_token = parse_redirection(input, i, exit_status);
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
