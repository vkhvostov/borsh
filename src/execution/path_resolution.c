#include "../../include/execution.h"

// Helper to check if a path is executable
static bool is_executable(const char *path) {
	return access(path, X_OK) == 0;
}

char *resolve_path(char *command_name) {
	if (command_name == NULL || *command_name == '\0') {
		return NULL;
	}

	// If command_name contains a '/', it's an absolute or relative path
	if (strchr(command_name, '/') != NULL) {
		if (is_executable(command_name)) {
			char *abs_path = strdup(command_name);
			if (abs_path == NULL) {
				perror("strdup failed for command_name");
			}
			return abs_path;
		} else {
			return NULL; // Not found or not executable
		}
	}

	// Command does not contain '/', search in PATH
	char *path_env = getenv("PATH");
	if (path_env == NULL) {
		return NULL; // No PATH variable set
	}

	char *path_env_copy = strdup(path_env);
	if (path_env_copy == NULL) {
		perror("strdup failed for PATH");
		return NULL;
	}

	char *resolved_path_str = NULL;

	// Using strtok_r for safety, though simple strtok would also work here
	// as we are processing a copy.
	// However, strtok modifies its first argument.
	// A manual loop might be cleaner for some.
	
	// Let's use a manual parsing loop to avoid strtok complexities with freeing.
	// We iterate through path_env_copy using pointers.
	
	char *current_path_start = path_env_copy;
	while (*current_path_start && resolved_path_str == NULL) {
		char *colon = strchr(current_path_start, ':');
		size_t dir_len;
		if (colon != NULL) {
			dir_len = colon - current_path_start;
		} else {
			dir_len = strlen(current_path_start); // Last directory in PATH
		}

		if (dir_len == 0) { // Empty path component (e.g., PATH=":/bin" or PATH="/bin::/usr/bin")
						// This implies current directory "."
			// For security reasons, many shells handle "" in PATH explicitly or only if "." is there.
			// Let's assume for now an empty path component means current dir if not handled by shell setup.
			// Or, more simply, we can skip empty dir_len.
			// For now, let's construct path with "." if dir_len is 0
			// dir_len = 1; // for "."
			// char *dir = "."; // This is complex. Let's skip empty dir_len for now.
			if (dir_len == 0) {
				current_path_start = (colon != NULL) ? colon + 1 : ""; // Move to next part
				continue;
			}
		}
		
		// Allocate space for dir/command_name + null terminator + slash
		char *full_path = malloc(dir_len + 1 + strlen(command_name) + 1);
		if (full_path == NULL) {
			perror("malloc for full_path failed");
			break; // Critical memory error
		}

		strncpy(full_path, current_path_start, dir_len);
		full_path[dir_len] = '/';
		strcpy(full_path + dir_len + 1, command_name);

		if (is_executable(full_path)) {
			resolved_path_str = full_path; // full_path is already malloc'd
										// No need for another strdup here.
		} else {
			free(full_path);
		}
		
		if (colon != NULL) {
			current_path_start = colon + 1;
		} else {
			break; // End of PATH string
		}
	}

	free(path_env_copy);
	return resolved_path_str;
}
