#include "libft.h"

char	*ft_strdup(const char *s1)
{
	int		length;
	char	*copy;

	length = 0;
	length = ft_strlen(s1);
	copy = (char *)malloc(sizeof(char) * (length + 1));
	if (copy == NULL)
		return (NULL);
	ft_strlcpy(copy, s1, length + 1);
	return (copy);
}
