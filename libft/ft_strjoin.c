#include "libft.h"

static void	copy_str(char *dest, const char *src, size_t offset)
{
	size_t	index;

	index = 0;
	while (src[index])
	{
		dest[offset + index] = src[index];
		index++;
	}
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	size_t	s1_len;
	size_t	s2_len;

	if (s1 == NULL || s2 == NULL)
		return (NULL);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	str = (char *)malloc(s1_len + s2_len + 1);
	if (str == NULL)
		return (NULL);
	copy_str(str, s1, 0);
	copy_str(str, s2, s1_len);
	str[s1_len + s2_len] = '\0';
	return (str);
}
