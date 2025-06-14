#include "../../include/execution.h"

// Helper to check if a path is executable or a directory
static bool is_executable(const char *path) {
	struct stat path_stat;
	
	if (stat(path, &path_stat) == -1)
	{
		if (errno == ENOENT)
			return false;  // Keep ENOENT error
		return false;
	}
	
	if (S_ISDIR(path_stat.st_mode))
	{
		errno = EISDIR;
		return false;
	}
	
	if (access(path, X_OK) == -1)
	{
		if (errno == EACCES)
			return false;  // Keep EACCES error
		return false;
	}
	
	return true;
}

static char *handle_absolute_path(char *command_name)
{
	struct stat path_stat;
	char *abs_path;

	if (stat(command_name, &path_stat) == -1)
	{
		if (errno == ENOENT)
			return NULL;  // Keep ENOENT error
		return NULL;
	}

	if (S_ISDIR(path_stat.st_mode))
	{
		errno = EISDIR;
		return NULL;
	}

	if (is_executable(command_name))
	{
		abs_path = strdup(command_name);
		if (abs_path == NULL)
		{
			perror("strdup failed for command_name");
			set_last_exit_status(1);  // Memory error
			return (NULL);
		}
		return (abs_path);
	}
	return (NULL);
}

static char *try_path_component(const char *dir, size_t dir_len, const char *command_name)
{
	char *full_path;

	if (dir_len == 0)
		return (NULL);

	full_path = malloc(dir_len + 1 + strlen(command_name) + 1);
	if (full_path == NULL)
	{
		perror("malloc for full_path failed");
		set_last_exit_status(1);  // Memory error
		return (NULL);
	}

	strncpy(full_path, dir, dir_len);
	full_path[dir_len] = '/';
	strcpy(full_path + dir_len + 1, command_name);

	if (is_executable(full_path))
		return (full_path);

	free(full_path);
	return (NULL);
}

static char *search_in_path(char *path_env_copy, const char *command_name)
{
	char *current_path_start;
	char *colon;
	size_t dir_len;
	char *resolved_path;

	current_path_start = path_env_copy;
	resolved_path = NULL;
	while (*current_path_start && resolved_path == NULL)
	{
		colon = strchr(current_path_start, ':');
		if (colon != NULL)
			dir_len = colon - current_path_start;
		else
			dir_len = strlen(current_path_start);

		resolved_path = try_path_component(current_path_start, dir_len, command_name);
		if (colon != NULL)
			current_path_start = colon + 1;
		else
			break;
	}
	return (resolved_path);
}

char *resolve_path(char *command_name)
{
	char *path_env;
	char *path_env_copy;
	char *resolved_path;

	if (command_name == NULL || *command_name == '\0')
		return (NULL);

	if (strchr(command_name, '/') != NULL)
		return (handle_absolute_path(command_name));

	path_env = getenv("PATH");
	if (path_env == NULL)
		return (NULL);

	path_env_copy = strdup(path_env);
	if (path_env_copy == NULL)
	{
		perror("strdup failed for PATH");
		set_last_exit_status(1);  // Memory error
		return (NULL);
	}

	resolved_path = search_in_path(path_env_copy, command_name);
	free(path_env_copy);
	return (resolved_path);
}
