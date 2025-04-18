#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	searched_char;
	int		str_len;
	int		index;

	searched_char = (char)c;
	str_len = ft_strlen(s);
	index = 0;
	while (index <= str_len)
	{
		if (s[index] == searched_char)
			return ((char *)(s + index));
		index++;
	}
	return (NULL);
}
