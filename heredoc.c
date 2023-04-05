/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:39:17 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:08:43 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	create_buffer(t_var *var)
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

int	heredoc(t_cmd *cmd, t_var *var)
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

void	del_heredoc(t_cmd *cmd)
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
