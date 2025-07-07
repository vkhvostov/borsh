#include "../../include/borsh.h"

t_token	*free_null(char	*result)
{
	if (result)
		free(result);
	return (NULL);
}

t_token	*free_return(char *value)
{
	t_token	*token;

	token = create_quote_token(value);
	if (!token && value)
		free(value);
	return (token);
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

void	expand_tilde(t_token *token)
{
	char	*home;
	char	*new_value;
	size_t	home_len;
	size_t	suffix_len;
	size_t	new_len;

	if (!token || !token->value)
		return ;
	if (token->value[0] != '~')
		return ;
	if (token->value[1] != '\0' && token->value[1] != '/')
		return ;
	home = getenv("HOME");
	if (!home)
		return ;
	home_len = ft_strlen(home);
	suffix_len = ft_strlen(token->value + 1);
	new_len = home_len + suffix_len + 1;
	new_value = malloc(sizeof(char) * new_len);
	if (!new_value)
		return ;
	ft_strlcpy(new_value, home, new_len);
	ft_strlcat(new_value, token->value + 1, new_len);
	free(token->value);
	token->value = new_value;
}
