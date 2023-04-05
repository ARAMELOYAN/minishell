/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:44:12 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:06:28 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	int_handler_exec(int sig)
{
	(void)sig;
}

void	quit_handler_exec(int sig)
{
	(void)sig;
}

void	dup2pipe_and_execute(t_cmd *cmd, char **envp)
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

void	dup_standart_fd(t_var *var)
{
	var->fd_input = dup(0);
	var->fd_output = dup(1);
	var->fd_err = dup(2);
	if (var->fd_input == -1 || var->fd_output == -1 || var->fd_err == -1)
		perror("msh");
}

void	reverse_dup_standart_fd(t_var *var)
{
	dup2(var->fd_input, 0);
	dup2(var->fd_output, 1);
	dup2(var->fd_err, 2);
	close(var->fd_input);
	close(var->fd_output);
	close(var->fd_err);
}
