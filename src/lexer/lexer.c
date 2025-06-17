#include "../../include/borsh.h"

static t_token	*create_token_list(char *trimmed_input)
{
	int		i;
	t_token	*token_list;
	t_token	*current_token;

	i = 0;
	token_list = NULL;
	current_token = NULL;
	while (trimmed_input[i])
	{
		skip_whitespace(trimmed_input, &i);
		handle_token(trimmed_input, &current_token, &i);
		if (!current_token)
		{
			free_tokens(token_list);
			set_last_exit_status(2);
			return (NULL);
		}
		add_token(&token_list, current_token);
		current_token = NULL;
	}
	return (token_list);
}

t_token	*lexer(char *input)
{
	t_token	*token_list;
	char	*trimmed_input;
	char	*set;

	set = " \n\t\r\v\f";
	trimmed_input = ft_strtrim(input, set);
	if (!trimmed_input)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	token_list = create_token_list(trimmed_input);
	free(trimmed_input);
	return (token_list);
}
