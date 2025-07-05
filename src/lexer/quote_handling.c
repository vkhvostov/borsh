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

	quote_type = input[quote_start - 1];
	quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
	if (!quoted)
		return (NULL);
	if (quote_type == '"')
	{
		processed = process_quoted_escapes(quoted, quote_type);
		free(quoted);
		return (processed);
	}
	return (quoted);
}

// joins a newly parsed quoted part with a previous result
char	*handle_quoted_part_result(char *result, char *quoted)
{
	char	*temp;

	if (!result)
		return (quoted);
	temp = result;
	result = join_word_and_quoted(temp, quoted);
	if (!result)
		return (NULL);
	return (result);
}

// handles a complete quoted section
char	*handle_quoted_part(char *input, int *i, char *result, int *exit_status)
{
	int		quote_start;
	int		quote_end;
	char	*quoted;

	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '"', exit_status) == -1)
	{
		if (result)
			free(result);
		return (NULL);
	}
	quote_end = *i;
	(*i)++;
	quoted = handle_quoted_content(input, quote_start, quote_end);
	if (!quoted)
	{
		if (result)
			free(result);
		return (NULL);
	}
	return (handle_quoted_part_result(result, quoted));
}
