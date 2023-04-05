/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 13:45:59 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:48:04 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
