#include "minishell.h"

cmd_t	*add_cmd(char *str, cmd_t *cmd)
{
	cmd_t	*cmd_1;

	cmd_1->num++;
	if (!str || !*str)
		return (NULL);
	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->arg = ft_split(str, ' ');
	cmd_1->pathname = ft_strdup(cmd_1->arg[0]);	
	cmd_1->next = NULL;
	while (cmd && cmd->next)
		cmd = cmd->next;
	if (cmd)
	{
		cmd->next = cmd_1;
		cmd_1->fd = 1;
		return (cmd_1);
	}
	cmd_1->fd = 0;
	return (cmd_1);
}

cmd_t	*del_cmd(cmd_t *cmd)
{
	cmd_t	*cmd_1;

	cmd_1->num--;
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

int	funk(cmd_t **cmd)
{
	int	fd[2];
	int	pid;

	while (*cmd->num--)
	{
		pid = fork();
		if (pid == -1)
			perror("\e[1;31mCHEXAV\e[0;0m");
		else if (pid == 0)
		{
			if (pipe(fd) == -1)
				perror(0);
			else 
			{
				dup2(fd[1], 1);
				*cmd->next->fd = fd[0];
			}
		}
		if (*cmd->fd)
			if (dup2(cmd->fd, 0) == -1)
				perror(0);
		if (!buildin(*cmd) && !exec(*cmd))
			perror("\e[1;31mSXAL\e[0;0m");
		if (fd[1])
		{
			close(fd[1]);
			fd[1] = 0;
		}
		if (*cmd->fd)
			close(*cmd->fd);
		*cmd = del_cmd(cmd);
	write(2, "\e[1;31mASKJHKAJSDASD\e[0;0m\n", 30);
	}
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
		funk(&cmd);
		free(ch);
	}
	return (0);
}
