#include "../include/borsh.h"

int	is_word_char(char c)
{
	return (c && c != '|' && c != '<' && c != '>'
		&& c != '"' && c != '\'' && c != ' ' && c != '\\');
}

void	skip_whitespace(char *input, int *i)
{
	while (input[*i] && (input[*i] == 32
			|| (input[*i] >= 9 && input[*i] <= 13)))
		(*i)++;
}

char	*empty_string(void)
{
	char	*str;

	str = malloc(1);
	if (str == NULL)
		return (NULL);
	str[0] = '\0';
	return (str);
}

void	free_shell_env(char **shell_env)
{
	int	i;

	i = 0;
	while (shell_env[i])
	{
		free(shell_env[i]);
		i++;
	}
	free(shell_env);
}

void	update_terminal_settings(bool enable_vquit)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= ECHOCTL;
	// term.c_iflag |= ICRNL;     // Translate carriage return to newline
	// term.c_iflag &= ~IXON;     // Disable XON/XOFF flow control
	// term.c_lflag |= ICANON;    // Enable canonical mode
	// term.c_lflag |= (ISIG | IEXTEN);  // Enable signals and extensions
	if (enable_vquit)
	{
		term.c_cc[VQUIT] = 0x1C;      // Enable SIGQUIT for child processes
		term.c_lflag |= ISIG;         // Enable signal generation
	}
	else
		term.c_cc[VQUIT] = _POSIX_VDISABLE;  // Disable QUIT char for shell
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
