#include "../../include/borsh.h"

// Helper function to process escape sequences in quoted content
static char	*process_quoted_escapes(char *content, char quote_type)
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
		if (content[i] == '\\' && content[i + 1]
			&& (content[i + 1] == quote_type || content[i + 1] == '\\'))
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

// extracts the content from within a pair of quotes
// processes escape sequences
char	*handle_quoted_content(char *input, int quote_start, int quote_end)
{
	char	*quoted;
	char	*processed;
	char	quote_type;
	int		is_empty;

	quote_type = input[quote_start - 1];
	is_empty = (quote_start == quote_end);
	if (is_empty)
		quoted = ft_strdup("");
	else
		quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
	if (!quoted)
		return (NULL);
	if (!is_empty && quote_type == '"')
	{
		processed = process_quoted_escapes(quoted, quote_type);
		free(quoted);
		return (processed);
	}
	return (quoted);
}
