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

void	handle_token(char *input, t_token **current_token, int *i)
{
	if (input[*i] == '\'')
		*current_token = parse_single_quote(input, i);
	else if (input[*i] == '"')
		*current_token = parse_double_quote(input, i);
	else if (is_word_char(input[*i]))
		*current_token = parse_word(input, i);
	else if (input[*i] == '|')
		*current_token = parse_pipe(i);
	else if (input[*i] == '<' || input[*i] == '>')
		*current_token = parse_redirection(input, i);
}
