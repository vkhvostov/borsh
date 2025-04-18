#include "libft.h"

void	*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*pointer;

	pointer = (unsigned char *)b;
	while (len--)
	{
		*pointer = (unsigned char)c;
		pointer++;
	}
	return (b);
}
