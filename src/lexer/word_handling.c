#include "../../include/borsh.h"

// Helper function to process escape sequences in word content
static char	*process_escape_sequences(char *content)
{
	char	*result;
	int		i;
	int		j;

	if (!content)
		return (NULL);
	result = malloc(ft_strlen(content) + 1);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	while (content[i])
	{
		if (content[i] == '\\' && content[i + 1])
		{
			result[j++] = content[i + 1];
			i += 2;
		}
		else
			result[j++] = content[i++];
	}
	result[j] = '\0';
	return (result);
}

// extracts a substring for a word from the input and processes escape sequences
char	*handle_word_content(char *input, int start, int end)
{
	char	*word;
	char	*processed;

	word = ft_strndup(&input[start], end - start);
	if (!word)
		return (NULL);
	processed = process_escape_sequences(word);
	free(word);
	return (processed);
}

// joins a word and a quoted string
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

// handles a word immediately followed by a quoted section
t_token	*handle_word_with_quote(char *input, int *i, char *word,
	int *exit_status)
{
	t_token	*token;
	char	*quoted;
	char	*result;

	if (input[*i] == '\'')
		token = parse_quote(input, i, exit_status);
	else
		token = parse_quote(input, i, exit_status);
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
