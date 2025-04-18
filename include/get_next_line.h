#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include "../libft/libft.h"

# include <stdio.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 2048
# endif

char	*get_next_line(int fd);

// int		ft_strlen(const char *s);
// char	*ft_strjoin(char const *s1, char const *s2);
// char	*ft_strchr(const char *s, int c);

#endif
