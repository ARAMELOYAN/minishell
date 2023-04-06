/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_env_echo.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 22:50:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 15:12:37 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	echom(t_cmd *cmd)
{
	t_var	var;
	int		nline;

	var.iter_i = 0;
	var.iter_j = 0;
	nline = 1;
	while (cmd->arg[++var.iter_j]
		&& !ft_strncmp(cmd->arg[var.iter_j], "-", 1))
	{
		var.iter_i = 0;
		while (cmd->arg[var.iter_j][++var.iter_i] &&
				cmd->arg[var.iter_j][var.iter_i] == 'n')
			;
		if (cmd->arg[var.iter_j][var.iter_i] != '\0')
			break ;
		else
			nline = 0;
	}
	while (cmd->arg[var.iter_j]
		&& write(1, cmd->arg[var.iter_j], ft_strlen(cmd->arg[var.iter_j])))
		if (cmd->arg[++var.iter_j])
			ft_putchar_fd(' ', 1);
	if (nline)
		ft_putchar_fd('\n', 1);
}

void	env(char **envp)
{
	while (*envp)
	{
		if (!ft_strchr(*envp, '='))
		{
			++envp;
			continue ;
		}
		ft_putstr_fd(*envp++, 1);
		ft_putstr_fd("\n", 1);
	}
}

void	print_err_arg(const char *s)
{
	ft_putstr_fd("msh: exit: ", 2);
	ft_putstr_fd((char *)s, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	exit(255);
}	

long int	msh_atoi(const char *s)
{
	unsigned long	val;
	int				i;
	int				k;

	val = 0;
	i = 0;
	k = 1;
	while (s[i] == ' ' || s[i] == '-' || s[i] == '+')
		if ((s[i++] == '+' || s[i - 1] == '-') && !ft_isdigit(s[i]))
			print_err_arg(s);
	if (i != 0 && s[i - 1] == '-')
		k = -1;
	while (ft_isdigit(s[i]))
	{
		val = (val * 10) + s[i++] - '0';
		if (val && ((val > LONG_MAX && k == 1)
				|| - val < (unsigned long)LONG_MIN))
			print_err_arg(s);
	}
	return (val * k);
}

void	exitm(t_cmd *cmd)
{
	t_var	var;
	long	val;

	printf("exit\n");
	if (cmd->arg[1])
	{
		var.iter_i = 0;
		while (cmd->arg[1][var.iter_i])
		{
			if (var.iter_i == 0 && (cmd->arg[1][0] == '-'
				|| cmd->arg[1][0] == '+'))
				;
			else if (!ft_isdigit(cmd->arg[1][var.iter_i]))
				print_err_arg(cmd->arg[1]);
			var.iter_i++;
		}
		val = msh_atoi(cmd->arg[1]);
		if (!cmd->arg[2])
			exit(val);
		ft_putstr_fd("msh: exit: too many arguments\n", 2);
		g_status = 1;
		return ;
	}
	exit(0);
}
