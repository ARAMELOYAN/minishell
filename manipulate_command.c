/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manipulate_command.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:16:21 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:21:35 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
