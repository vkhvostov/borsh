#include "../include/get_next_line.h"

static char	*free_lines(char **lines)
{
	free(*lines);
	*lines = NULL;
	return (NULL);
}

static char	*read_input(int fd, char **lines)
{
	char	*buffer;
	int		bytes_read;
	char	*current_line;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	bytes_read = read(fd, buffer, BUFFER_SIZE);
	while (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		if (!*lines)
			current_line = ft_strdup(buffer);
		else
			current_line = ft_strjoin(*lines, buffer);
		if (!current_line)
			return (free_lines(lines));
		*lines = current_line;
		if (ft_strchr(*lines, '\n'))
			break ;
		bytes_read = read(fd, buffer, BUFFER_SIZE);
	}
	free(buffer);
	if (bytes_read < 0 || (!*lines && bytes_read == 0))
		return (free_lines(lines));
	return (*lines);
}

static char	*extract_line(char *lines)
{
	int		index;
	char	*line;

	index = 0;
	while (lines[index] && lines[index] != '\n')
		index++;
	if (lines[index] == '\n')
		index++;
	if (index == 0)
		return (NULL);
	line = malloc(index + 1);
	if (!line)
		return (NULL);
	ft_strlcpy(line, lines, index + 1);
	line[index] = '\0';
	return (line);
}

static char *update_leftover_lines(char *lines)
{
	int		index;
	char	*line;

	index = 0;
	while (lines[index] && lines[index] != '\n')
		index++;
	if (lines[index] == '\n')
		index++;
	if (index == 0)
		return (NULL);
	line = ft_substr(lines, index, ft_strlen(lines));
	free_lines(&lines);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*lines;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!read_input(fd, &lines))
		return (NULL);
	line = extract_line(lines);
	lines = update_leftover_lines(lines);
	return (line);
}
