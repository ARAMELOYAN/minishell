#include "minishell.h"

int	special(char *ch)
{
	if (*ch == '|' || *ch == '&' || *ch == ';' || *ch == '<' || *ch == '>'
			|| *ch == '(' || *ch == ')' || *ch == '$' || *ch == '`'
			|| *ch == '"' || *ch == '\0')
		return (1);
	return (0);
}

char	*next_special(char *str)
{
	if (!str || !*str)
		return (0);
	while (!special((++str)))
		;
	return (str);
}

int my_open(cmd_t *cmd, char *str, int i, char *motiv)
{
	var_t	var;

	if (str && *str)
	{
		var.file_1 = ft_strtrim(str, " \t\v\n");
		var.ptr = next_special(str);
		ft_memmove(str, var.ptr, ft_strlen(var.ptr) + 1);
		var.ptr = next_special(var.file_1);
		*var.ptr = '\0';
		var.file = ft_strtrim(var.file_1, " \t\v\n");
		free(var.file_1);
		var.fd = open(var.file, O_RDWR | O_CREAT, 0644);
		free(var.file);
		if (var.fd == -1)
			return (1);
		if (!ft_strncmp(motiv, "output", 6))
			cmd->fd[1] = var.fd;
		else
			cmd->fd[0] = var.fd;
		if (i == 0)
			return (0);
		while(read(var.fd, var.buffer, 1))
			;
		return (0);
	}
}

int	parse_str(cmd_t *cmd, char *str, char *motiv, char ch)
{
	var_t	var;

	var.ptr = ft_strchr(str, ch);
	while (var.ptr)
	{
		ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
		if ((*(var.ptr) == ch && special(var.ptr + 1))
				|| (*(var.ptr) != ch && special(var.ptr)))
		{
			perror("SPECIAL 1");
			exit (1);
		}
		else if (!*(var.ptr) || !*(var.ptr + 1))
		{
			perror("SPECIAL 2");
			exit(2);
		}
		if (*(var.ptr) == ch)//depq: ls >> file
		{
			ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
			my_open(cmd, var.ptr, 1, motiv);
		}
		else
			my_open(cmd, var.ptr, 0, motiv);//depq: ls > file
		var.ptr = ft_strchr(str, ch);
	}
}

cmd_t	*add_cmd(char *str, cmd_t *cmd, var_t *var)
{
	cmd_t	*cmd_1;
	int		fd[2];

	if (!str || !*str)
		return (NULL);
	var->count++;
	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->next = NULL;
	while (cmd && cmd->next)
		cmd = cmd->next;
	cmd_1->fd[0] = 0;
	cmd_1->fd[1] = 1;
	if (cmd)
	{
		if (pipe(fd) == -1)
			perror(0);
		cmd->fd[1] = fd[1];
		cmd->next = cmd_1;
		cmd_1->fd[0] = fd[0];
	}
	printf("%s\n", str);
	parse_str(cmd_1, str, "output", '>');
	printf("%s\n", str);
	cmd_1->arg = ft_split(str, ' ');
	return (cmd_1);
}

cmd_t	*del_cmd(cmd_t *cmd, var_t *var)
{
	cmd_t	*cmd_1;

	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	cmd_1 = cmd->next;
	var->iter_i = 0;
	while (cmd->arg[var->iter_i])
		free(cmd->arg[var->iter_i++]);
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
		if (!ft_strncmp(cmd->arg[0], "cd", 2)) //&&strelen(pathname == 2)
			cd(cmd);
		else if (!ft_strncmp(cmd->arg[0], "pwd", 3))
			pwd();
		else if (!ft_strncmp(cmd->arg[0], "echo", 4))
			echom(cmd);
		else if (!ft_strncmp(cmd->arg[0], "env", 3))
			env();
		else if (!ft_strncmp(cmd->arg[0], "export", 6))
			export(cmd);
		else if (!ft_strncmp(cmd->arg[0], "unset", 5))
			unset(cmd);
		else if (!ft_strncmp(cmd->arg[0], "exit", 4))
			exitm(cmd);
		else
			return (0);
		return (1);
}

int	check_serror(char *s, cmd_t **cmd, var_t *var)
{
	char	*dst;
	char	*ptr;

	*cmd = NULL;
	var->count = 0;
	ptr = ft_strchr(s, '|');
	while(ptr)
	{
		if (ptr == s || *(++ptr) == '|')//syntax
			return (0);
		dst = (char *)malloc(ptr - s);
		if (!dst)
			return (0);
		ft_strlcpy(dst, s, ptr - s);//poxaren@ kareli e memmove ogtagorcel erevi
		if (*cmd)
			add_cmd(dst, *cmd, var);
		else
			*cmd = add_cmd(dst, *cmd, var);
		free(dst);
		s = ptr;
		ptr = ft_strchr(s, '|');
	}
	if (*cmd)
		add_cmd(s, *cmd, var);
	else
		*cmd = add_cmd(s, *cmd, var);
	return (1);
}

int	funk(cmd_t *cmd, var_t *var)
{
	int	pid;

	while (cmd)
	{
		if (var->count > 1)
			pid = fork();
		else
			pid = 0;
		if (pid == -1)
			perror("\e[1;31mfork\e[0;0m");
		else if (pid == 0)
		{
			if (dup2(cmd->fd[1], 1) == -1)
				perror("\e[1;31mdup2\e[0;0m");
			if (dup2(cmd->fd[0], 0) == -1)
				perror("\e[1;31mdup2\e[0;0m");
			if (!buildin(cmd) && !exec(cmd))
				perror("\e[1;31mCommand not found\e[0;0m");
			if (cmd->fd[0])
				close(cmd->fd[0]);
			if (cmd->fd[1] > 1)
				close(cmd->fd[1]);
			if (var->count > 1)
				exit(0);
		}
		cmd = del_cmd(cmd, var);
	}
}

int main()
{
	char	*ch;
	cmd_t	*cmd;
	var_t	*var;

	var = (var_t *)malloc(sizeof(var_t));
	while (1)
	{
		//SHLVL++;
		ch = readline("minishell$ ");
		if (!ch)
			continue ;
		if (!check_serror(ch, &cmd, var))
		{
			perror(strerror(errno));
			free(ch);
			continue ;
		}
		add_history(ch);
		funk(cmd, var);
		while (var->count--)
			wait(NULL);
		free(ch);
	}
	free(var);
	return (0);
}
