#include "minishell.h"

int	exec(char *str);

int main(int ac, char **av)
{
	char *cmd;
	int i;

	i = 2;
	cmd = av[1];
	while (i < ac)
	{
		cmd = ft_strjoin(cmd, " ");
		cmd = ft_strjoin(cmd, av[i++]);
	}
	if (exec(cmd) == 0)
		printf("%s\n", "HELLO");
	return (0);
}
