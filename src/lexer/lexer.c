#include "../../include/borsh.h"

//converts input into a linked list of tokens
static t_token	*create_token_list(char *input, int *exit_status)
{
	int		i;
	t_token	*token_list;
	t_token	*current_token;

	i = 0;
	token_list = NULL;
	current_token = NULL;
	while (input[i])
	{
		if (input[i] == ' ' || (input[i] >= 9 && input[i] <= 13))
		{
			i++;
			continue ;
		}
		handle_token(input, &current_token, &i, exit_status);
		if (!current_token)
		{
			free_tokens(token_list);
			*exit_status = 2;
			return (NULL);
		}
		add_token(&token_list, current_token);
		current_token = NULL;
	}
	return (token_list);
}

//tokenizes the input string into a list of tokens
t_token	*lexer(char *input, int *exit_status)
{
	t_token	*token_list;

	if (!input)
	{
		*exit_status = 1;
		return (NULL);
	}
	token_list = create_token_list(input, exit_status);
	return (token_list);
}
