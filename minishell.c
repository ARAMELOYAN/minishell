#include "minishell.h"

cmd_t	*add_cmd(char *str, cmd_t *cmd)
{
	cmd_t	*cmd_1;
	if (!str || !*str)
		return (NULL);
	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->arg = ft_split(str, ' ');
	cmd_1->pathname = ft_strdup(cmd_1->arg[0]);	
	cmd_1->next = NULL;
	while (cmd && cmd->next)
		cmd = cmd->next;
	if (cmd)
		cmd->next = cmd_1;
	return (cmd_1);
}

cmd_t	*del_cmd(cmd_t *cmd)
{
	cmd_t	*cmd_1;

	cmd_1 = cmd->next;
	free(cmd->pathname);
	iter_i = 0;
	while (cmd->arg[iter_i])
		free(cmd->arg[iter_i++]);
	//free(cmd->arg);
	free(cmd);
	return (cmd_1);
}

void	handler(int sig)
{
	if (sig == SIGHUP)
		write(1, "HELLO\n", 6);
	if (sig == SIGQUIT)
		write(1, "HELLO\n", 6);
	if (sig == SIGINT)
		rl_on_new_line();
}

void	sig(void)
{
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	signal(SIGHUP, handler);
}

int	buildin(cmd_t *cmd)
{
		if (!ft_strncmp(cmd->pathname, "cd", 2))
			cd(cmd);
		else if (!ft_strncmp(cmd->pathname, "pwd", 3))
			pwd();
		else if (!ft_strncmp(cmd->pathname, "echo", 4))
			echom(cmd);
		else if (!ft_strncmp(cmd->pathname, "env", 3))
			env();
		else if (!ft_strncmp(cmd->pathname, "export", 6))
			export(cmd);
		else if (!ft_strncmp(cmd->pathname, "unset", 5))
			unset(cmd);
		else if (!ft_strncmp(cmd->pathname, "exit", 4))
			exitm(cmd);
		else
			return (0);
		return (1);
}

int	check_serror(char *s, cmd_t **cmd)
{
	char	*dst;
	char	*ptr;

	*cmd = NULL;
	ptr = ft_strchr(s, '|');
	while(ptr)
	{
		if (ptr == s || *(++ptr) == '|')
			return (0);
		dst = (char *)malloc(ptr - s);
		if (!dst)
			return (0);
		ft_strlcpy(dst, s, ptr - s);//poxaren@ kareli e memmove ogtagorcel erevi
		printf("%s\n", dst);
		if (*cmd)
			add_cmd(dst, *cmd);
		else
			*cmd = add_cmd(dst, *cmd);
		free(dst);
		s = ptr;
		ptr = ft_strchr(s, '|');
	}
	if (*cmd)
		add_cmd(s, *cmd);
	else
		*cmd = add_cmd(s, *cmd);
	return (1);
}

int main()
{
	char	*ch;
	cmd_t	*cmd;

	sig();
	while (1)
	{
		//SHLVL++;

		ch = readline("minishell$ ");
		if (!ch)
			continue;
		if (!check_serror(ch, &cmd))
		{
			errno = 258;
			perror(strerror(errno));
			free(ch);
			continue ;
		}
		add_history(ch);
		while (cmd)
		{
			if (!buildin(cmd) && !exec(cmd))
				perror(0);
			cmd = cmd->next;
		}
		free(ch);
	}
	return (0);
}
