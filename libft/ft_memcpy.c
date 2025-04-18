#include "libft.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*destination;
	unsigned char	*source;

	if (dst == NULL && src == NULL)
		return (NULL);
	destination = (unsigned char *)dst;
	source = (unsigned char *)src;
	while (n--)
		*destination++ = *source++;
	return (dst);
}
