#include "../../include/borsh.h"

char	*handle_word_content(char *input, int start, int end)
{
	char	*word;

	word = ft_strndup(&input[start], end - start);
	if (!word)
		return (NULL);
	return (word);
}

char	*handle_word_part(char *input, int *i, char *result)
{
	int		start;
	char	*word;
	char	*temp;

	start = *i;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	word = handle_word_content(input, start, *i);
	if (!word)
	{
		if (result)
			free(result);
		return (NULL);
	}
	if (!result)
		return (word);
	temp = result;
	result = join_word_and_quoted(temp, word);
	if (!result)
		return (NULL);
	return (result);
}

char	*join_word_and_quoted(char *word, char *quoted)
{
	char	*result;
	char	*temp;

	if (!word)
		return (quoted);
	if (!quoted)
		return (word);
	temp = word;
	result = ft_strjoin(word, quoted);
	free(temp);
	free(quoted);
	return (result);
}

t_token	*handle_word_with_quote(char *input, int *i, char *word)
{
	t_token	*token;
	char	*quoted;
	char	*result;

	if (input[*i] == '\'')
		token = parse_single_quote(input, i);
	else
		token = parse_double_quote(input, i);
	if (!token)
	{
		free(word);
		return (NULL);
	}
	quoted = token->value;
	result = join_word_and_quoted(word, quoted);
	free(token);
	return (create_word_token(result));
}
