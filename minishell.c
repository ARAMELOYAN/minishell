/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 13:45:59 by aeloyan           #+#    #+#             */
/*   Updated: 2023/03/22 17:14:48 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

quote_t	*get_quote(char *ptr);
void	print_cmd(cmd_t *cmd);
void	clear_quote(char **arg);
void    find_dollar(char **arg);

char	*my_min(char *p1, char *p2)
{
	if (p1 && p2)
		if (p1 < p2)
			return (p1);
		else
			return (p2);
	if (!p1)
		return (p2);
	return (p1);
}

static int	rows(char *s, char c)
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

static void	clean(char **mat, int j)
{
	while (j-- > 1)
		free(mat[j - 1]);
	free(mat);
	return ;
}

static char	**init(char **mat, char *s, char c)
{
	quote_t	*quote;
	quote_t	*quote_1;
	char	*p_start;
	char	*p_end;
	int		i;
	int		j;

	j = 0;
	i = 0;
	p_start = s;
	p_end = s;
	quote = get_quote(s);
	while (s[i])
	{
		while (s[i] == c)
		{
			p_start = &s[++i];
		}
		while (s[i] != c && s[i])
		{
			p_end = &s[++i];
		}
		while (quote > 3 &&  p_end > quote->start)
		{
			p_end = my_min(ft_strchr(quote->end, ' '), ft_strchr(quote->end, '\0'));
			quote_1 = quote;
			quote = get_quote(quote->end + 1);
			free(quote_1);
			i = p_end - s;// stex kaskac ka
		}
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

char	**ft_splir(char const *s, char c)
{
	char	**mat;

	if (!s)
		return (0);
	mat = (char **)malloc(sizeof(char *) * (rows((char *)s, c) + 1));
	if (!mat)
		return (0);
	mat = init(mat, (char *)s, c);
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

int	heredoc(cmd_t *cmd, var_t *var)
{
	char 	*ch[2];

	var->fd = open(".heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ch[1] = NULL;
	while (1)
	{
		ch[0] = readline("heredoc> ");
		if (!ft_strncmp(ch[0], var->file, ft_strlen(ch[0]) + 1))
			break;
		if (var->open_dollar == 1)
			find_dollar(ch);
		write(var->fd, ch[0], ft_strlen(ch[0]));
		write(var->fd, "\n", 1);
		free(ch[0]);
	}
	free(ch[0]);
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
	if (var->redir_error)
	{
		if (cmd->fd[2] > 2)
			close(cmd->fd[2]);
		cmd->fd[2] = var->fd;
	}
	else if (cmd->fd[1] > 1)
	{
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
	if (quote > 3)
		var->open_dollar = 0;
	else
		var->open_dollar = 1;
	while (quote > 3 && var->ptr == quote->start)
	{
		var->ptr = next_special(quote->end);
		quote_1 = quote;
		quote = get_quote(quote->end + 1);
		free(quote_1);
	}
	if (quote > 3)
		free(quote);
}

int my_open(cmd_t *cmd, char *str, int i, char *motiv, int red)
{
	var_t	var;
	char	*file[2];

	var.redir_error = red;
	file[1] = NULL;
	if (str && *str)
	{
		var.file_1 = ft_strtrim(str, " \t\v\n");
		find_key(str, &var);
		ft_memmove(str, var.ptr, ft_strlen(var.ptr) + 1);
		find_key(var.file_1, &var);
		*var.ptr = '\0';
		var.file = ft_strtrim(var.file_1, " \t\v\n");
		free(var.file_1);
		file[0] = var.file;
		clear_quote(file);
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
	if (var.ptr && *(var.ptr - 1) == '2')
	{
		ft_memmove(var.ptr - 1, var.ptr, ft_strlen(var.ptr) + 1);
		if (*(--var.ptr) == '>')
			var.redir_error = 1;
		else
			var.redir_error = 0;
	}
	while (var.ptr)
	{
		ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
		if ((*(var.ptr) == ch && special(var.ptr + 1) && *(var.ptr + 1) != ' ')
				|| (*(var.ptr) != ch && special(var.ptr) && *var.ptr != ' '))
		{
			ft_putstr_fd("msh: syntax error near unexpected token `", 2);
			if (!*var.ptr)
				ft_putstr_fd("newline", 2);
			ft_putchar_fd(*var.ptr, 2);
			ft_putstr_fd("'\n", 2);
			return (0);
		}
		else if (!*(var.ptr) || !*(var.ptr + 1))
		{
			perror("SPECIAL 2");
			return (0);
		}
		if (*(var.ptr) == ch)//depq: >>
		{
			ft_memmove(var.ptr, var.ptr + 1, ft_strlen(var.ptr));
			my_open(cmd, var.ptr, 1, motiv, var.redir_error);
		}
		else
			my_open(cmd, var.ptr, 0, motiv, var.redir_error);//depq: >
		var.ptr = ft_strchr(str, ch);
	}
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
		return (2);
	quote = (quote_t *)malloc(sizeof(quote_t));
	if (!quote)
		return (1);
	quote->start = ft_strchr(ptr, '\'');
	quote->end = ft_strchr(ptr, '"');
	if ((!quote->start && !quote->end))
	{
		free(quote);
		return (0);
	}
	if (quote->start && (quote->start < quote->end || !quote->end))
		quote->end = ft_strchr(quote->start + 1, '\'');
	else
	{
		quote->start = quote->end;
		quote->end = ft_strchr(quote->start + 1, '"');
	}
	if (!quote->end)
	{
		free(quote);
		return (3);
	}
	return (quote);
}

void	clear_quote(char **arg)
{
	quote_t	*quote;
	quote_t	*quote_1;

	while (arg && *arg)
	{
		quote = get_quote(*arg);
		while (quote > 3)
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

	i = -1;
	if (*str == '?')
		return (ft_substr(str, 0, 1));
	while (ft_isalnum(str[++i]))
			;
	return (ft_substr(str, 0, i));
}

void	replace(char **arg, char **ptr)
{
	char	*temp;
	char	*word;
	char	*arg_1;
	char	*err;

	word = next_word(*ptr + 1);
	err = ft_itoa(errno);
	**ptr = '\0';
	if (*(*ptr + 1) == '?')
		arg_1 = ft_strjoin(*arg, err);
	else if (getenv(word))
		arg_1 = ft_strjoin(*arg, getenv(word));
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

void	replace_dollar(char **arg, quote_t **quote,  char **ptr)
{
	quote_t	*quote_1;

	if (ft_isalpha(*(*ptr + 1)) || *(*ptr + 1) == '?')
	{
		replace(arg, ptr);
		if (quote > 3 && *quote > 3)
		{
			free(*quote);
			*quote = get_quote(*arg);
		}
		while (quote && *quote > 3 && (*quote)->end < *ptr)
		{
			quote_1 = *quote;
			*quote = get_quote((*quote)->end + 1);
			free(quote_1);
		}
	}
	else
		*ptr = ft_strchr(*ptr + 1, '$');
}

void	find_dollar(char **arg)
{
	quote_t	*quote;
	quote_t	*quote_1;
	char	*ptr;

	while (arg && *arg)
	{
		ptr = ft_strchr(*arg, '$');
		quote = get_quote(*arg);
		while (ptr && quote > 3)
		{
			while (ptr && quote > 3 && ptr < quote->start)
				replace_dollar(arg, &quote, &ptr);
			while (ptr && quote > 3 && ptr > quote->start
					&& ptr < quote->end && *quote->start == '"')
				replace_dollar(arg, &quote, &ptr);
			if (quote > 3 && *quote->end == '\'')
			{
				ptr = ft_strchr(quote->end, '$');
				quote_1 = quote;
				quote = get_quote(quote->end + 1);
				free(quote_1);
			}
			while (ptr && quote > 3 && ptr > quote->end)
			{
				quote_1 = quote;
				quote = get_quote(quote->end + 1);
				free(quote_1);
			}
		}
		while (ptr)
			replace_dollar(arg, NULL, &ptr);
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

int	add_cmd(char *str, cmd_t **cmd, var_t *var)
{
	cmd_t	*cmd_1;

	if (!str || !*str)
		return (0);
	var->count++;
	cmd_1 = (cmd_t *)malloc(sizeof(cmd_t));
	cmd_1->next = NULL;
	cmd_1->hd = 0;
	cmd_1->fd[0] = 0;
	cmd_1->fd[1] = 1;
	cmd_1->fd[2] = 2;
	if (cmd && *cmd)
		my_pipe(*cmd, cmd_1);
	if (!parse_str(cmd_1, str, "output", '>')
		|| !parse_str(cmd_1, str, "input", '<'))
	{
		free(cmd_1);
		return (0);
	}
	cmd_1->arg = ft_splir(str, ' ');
	find_dollar(cmd_1->arg);
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

int	dev_cmd(char **s, char **ptr, cmd_t **cmd, var_t *var, quote_t *quote)
{
	char	*dst;

	if (*ptr == *s || *(++*ptr) == '|')
		return (0);
	dst = (char *)malloc(*ptr - *s);
	if (!dst)
		return (0);
	ft_strlcpy(dst, *s, *ptr - *s);
	if (add_cmd(dst, cmd, var))
	{
		free(dst);
		return (0);
	}
	free(dst);
	*s = *ptr;
	*ptr = ft_strchr(*s, '|');
}

int	check_serror(char *s, cmd_t **cmd, var_t *var)
{
	char	*ptr;
	quote_t	*quote;
	quote_t	*quote_1;
	quote_t	*quote_temp;

	*cmd = NULL;
	var->count = 0;
	ptr = ft_strchr(s, '|');
	quote = get_quote(s);
	if (quote == -1)
		return (0);
	while (quote > 3 && ptr)
	{
		while (ptr && ptr < quote->start)
			if (!dev_cmd(&s, &ptr, cmd, var, quote))
			{ 
				free(quote);
				return (0);
			}
		if (ptr)
			ptr = ft_strchr(quote->end, '|');
		quote_1 = quote;
		quote = get_quote(quote->end + 1);
		free(quote_1);
	}
	if (quote == -3)
	{
		ft_putstr_fd("msh: syntax error near unexpected token `quote'\n", 2);
		return (0);
	}
	while(ptr)
	{
		if (!dev_cmd(&s, &ptr, cmd, var, quote))
			return (0);
	}
	if (!add_cmd(s, cmd, var))
		return (0);
	return (1);
}

void	run(cmd_t *cmd, var_t *vari, char **envp)
{
	if (dup2(cmd->fd[2], 2) == -1)
		perror("msh");
	if (dup2(cmd->fd[1], 1) == -1)
		perror("msh");
	if (dup2(cmd->fd[0], 0) == -1)
		perror("msh");
	if (cmd->fd[0])
		close(cmd->fd[0]);
	if (cmd->fd[1] > 1)
		close(cmd->fd[1]);
	if (cmd->fd[2] > 2)
		close(cmd->fd[2]);
	if (!buildin(cmd, envp) && !exec(cmd, envp))
	{
		errno = 127;
		ft_putstr_fd("msh: ", 2);
		ft_putstr_fd(cmd->arg[0], 2);
		ft_putstr_fd(": command not found\n", 2);
	}
}

int	funk(cmd_t *cmd, var_t *var, char **envp)
{
	int	pid;

	var->fd_input = dup(0);
	if (var->fd_input == -1)
		perror("msh");
	var->fd_output = dup(1);
	if (var->fd_output == -1)
		perror("msh");
	var->fd_err = dup(2);
	if (var->fd_err == -1)
		perror("msh");
	while (cmd)
	{
		if (var->count > 1)
		{
			pid = fork();
			if (pid == -1)
				perror("msh");
			else if (pid == 0)
			{
				run(cmd, var, envp);
				exit(0);
			}
		}
		else
			run(cmd, var, envp);
		dup2(var->fd_input, 0);
		dup2(var->fd_output, 1);
		dup2(var->fd_err, 2);
		close(var->fd_input);
		close(var->fd_output);
		close(var->fd_err);
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

int	emptyline(char *ch)
{
	if (!ch || !*ch)
		return (1);
	while (*ch)
		if (*(ch++) != ' ')
			return (0);
	return (1);
}
int main(int ac, char **av, char **envp)
{
	char	*ch;
	cmd_t	*cmd;
	var_t	*var;

	cmd = NULL;
	my_alloc(envp);
	change_shlvl(envp);
	var = (var_t *)malloc(sizeof(var_t));
	while (1)
	{
		ch = readline("\e[0;32mminishell \e[0;0m");
		if (emptyline(ch))
		{
			free(ch);
			continue ;
		}
		add_history(ch);
		if (!check_serror(ch, &cmd, var))
		{
			free(ch);
			continue ;
		}
		funk(cmd, var, envp);
		while (var->count--)
			wait(NULL);
		free(ch);
	}
	free(var);
	return (0);
}
