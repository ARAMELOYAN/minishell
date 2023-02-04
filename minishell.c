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

int	heredoc(cmd_t *cmd, var_t *var)
{
	char 	*ch;

	var->fd = open(".heredoc", O_WRONLY| O_CREAT | O_TRUNC, 0644);
	while (1)
	{
		ch = readline("heredoc> ");
		if (!ft_strncmp(ch, var->file, ft_strlen(ch))
				&& ft_strlen(ch) == ft_strlen(var->file))
			break;
		write(var->fd, ch, ft_strlen(ch));
		write(var->fd, "\n", 1);
		free(ch);
	}
	free(ch);
	close(var->fd);
	var->fd = open(".heredoc", O_RDONLY);
	cmd->hd = 1;
	return (0);
}

int	redir_input(cmd_t *cmd, var_t *var, int i)
{
	if (i == 0)
		var->fd = open(var->file, O_RDONLY);
	else
		heredoc(cmd, var);
	free(var->file);
	if (var->fd == -1)
		return (1);
	if (cmd->fd[0] > 0)
		close(cmd->fd[0]);
	cmd->fd[0] = var->fd;
	return (0);
}

int	redir_output(cmd_t *cmd, var_t *var, int i)
{
	if (i == 0)
		var->fd = open(var->file, O_RDWR | O_CREAT | O_TRUNC, 0644);
	else
		var->fd = open(var->file, O_RDWR | O_CREAT | O_APPEND, 0644);
	free(var->file);
	if (var->fd == -1)
		return (1);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	cmd->fd[1] = var->fd;
	return (0);
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
		if (!ft_strncmp(motiv, "output", 6))
			return (redir_output(cmd, &var, i));
		else if (!ft_strncmp(motiv, "input", 6))
			return (redir_input(cmd, &var, i));
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

void	my_pipe(cmd_t *cmd, cmd_t *cmd_1)
{
	int		fd[2];

	if (pipe(fd) == -1)
		perror(0);
	if (cmd->fd[1] == 1)
		cmd->fd[1] = fd[1];
	else
		close(fd[1]);
	cmd->next = cmd_1;
	if (cmd->fd[0] == 0)
		cmd->fd[0] = fd[0];
	else
		close(fd[0]);
}

cmd_t	*add_cmd(char *str, cmd_t *cmd, var_t *var)
{
	cmd_t	*cmd_1;

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
		my_pipe(cmd, cmd_1);
	parse_str(cmd_1, str, "output", '>');
	parse_str(cmd_1, str, "input", '<');
	cmd_1->arg = ft_split(str, ' ');
	return (cmd_1);
}

void	del_heredoc()
{
	char *arg[3];

	arg[0] = ft_strdup("rm");
	arg[1] = ft_strdup(".heredoc");
	arg[2] = NULL;
	if (fork() == 0)
	{
		if (execve("/bin/rm", arg, NULL) == -1)
			perror(0);
		exit(0);
	}
	wait(0);
	free(arg[0]);
	free(arg[1]);
}

cmd_t	*del_cmd(cmd_t *cmd, var_t *var)
{
	cmd_t	*cmd_1;
	cmd_t	*cmd_2;

	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	if (cmd->hd)
		del_heredoc();
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

void	run(cmd_t *cmd, var_t *var)
{
	if (dup2(cmd->fd[1], 1) == -1)
		perror("\e[1;31mdup1\e[0;0m");
	if (dup2(cmd->fd[0], 0) == -1)
		perror("\e[1;31mdup2\e[0;0m");
	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	if (!buildin(cmd) && !exec(cmd))
		perror("\e[1;31mCommand not found\e[0;0m");
}

int	funk(cmd_t *cmd, var_t *var)
{
	int	pid;

	var->fd_input = dup(0);
	if (var->fd_input == -1)
		perror("ERROR!");
	var->fd_output= dup(1);
	if (var->fd_output == -1)
		perror("ERROR!1");
	while (cmd)
	{
		if (var->count > 1)
		{
			pid = fork();
			if (pid == -1)
				perror("\e[1;31mfork\e[0;0m");
			else if (pid == 0)
			{
				run(cmd, var);
				exit(0);
			}
		}
		else
			run(cmd, var);
	dup2(var->fd_input, 0);
	dup2(var->fd_output, 1);
	close(var->fd_input);
	close(var->fd_output);
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
		add_history(ch);
		if (!check_serror(ch, &cmd, var))
		{
			perror(strerror(errno));
			free(ch);
			continue ;
		}
		funk(cmd, var);
		while (var->count--)
			wait(NULL);
		free(ch);
	}
	free(var);
	return (0);
}
