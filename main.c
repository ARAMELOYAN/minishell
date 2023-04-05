/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:08:00 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:09:43 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
