#include "../../include/borsh.h"

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
