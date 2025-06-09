#include "../../include/borsh.h"

t_token	*lexer(char *input)
{
	int		i;
	t_token	*token_list;
	t_token	*current_token;
	char	*trimmed_input;
	char	*set;

	i = 0;
	token_list = NULL;
	current_token = NULL;
	set = " \n\t\r\v\f";
	trimmed_input = ft_strtrim(input, set);
	if (!trimmed_input)
	{
		set_last_exit_status(1);
		return (NULL);
	}
	while(trimmed_input[i])
	{
		skip_whitespace(trimmed_input, &i);
		handle_token(trimmed_input, &current_token, &i);
		if (!current_token)
		{
			free_tokens(token_list);
			free(trimmed_input);
			set_last_exit_status(2);  // Syntax error
			return (NULL);
		}
		add_token(&token_list, current_token);
		current_token = NULL;
	}
	free(trimmed_input);
	return (token_list);
}
