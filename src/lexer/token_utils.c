#include "../../include/borsh.h"

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

void	free_tokens(t_token *token_list)
{
	t_token	*tmp;

	while (token_list)
	{
		tmp = token_list;
		token_list = token_list->next;
		free(tmp->value);
		free(tmp);
	}
}

// checks the type of the next token and calls the appropriate parser
void	handle_token(char *input, t_token **current_token, int *i,
	int *exit_status)
{
	if (input[*i] == '\'')
		*current_token = parse_single_quote(input, i, exit_status);
	else if (input[*i] == '"')
		*current_token = parse_double_quote(input, i, exit_status);
	else if (is_word_char(input[*i]))
	{
		*current_token = parse_word(input, i, exit_status);
		expand_tilde(*current_token);
	}
	else if (input[*i] == '|')
		*current_token = parse_pipe(i);
	else if (input[*i] == '<' || input[*i] == '>')
		*current_token = parse_redirection(input, i, exit_status);
}

int	count_redir_arrows(char *input, int *i, int *exit_status)
{
	int		start;
	char	c;
	int		count;

	start = *i;
	c = input[*i];
	count = 0;
	while (input[*i] == c)
	{
		count++;
		(*i)++;
	}
	if (count > 2)
	{
		ft_putstr_fd("borsh: syntax error near unexpected token `",
			STDERR_FILENO);
		ft_putstr_fd(&input[start], STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		*exit_status = 2;
		return (-1);
	}
	return (1);
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
