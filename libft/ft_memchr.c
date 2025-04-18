#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	searched_char;
	unsigned char	*str;
	size_t			index;

	searched_char = (unsigned char)c;
	str = (unsigned char *)s;
	index = 0;
	while (index < n)
	{
		if (str[index] == searched_char)
			return ((void *)(str + index));
		index++;
	}
	return (NULL);
}
