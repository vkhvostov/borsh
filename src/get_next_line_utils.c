#include "../include/get_next_line.h"
#include "../libft/libft.h"


static void	copy_str(char *dest, const char *src, int offset)
{
	int	index;

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
	int		s1_len;
	int		s2_len;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	str = (char *)malloc(s1_len + s2_len + 1);
	if (str == NULL)
		return (NULL);
	copy_str(str, s1, 0);
	copy_str(str, s2, s1_len);
	str[s1_len + s2_len] = '\0';
	free((char *)s1);
	return (str);
}

char	*ft_strchr(const char *s, int c)
{
	char	searched_char;
	int		str_len;
	int		index;

	if (!s)
		return (NULL);
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
