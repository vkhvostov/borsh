#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	searched_char;
	int		str_len;

	searched_char = (char)c;
	str_len = ft_strlen(s);
	while (str_len >= 0)
	{
		if (s[str_len] == searched_char)
			return ((char *)(s + str_len));
		str_len--;
	}
	return (NULL);
}
