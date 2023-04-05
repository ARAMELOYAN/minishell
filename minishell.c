/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 13:45:59 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 20:03:22 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

quote_t	*get_quote(char *ptr);
void	print_cmd(cmd_t *cmd);
void	clear_quote(char **arg);
void	find_dollar(char **arg, char **envp);

char	*my_getenv(char **env, char *key)
{
	char	*temp;

	if (!env || !*env)
		return (0);
	while (*env)
	{
		temp = ft_strjoin(key, "=");
		if (!ft_strncmp(*env, temp, ft_strlen(temp)))
		{
			free(temp);
			return (ft_strchr(*env, '=') + 1);
		}
		free(temp);
		env++;
	}
	return (0);
}

void	int_handler_exec(int sig)
{
	(void)sig;
}

void	int_handler_fork(int sig)
{
	(void)sig;
	write(1, "\n", 1);
}

void	quit_handler_exec(int sig)
{
	(void)sig;
}

void	quit_handler_fork(int sig)
{
	(void)sig;
	g_status = 131;
	write(1, "Quit: 3\n", 8);
}

void	handler(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	hd_handler(int sig)
{
	(void)sig;
	g_status = 1200;
	write(1, "\n", 1);
	ioctl(STDIN_FILENO, TIOCSTI, "\n");
	rl_replace_line("", 0);
	rl_on_new_line();
}

char	*my_min(char *p1, char *p2)
{
	if (p1 && p2)
	{
		if (p1 < p2)
			return (p1);
		else
			return (p2);
	}
	if (!p1)
		return (p2);
	return (p1);
}

int	rows(char *s, char c)
{
	int	i;
	int	k;
	int	d;

	i = 0;
	k = 0;
	while (s[i] && c)
	{
		d = 0;
		while (s[i] == c)
			++i;
		while (s[i] != c && s[i])
		{
			i++;
			d++;
		}
		if (d)
			k++;
	}
	return (k);
}

void	clean(char **mat, int j)
{
	while (j-- > 1)
		free(mat[j - 1]);
	free(mat);
	return ;
}

int	init_val(quote_t **quote, int *i, char **p_e, char *s)
{
	*i = 0;
	*p_e = s;
	*quote = get_quote(s);
	return (0);
}

int	search_word(char **p_s, char *s, char c, int i)
{
	while (s[i] == c)
		*p_s = &s[++i];
	while (s[i] && s[i] != c)
		i++;
	return (i);
}

int	search_end(quote_t **qu, char **p_e, char *s)
{
	quote_t	*quote_1;

	*p_e = my_min(ft_strchr((*qu)->end, ' '), ft_strchr((*qu)->end, '\0'));
	quote_1 = *qu;
	*qu = get_quote((*qu)->end + 1);
	free(quote_1);
	return (*p_e - s);
}

char	**splitting(char **mat, char *s, char c)
{
	quote_t	*quote;
	char	*p_start;
	char	*p_end;
	int		i;
	int		j;

	j = init_val(&quote, &i, &p_end, s);
	p_start = s;
	while (s[i])
	{
		i = search_word(&p_start, s, c, i);
		p_end = s + i;
		while (quote > (quote_t *)3 && p_end > quote->start)
			i = search_end(&quote, &p_end, s);
		if (p_start < p_end)
			mat[j] = ft_substr(s, p_start - s, p_end - p_start);
		if (p_start < p_end && !mat[j++])
		{
			clean(mat, j);
			return (0);
		}
	}
	mat[j] = 0;
	return (mat);
}

char	**my_split(char const *s, char c)
{
	char	**mat;

	if (!s)
		return (0);
	mat = (char **)malloc(sizeof(char *) * (rows((char *)s, c) + 1));
	if (!mat)
		return (0);
	mat = splitting(mat, (char *)s, c);
	if (!mat)
		return (0);
	return (mat);
}

int	special(char *ch)
{
	if (*ch == '|' || *ch == '&' || *ch == ';' || *ch == '<' || *ch == '>'
		|| *ch == '(' || *ch == ')' || *ch == '$' || *ch == '`'
		|| *ch == '"' || *ch == '\0' || *ch == '\'' || *ch == ' ')
		return (1);
	return (0);
}

int	create_buffer(var_t *var)
{
	char	*ch[2];

	ch[1] = NULL;
	ch[0] = readline("heredoc> ");
	if (g_status == 1200)
	{
		g_status = 0;
		close(var->fd);
		var->fd = open(".heredoc", O_RDONLY | O_TRUNC);
		free(ch[0]);
		return (0);
	}
	if (!ch[0])
		return (0);
	if (!ft_strncmp(ch[0], var->file, ft_strlen(ch[0]) + 1))
	{
		free(ch[0]);
		return (0);
	}
	if (var->open_dollar == 1)
		find_dollar(ch, var->envp);
	write(var->fd, ch[0], ft_strlen(ch[0]));
	write(var->fd, "\n", 1);
	free(ch[0]);
	return (1);
}

int	heredoc(cmd_t *cmd, var_t *var)
{
	var->fd = open(".heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	signal(SIGINT, hd_handler);
	while (1)
		if (!create_buffer(var))
			break ;
	signal(SIGINT, handler);
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
	if (i > 7)
		var->fd = open(var->file, O_RDWR | O_CREAT | O_APPEND, 0644);
	else
		var->fd = open(var->file, O_RDWR | O_CREAT | O_TRUNC, 0644);
	free(var->file);
	if (var->fd == -1)
		return (1);
	if (var->redir_error)
	{
		if (cmd->fd[2] > 2)
			close(cmd->fd[2]);
		cmd->fd[2] = var->fd;
	}
	else
	{
		if (cmd->fd[1] > 1)
			close(cmd->fd[1]);
		cmd->fd[1] = var->fd;
	}
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

void	find_key(char *str, var_t *var)
{
	quote_t	*quote;
	quote_t	*quote_1;

	var->ptr = next_special(str);
	quote = get_quote(str);
	var->open_dollar = 1;
	while (quote > (quote_t *)3 && var->ptr >= quote->start)
	{
		var->open_dollar = 0;
		var->ptr = next_special(quote->end);
		quote_1 = quote;
		quote = get_quote(quote->end + 1);
		free(quote_1);
	}
	if (quote > (quote_t *)3)
		free(quote);
}

int	my_open(cmd_t *cmd, char *str, char *motiv, int red)
{
	var_t	var;
	char	*file[2];

	var.envp = NULL;
	var.redir_error = red % 2;
	file[1] = NULL;
	if (str && *str)
	{
		var.file_1 = ft_strtrim(str, " \t\v\n");
		find_key(str, &var);
		ft_memmove(str, var.ptr, ft_strlen(var.ptr) + 1);
		find_key(var.file_1, &var);
		*(var.ptr) = '\0';
		var.file = ft_strtrim(var.file_1, " \t\v\n");
		free(var.file_1);
		file[0] = var.file;
		clear_quote(file);
		if (!ft_strncmp(motiv, "output", 7))
			return (redir_output(cmd, &var, red));
		else if (!ft_strncmp(motiv, "input", 6))
			return (redir_input(cmd, &var, red));
		return (0);
	}
	return (0);
}

int	set_redir_error(quote_t **quote, char **ptr, char ch)
{
	quote_t	*quote_1;

	while (*quote > (quote_t *)3 && *ptr > (*quote)->start)
	{
		*ptr = ft_strchr((*quote)->end, ch);
		quote_1 = *quote;
		*quote = get_quote((*quote)->end + 1);
		free(quote_1);
	}
	if (*quote > (quote_t *)3)
		free(*quote);
	if (!*ptr)
		return (-1);
	if (*ptr && *(*ptr - 1) == '2')
	{
		ft_memmove(*ptr - 1, *ptr, ft_strlen(*ptr) + 1);
		if (*(--*ptr) == '>')
			return (1);
		return (0);
	}
	return (0);
}

int	syntax_error(char **ptr, char ch)
{
	ft_memmove(*ptr, *ptr + 1, ft_strlen(*ptr));
	if ((*(*ptr) == ch && special(*ptr + 1) && *(*ptr + 1) != ' '
			&& *(*ptr + 1) != '"' && *(*ptr + 1) != '\'')
		|| (*(*ptr) != ch && special(*ptr) && **ptr != ' '
			&& *(*ptr) != '"' && *(*ptr) != '\'')
		|| (**ptr == ' ' && special(*ptr + 1)
			&& *(*ptr + 1) != '"' && *(*ptr + 1) != '\''))
	{
		ft_putstr_fd("msh: syntax error near unexpected token `", 2);
		if (!**ptr)
			ft_putstr_fd("newline", 2);
		ft_putchar_fd(*(++*ptr), 2);
		ft_putstr_fd("'\n", 2);
		return (1);
	}
	else if (!*(*ptr) || !*(*ptr + 1))
	{
		perror("SPECIAL 2");
		return (1);
	}
	return (0);
}

int	parse_str(cmd_t *cmd, char *str, char *motiv, char ch)
{
	var_t	var;
	quote_t	*quote;

	var.ptr = ft_strchr(str, ch);
	quote = get_quote(str);
	while (var.ptr)
	{
		var.redir_error = set_redir_error(&quote, &var.ptr, ch);
		if (var.redir_error == -1)
			break ;
		if (syntax_error(&var.ptr, ch))
			return (0);
		if (*(var.ptr) == ch)
		{
			ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
			my_open(cmd, var.ptr, motiv, var.redir_error + 8);
		}
		else
			my_open(cmd, var.ptr, motiv, var.redir_error);
		var.ptr = ft_strchr(str, ch);
		quote = get_quote(str);
	}
	if (quote > (quote_t *)3)
		free(quote);
	return (1);
}

void	my_pipe(cmd_t *cmd, cmd_t *cmd_1)
{
	int		fd[2];

	while (cmd && cmd->next)
		cmd = cmd->next;
	cmd->next = cmd_1;
	if (pipe(fd) == -1)
		perror(0);
	if (cmd->fd[1] == 1)
		cmd->fd[1] = fd[1];
	else
		close(fd[1]);
	cmd->next = cmd_1;
	if (cmd_1->fd[0] == 0)
		cmd_1->fd[0] = fd[0];
	else
		close(fd[0]);
}

quote_t	*get_quote(char *ptr)
{
	quote_t	*quote;

	if (!ptr)
		return ((quote_t *)2);
	quote = (quote_t *)malloc(sizeof(quote_t));
	if (!quote)
		return ((quote_t *)1);
	quote->start = ft_strchr(ptr, '\'');
	quote->end = ft_strchr(ptr, '"');
	if ((quote->start || quote->end))
	{
		if (quote->start && (quote->start < quote->end || !quote->end))
			quote->end = ft_strchr(quote->start + 1, '\'');
		else
		{
			quote->start = quote->end;
			quote->end = ft_strchr(quote->start + 1, '"');
		}
		if (quote->end)
			return (quote);
		free(quote);
		return ((quote_t *)3);
	}
	free(quote);
	return ((quote_t *)0);
}

void	clear_quote(char **arg)
{
	quote_t	*quote;
	quote_t	*quote_1;

	while (arg && *arg)
	{
		quote = get_quote(*arg);
		while (quote > (quote_t *)3)
		{
			ft_memmove(quote->start, quote->start + 1, ft_strlen(quote->start));
			ft_memmove(quote->end - 1, quote->end, ft_strlen(quote->end) + 1);
			quote_1 = quote;
			quote = get_quote(quote->end - 1);
			free(quote_1);
		}
		arg++;
	}
}

char	*next_word(char *str)
{
	int	i;

	i = 0;
	if (*str == '?')
		return (ft_substr(str, 0, 1));
	while (ft_isalnum(str[i]) || str[i] == '_')
			i++;
	return (ft_substr(str, 0, i));
}

void	replace(char **arg, char **ptr, char **envp)
{
	char	*temp;
	char	*word;
	char	*arg_1;
	char	*err;

	word = next_word(*ptr + 1);
	err = ft_itoa(g_status);
	**ptr = '\0';
	if (*(*ptr + 1) == '?')
		arg_1 = ft_strjoin(*arg, err);
	else if (my_getenv(envp, word))
		arg_1 = ft_strjoin(*arg, my_getenv(envp, word));
	else
		arg_1 = ft_strdup(*arg);
	temp = ft_strjoin(arg_1, *ptr + ft_strlen(word) + 1);
	free(*arg);
	*arg = temp;
	free(err);
	free(arg_1);
	free(word);
	*ptr = ft_strchr(*arg, '$');
	while (*ptr && !ft_isalpha(*(*ptr + 1)) && *(*ptr + 1) != '?')
		*ptr = ft_strchr(*ptr + 1, '$');
}

void	replace_dollar(char **arg, quote_t **quote, char **ptr, char **envp)
{
	quote_t	*quote_1;

	if (ft_isalpha(*(*ptr + 1)) || *(*ptr + 1) == '?' || *(*ptr + 1) == '_')
	{
		replace(arg, ptr, envp);
		if (quote && *quote > (quote_t *)3)
		{
			free(*quote);
			*quote = get_quote(*arg);
		}
		while (quote && *quote > (quote_t *)3 && (*quote)->end < *ptr)
		{
			quote_1 = *quote;
			*quote = get_quote((*quote)->end + 1);
			free(quote_1);
		}
	}
	else
		*ptr = ft_strchr(*ptr + 1, '$');
}

void	dollar_into_quote(char **ptr, char **arg, char **envp, quote_t **quote)
{
	quote_t	*quote_1;

	while (*ptr && (*quote) > (quote_t *)3 && *ptr < (*quote)->start)
		replace_dollar(arg, quote, ptr, envp);
	while (*ptr && (*quote) > (quote_t *)3 && *ptr > (*quote)->start
		&& *ptr < (*quote)->end && *(*quote)->start == '"')
		replace_dollar(arg, quote, ptr, envp);
	if ((*quote) > (quote_t *)3 && *(*quote)->end == '\'')
	{
		*ptr = ft_strchr((*quote)->end, '$');
		quote_1 = (*quote);
		(*quote) = get_quote((*quote)->end + 1);
		free(quote_1);
	}
	while (*ptr && (*quote) > (quote_t *)3 && *ptr > (*quote)->end)
	{
		quote_1 = (*quote);
		(*quote) = get_quote((*quote)->end + 1);
		free(quote_1);
	}
}

void	find_dollar(char **arg, char **envp)
{
	quote_t	*quote;
	char	*ptr;

	while (arg && *arg)
	{
		ptr = ft_strchr(*arg, '$');
		quote = get_quote(*arg);
		while (ptr && quote > (quote_t *)3)
			dollar_into_quote(&ptr, arg, envp, &quote);
		while (ptr)
			replace_dollar(arg, NULL, &ptr, envp);
		if (quote > (quote_t *)3)
			free(quote);
		arg++;
	}
}

void	reset_empty_line(cmd_t *cmd)
{
	char	**temp;

	if (!cmd)
		return ;
	temp = cmd->arg;
	while (temp && *temp)
	{
		if (**temp == '\0')
			**temp = 1;
		temp++;
	}
}

void	cmd_init(cmd_t **cmd)
{
	*cmd = (cmd_t *)malloc(sizeof(cmd_t));
	(*cmd)->next = NULL;
	(*cmd)->hd = 0;
	(*cmd)->fd[0] = 0;
	(*cmd)->fd[1] = 1;
	(*cmd)->fd[2] = 2;
}

int	add_cmd(char *str, cmd_t **cmd, var_t *var)
{
	cmd_t	*cmd_1;

	if (!str || !*str)
		return (0);
	cmd_init(&cmd_1);
	var->count++;
	if (cmd && *cmd)
		my_pipe(*cmd, cmd_1);
	if (!parse_str(cmd_1, str, "input", '<')
		|| !parse_str(cmd_1, str, "output", '>'))
	{
		g_status = 258;
		free(cmd_1);
		return (0);
	}
	cmd_1->arg = my_split(str, ' ');
	find_dollar(cmd_1->arg, var->envp);
	clear_quote(cmd_1->arg);
	reset_empty_line(cmd_1);
	if (!*cmd)
		*cmd = cmd_1;
	return (1);
}

void	del_heredoc(cmd_t *cmd)
{
	char	*arg[3];

	cmd->hd = 0;
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

	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	if (cmd->fd[2] > 2)
		close(cmd->fd[2]);
	if (cmd->hd)
		del_heredoc(cmd);
	cmd_1 = cmd->next;
	var->iter_i = 0;
	while (cmd->arg[var->iter_i])
		free(cmd->arg[var->iter_i++]);
	free(cmd->arg);
	free(cmd);
	return (cmd_1);
}

int	dev_cmd(char **s, cmd_t **cmd, var_t *var)
{
	char	*dst;

	if (var->ptr == *s || *(++var->ptr) == '|')
	{
		g_status = 258;
		return (0);
	}
	dst = (char *)malloc(var->ptr - *s);
	if (!dst)
	{
		perror("msh");
		g_status = 1;
		return (0);
	}
	ft_strlcpy(dst, *s, var->ptr - *s);
	if (!add_cmd(dst, cmd, var))
	{
		free(dst);
		return (0);
	}
	free(dst);
	*s = var->ptr;
	var->ptr = ft_strchr(*s, '|');
	return (1);
}

int	bef_quo(cmd_t **cmd, char **s, quote_t **quote, var_t *var)
{
	quote_t	*quote_1;

	while (var->ptr && var->ptr < (*quote)->start)
	{
		if (!dev_cmd(s, cmd, var))
		{
			free(*quote);
			return (0);
		}
	}
	if (var->ptr)
		var->ptr = ft_strchr((*quote)->end, '|');
	quote_1 = *quote;
	*quote = get_quote((*quote)->end + 1);
	free(quote_1);
	return (1);
}

int	check_syntax_error(char *s, cmd_t **cmd, var_t *var)
{
	quote_t	*quote;

	*cmd = NULL;
	var->count = 0;
	var->ptr = ft_strchr(s, '|');
	quote = get_quote(s);
	if (quote == (quote_t *)1)
		return (0);
	while (quote > (quote_t *)3 && var->ptr)
		if (!bef_quo(cmd, &s, &quote, var))
			return (0);
	if (quote == (quote_t *)3)
	{
		ft_putstr_fd("msh: syntax error near unexpected token `quote'\n", 2);
		g_status = 258;
		return (0);
	}
	if (quote > (quote_t *)3)
		free(quote);
	while (var->ptr)
		if (!dev_cmd(&s, cmd, var))
			return (0);
	return (add_cmd(s, cmd, var));
}

int	buildin(cmd_t *cmd, char **envp)
{
	if (!cmd->arg[0])
		return (0);
	if (!ft_strncmp(cmd->arg[0], "cd", 3))
		cd(cmd, envp);
	else if (!ft_strncmp(cmd->arg[0], "pwd", 4))
		pwd();
	else if (!ft_strncmp(cmd->arg[0], "echo", 5))
		echom(cmd);
	else if (!ft_strncmp(cmd->arg[0], "env", 4))
		env(envp);
	else if (!ft_strncmp(cmd->arg[0], "export", 7))
		export(cmd, envp);
	else if (!ft_strncmp(cmd->arg[0], "unset", 6))
		unset(cmd, envp);
	else if (!ft_strncmp(cmd->arg[0], "exit", 5))
		exitm(cmd);
	else
		return (0);
	return (1);
}

void	dup2pipe_and_execute(cmd_t *cmd, char **envp)
{
	if (dup2(cmd->fd[0], 0) == -1 || dup2(cmd->fd[1], 1) == -1
		|| dup2(cmd->fd[2], 2) == -1)
	{
		perror("msh");
		g_status = 1;
	}
	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	if (cmd->fd[2] > 2)
		close(cmd->fd[2]);
	if (!buildin(cmd, envp) && !exec(cmd, envp))
	{
		g_status = 127;
		ft_putstr_fd("msh: ", 2);
		ft_putstr_fd(cmd->arg[0], 2);
		ft_putstr_fd(": command not found\n", 2);
	}
}

void	dup_standart_fd(var_t *var)
{
	var->fd_input = dup(0);
	var->fd_output = dup(1);
	var->fd_err = dup(2);
	if (var->fd_input == -1 || var->fd_output == -1 || var->fd_err == -1)
		perror("msh");
}

void	reverse_dup_standart_fd(var_t *var)
{
	dup2(var->fd_input, 0);
	dup2(var->fd_output, 1);
	dup2(var->fd_err, 2);
	close(var->fd_input);
	close(var->fd_output);
	close(var->fd_err);
}

void	erease_and_return(cmd_t *cmd, var_t *var)
{
	perror("msh: fork");
	g_status = 1;
	while (cmd)
		cmd = del_cmd(cmd, var);
	reverse_dup_standart_fd(var);
}

void	run(cmd_t *cmd, var_t *var, char **envp)
{
	dup_standart_fd(var);
	signal(SIGQUIT, quit_handler_fork);
	signal(SIGINT, int_handler_fork);
	while (cmd)
	{
		if (var->count > 1)
		{
			var->pid = fork();
			if (var->pid == -1)
				return (erease_and_return(cmd, var));
			else if (var->pid == 0)
			{
				signal(SIGQUIT, quit_handler_exec);
				signal(SIGINT, int_handler_exec);
				dup2pipe_and_execute(cmd, envp);
				exit(0);
			}
		}
		else
			dup2pipe_and_execute(cmd, envp);
		reverse_dup_standart_fd(var);
		cmd = del_cmd(cmd, var);
	}
}

void	change_shlvl(char **envp)
{
	char	*num_a;
	char	*ptr;
	int		num;

	while (*envp)
	{
		if (!ft_strncmp(*envp, "SHLVL=", 6))
		{
			ptr = ft_strchr(*envp, '=');
			num = ft_atoi(ptr + 1);
			num++;
			num_a = ft_itoa(num);
			free(*envp);
			*envp = ft_strjoin("SHLVL=", num_a);
			free(num_a);
			return ;
		}
		envp++;
	}
}

void	my_alloc(char **en)
{
	while (en && *en)
	{
		*en = ft_strdup(*(en));
		en++;
	}
}

int	emptyline(char *ch, var_t *var)
{
	if (!ch)
	{
		free(var);
		ft_putstr_fd("exit\n", 1);
		exit (g_status);
	}
	if (!*ch)
	{
		return (1);
	}
	while (*ch)
		if (*(ch++) != ' ')
			return (0);
	return (1);
}

void	parse_and_run(cmd_t *cmd, char *ch, var_t *var, char **envp)
{
	add_history(ch);
	if (check_syntax_error(ch, &cmd, var))
	{
		run(cmd, var, envp);
		while (var->count--)
			wait(NULL);
	}
}

int	main(int ac, char **av, char **envp)
{
	char	*ch;
	cmd_t	*cmd;
	var_t	*var;

	(void)ac;
	(void)av;
	cmd = NULL;
	my_alloc(envp);
	g_status = 0;
	change_shlvl(envp);
	rl_catch_signals = 0;
	rl_done = 0;
	while (1)
	{
		signal(SIGQUIT, SIG_IGN);
		signal(SIGINT, handler);
		var = (var_t *)malloc(sizeof(var_t));
		var->envp = envp;
		ch = readline("\e[0;32mminishell \e[0;0m");
		if (!emptyline(ch, var))
			parse_and_run(cmd, ch, var, envp);
		free(ch);
		free(var);
	}
	return (0);
}
