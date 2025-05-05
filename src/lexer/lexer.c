#include "../../include/borsh.h"

t_token	*lexer(char *input)
{
	int		i;
	t_token	*token_list;
	t_token	*current_token;

	i = 0;
	token_list = NULL;
	current_token = NULL;
	while(input[i])
	{
		skip_whitespace(input, &i);
		handle_token(input, &current_token, &i);
		if (!current_token)
		{
			free_tokens(token_list);
			return (NULL);
		}
		add_token(&token_list, current_token);
		current_token = NULL;
	}
	return (token_list);
}
