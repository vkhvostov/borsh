#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char	*destination;
	unsigned char	*source;

	if (dst == NULL && src == NULL)
		return (NULL);
	destination = (unsigned char *)dst;
	source = (unsigned char *)src;
	if (destination < source)
	{
		while (len--)
			*destination++ = *source++;
	}
	else
	{
		destination += len;
		source += len;
		while (len--)
		{
			destination--;
			source--;
			*destination = *source;
		}
	}
	return (dst);
}
