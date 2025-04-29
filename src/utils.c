#include "../include/borsh.h"

int	is_word_char(char c)
{
	return (ft_isalpha(c) || ft_isdigit(c) || c == '_');
}

void	skip_whitespace(char *input, int *i)
{
	while(input[*i] && (input[*i] == 32 || (input[*i] >= 9 && input[*i] <= 13)))
		(*i)++;
}
