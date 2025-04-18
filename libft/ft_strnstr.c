#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	needle_len;
	size_t	index;
	size_t	needle_index;

	needle_len = ft_strlen(needle);
	if (needle_len == 0)
		return ((char *)haystack);
	index = 0;
	while (haystack[index] && index < len)
	{
		if (haystack[index] == needle[0])
		{
			needle_index = 0;
			while (haystack[index + needle_index] == needle[needle_index]
				&& needle[needle_index] && index + needle_index < len)
				needle_index++;
			if (needle[needle_index] == '\0')
				return ((char *)(haystack + index));
		}
		index++;
	}
	return (NULL);
}
