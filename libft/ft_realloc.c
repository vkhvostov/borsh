#include "libft.h"

char	*ft_realloc(char *ptr, size_t new_size)
{
	char	*new_ptr;
	size_t	old_size;
	size_t	index;

	if (!ptr)
		return (malloc(new_size));
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	old_size = ft_strlen(ptr);
	if (old_size >= new_size)
		old_size = new_size - 1;
	index = 0;
	while (index < old_size)
	{
		new_ptr[index] = ptr[index];
		index++;
	}
	new_ptr[index] = '\0';
	free(ptr);
	return (new_ptr);
}
