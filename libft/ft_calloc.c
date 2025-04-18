#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*pointer;

	pointer = malloc(count * size);
	if (pointer == NULL)
		return (NULL);
	if (count != 0 && size != 0)
		ft_bzero(pointer, count * size);
	return (pointer);
}
