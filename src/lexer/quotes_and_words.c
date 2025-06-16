#include "../../include/borsh.h"

static t_token	*create_word_token(char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;
	token->value = value;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

static char	*handle_quoted_content(char *input, int quote_start, int quote_end)
{
	char	*quoted;

	quoted = ft_strndup(&input[quote_start], quote_end - quote_start);
	if (!quoted)
		return (NULL);
	return (quoted);
}

static char	*handle_word_content(char *input, int start, int end)
{
	char	*word;

	word = ft_strndup(&input[start], end - start);
	if (!word)
		return (NULL);
	return (word);
}

static char	*join_word_and_quoted(char *word, char *quoted)
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

static int	handle_unclosed_quote(char quote_type)
{
	if (quote_type == '\'')
		printf("borsh: syntax error: unclosed single quote\n");
	else
		printf("borsh: syntax error: unclosed double quote\n");
	set_last_exit_status(2);
	return (-1);
}

static int	parse_quoted_part_loop(char *input, int *i, char quote_type)
{
	while (input[*i] && input[*i] != quote_type)
		(*i)++;
	if (input[*i] != quote_type)
	{
		handle_unclosed_quote(quote_type);
		return (-1);
	}
	return (0);
}

static char	*handle_single_quote_content(char *input, int start,
	int quote_start, int quote_end)
{
	char	*word;
	char	*quoted;
	char	*result;

	if (quote_start > start + 1)
	{
		word = handle_word_content(input, start, quote_start - 1);
		quoted = handle_quoted_content(input, quote_start, quote_end);
		result = join_word_and_quoted(word, quoted);
	}
	else
		result = handle_quoted_content(input, quote_start, quote_end);
	return (result);
}

static char	*handle_single_quote_after(char *input, int *i, int quote_end,
	char *result)
{
	char	*word;
	char	*quoted;

	if (*i > quote_end + 1)
	{
		word = handle_word_content(input, quote_end + 1, *i);
		quoted = result;
		result = join_word_and_quoted(quoted, word);
	}
	return (result);
}

static t_token	*create_single_quote_token(char *result)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = T_WORD;
	token->value = result;
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

t_token	*parse_single_quote(char *input, int *i)
{
	int		start;
	int		quote_start;
	int		quote_end;
	char	*result;

	start = *i;
	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '\'') == -1)
		return (NULL);
	quote_end = *i;
	(*i)++;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	result = handle_single_quote_content(input, start, quote_start, quote_end);
	result = handle_single_quote_after(input, i, quote_end, result);
	return (create_single_quote_token(result));
}

static char	*handle_quoted_part_result(char *result, char *quoted)
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

static char	*handle_quoted_part(char *input, int *i, char *result)
{
	int		quote_start;
	int		quote_end;
	char	*quoted;

	quote_start = ++(*i);
	if (parse_quoted_part_loop(input, i, '"') == -1)
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

static char	*handle_word_part(char *input, int *i, char *result)
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

static char	*handle_double_quote_content(char *input, int *i, char *result)
{
	while (input[*i])
	{
		if (input[*i] == '"')
		{
			result = handle_quoted_part(input, i, result);
			if (!result)
				return (NULL);
		}
		else if (is_word_char(input[*i]))
		{
			result = handle_word_part(input, i, result);
			if (!result)
				return (NULL);
		}
		else
			break;
	}
	return (result);
}

t_token	*parse_double_quote(char *input, int *i)
{
	t_token	*token;
	char	*result;

	result = NULL;
	result = handle_double_quote_content(input, i, result);
	if (!result)
		return (NULL);
	token = malloc(sizeof(t_token));
	if (!token)
	{
		free(result);
		return (NULL);
	}
	token->type = T_WORD;
	token->value = result;
	token->next = NULL;
	return (token);
}

static t_token	*handle_word_with_quote(char *input, int *i, char *word)
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

t_token	*parse_word(char *input, int *i)
{
	int		start;
	char	*word;

	start = *i;
	while (input[*i] && is_word_char(input[*i]))
		(*i)++;
	if (input[*i] && (input[*i] == '\'' || input[*i] == '"'))
	{
		word = handle_word_content(input, start, *i);
		return (handle_word_with_quote(input, i, word));
	}
	return (create_word_token(handle_word_content(input, start, *i)));
}
