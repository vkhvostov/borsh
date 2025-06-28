#include "../include/get_next_line.h"
#include "../libft/libft.h"

static void	*free_lines(char **lines, int free_up)
{
	if (free_up)
		free(*lines);
	*lines = NULL;
	return (NULL);
}

static char	*extract_line(char *lines)
{
	char	*line;
	int		i;

	i = 0;
	while (lines[i] && lines[i] != '\n')
		i++;
	if (lines[i] == '\n')
		i++;
	if (i == 0)
		return (NULL);
	line = (char *)malloc(sizeof(char) * (i + 1));
	if (!line)
		return (free_lines(&lines, 0));
	i = 0;
	while (lines[i] && lines[i] != '\n')
	{
		line[i] = lines[i];
		i++;
	}
	if (lines[i] == '\n')
	{
		line[i++] = '\n';
	}
	line[i] = '\0';
	return (line);
}

static char	*save_lines(char *lines)
{
	char	*line;
	int		i;
	int		j;

	i = 0;
	while (lines[i] && lines[i] != '\n')
		i++;
	if (!lines[i])
		return (free_lines(&lines, 1));
	line = (char *)malloc(sizeof(char) * (ft_strlen(lines) - i + 1));
	if (!line)
		return (free_lines(&lines, 1));
	i++;
	j = 0;
	while (lines[i])
		line[j++] = lines[i++];
	line[j] = '\0';
	free_lines(&lines, 1);
	return (line);
}

static char	*read_and_update_lines(int fd, char **lines)
{
	char	*buffer;
	int		bytes_read;
	char	*new_line;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	bytes_read = read(fd, buffer, BUFFER_SIZE);
	while (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		new_line = ft_strjoin(*lines, buffer);
		if (!new_line)
			return (free_lines(lines, 1));
		*lines = new_line;
		if (ft_strchr(*lines, '\n'))
			break ;
		bytes_read = read(fd, buffer, BUFFER_SIZE);
	}
	free(buffer);
	if (bytes_read < 0 || (!*lines && bytes_read == 0))
		return (free_lines(lines, 1));
	return (*lines);
}

char	*get_next_line(int fd)
{
	static char	*lines[FD_MAX];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!read_and_update_lines(fd, &lines[fd]))
		return (NULL);
	line = extract_line(lines[fd]);
	lines[fd] = save_lines(lines[fd]);
	return (line);
}

/* 
#include <fcntl.h>
#include <stdio.h>
int main(int count, char *args[]) {
		if (count != 2) return -1;
		int file = open(args[1], O_RDONLY);
		char *line = get_next_line(file);
		while (line) {
			printf("%s\n", line);
			line = get_next_line(file);
		}
		return 0;
}
 */