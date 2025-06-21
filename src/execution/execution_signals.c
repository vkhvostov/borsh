#include "../../include/borsh.h"

static void	handle_signal_status(int status, int *exit_status)
{
	if (WTERMSIG(status) == SIGINT)
	{
		*exit_status = 130;
		ft_putstr_fd("\n", STDERR_FILENO);
	}
	else if (WTERMSIG(status) == SIGQUIT)
	{
		*exit_status = 131;
		ft_putstr_fd("Quit: 3\n", STDERR_FILENO);
	}
	else
		*exit_status = 128 + WTERMSIG(status);
}

static void	handle_child_exit_status(int status, int *exit_status)
{
	if (WIFEXITED(status))
		*exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		handle_signal_status(status, exit_status);
}

static void	wait_for_child(pid_t pid, int is_last, int *exit_status)
{
	int	status;

	if (waitpid(pid, &status, 0) == -1)
	{
		ft_putstr_fd("waitpid failed\n", STDERR_FILENO);
		if (is_last)
			*exit_status = 1;
	}
	else if (is_last)
		handle_child_exit_status(status, exit_status);
}

void	wait_for_children(pid_t *pids, int cmd_idx, int *exit_status)
{
	int	i;
	int	last_cmd_idx;

	last_cmd_idx = cmd_idx - 1;
	i = 0;
	while (i < cmd_idx)
	{
		if (pids[i] > 0)
			wait_for_child(pids[i], i == last_cmd_idx, exit_status);
		i++;
	}
}
