#include "libft.h"

static int	count_words(const char *str, char c)
{
	int	count;
	int	in_word;

	count = 0;
	in_word = 0;
	while (*str)
	{
		if (*str != c && !in_word)
		{
			count++;
			in_word = 1;
		}
		else if (*str == c)
			in_word = 0;
		str++;
	}
	return (count);
}

static char	*allocate_word(const char *str, char c)
{
	int		length;
	char	*word;
	int		index;

	length = 0;
	while (str[length] && str[length] != c)
		length++;
	word = (char *)malloc((length + 1) * sizeof(char));
	if (!word)
		return (NULL);
	index = 0;
	while (index < length)
	{
		word[index] = str[index];
		index++;
	}
	word[index] = '\0';
	return (word);
}

static void	free_result(char **result, int words_allocated)
{
	while (words_allocated >= 0)
	{
		free(result[words_allocated]);
		words_allocated--;
	}
	free(result);
}

static char	**fill_result(char **result, const char *s, char c)
{
	int	index;

	index = 0;
	while (*s)
	{
		if (*s != c)
		{
			result[index] = allocate_word(s, c);
			if (!result[index])
			{
				free_result(result, index);
				return (NULL);
			}
			index++;
			while (*s && *s != c)
				s++;
		}
		else
			s++;
	}
	result[index] = NULL;
	return (result);
}

char	**ft_split(char const *s, char c)
{
	int		word_count;
	char	**result;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	result = (char **)malloc((word_count + 1) * sizeof(char *));
	if (!result)
		return (NULL);
	return (fill_result(result, s, c));
}
