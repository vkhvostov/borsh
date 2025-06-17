#include "../../include/borsh.h"

static bool	is_executable(const char *path)
{
	struct stat	path_stat;

	if (stat(path, &path_stat) == -1)
		return (false);
	if (S_ISDIR(path_stat.st_mode))
	{
		errno = EISDIR;
		return (false);
	}
	if (access(path, X_OK) == -1)
		return (false);
	return (true);
}

static char	*handle_absolute_path(char *command_name)
{
	struct stat	path_stat;
	char		*abs_path;

	if (stat(command_name, &path_stat) == -1)
		return (NULL);
	if (S_ISDIR(path_stat.st_mode))
	{
		errno = EISDIR;
		return (NULL);
	}
	if (is_executable(command_name))
	{
		abs_path = ft_strdup(command_name);
		if (abs_path == NULL)
		{
			perror("strdup failed for command_name");
			set_last_exit_status(1);
			return (NULL);
		}
		return (abs_path);
	}
	return (NULL);
}

static char	*try_path_component(const char *dir, size_t dir_len,
		const char *command_name)
{
	char	*full_path;
	size_t	len;
	size_t	cmd_len;

	if (dir_len == 0)
		return (NULL);
	cmd_len = (unsigned int)ft_strlen(command_name);
	len = (unsigned int)dir_len + 1 + cmd_len + 1;
	full_path = (char *)malloc(len);
	if (full_path == NULL)
	{
		perror("malloc for full_path failed");
		set_last_exit_status(1);
		return (NULL);
	}
	ft_memcpy(full_path, dir, dir_len);
	full_path[dir_len] = '/';
	ft_memcpy(full_path + dir_len + 1, command_name, cmd_len);
	full_path[dir_len + 1 + cmd_len] = '\0';
	if (is_executable(full_path))
		return (full_path);
	free(full_path);
	return (NULL);
}

static char	*search_in_path(char *path_env_copy, const char *command_name)
{
	char	*current_path_start;
	char	*colon;
	size_t	dir_len;
	char	*resolved_path;

	current_path_start = path_env_copy;
	resolved_path = NULL;
	while (*current_path_start && resolved_path == NULL)
	{
		colon = ft_strchr(current_path_start, ':');
		if (colon != NULL)
			dir_len = (size_t)(colon - current_path_start);
		else
			dir_len = ft_strlen(current_path_start);
		resolved_path = try_path_component(current_path_start, dir_len,
				command_name);
		if (colon != NULL)
			current_path_start = colon + 1;
		else
			break ;
	}
	return (resolved_path);
}

char	*resolve_path(char *command_name)
{
	char	*path_env;
	char	*path_env_copy;
	char	*resolved_path;

	if (command_name == NULL || *command_name == '\0')
		return (NULL);
	if (ft_strchr(command_name, '/') != NULL)
		return (handle_absolute_path(command_name));
	path_env = getenv("PATH");
	if (path_env == NULL)
		return (NULL);
	path_env_copy = ft_strdup(path_env);
	if (path_env_copy == NULL)
	{
		perror("strdup failed for PATH");
		set_last_exit_status(1);
		return (NULL);
	}
	resolved_path = search_in_path(path_env_copy, command_name);
	free(path_env_copy);
	return (resolved_path);
}
