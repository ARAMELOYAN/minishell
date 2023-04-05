/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/05 23:46:50 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 23:47:49 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	int_handler_fork(int sig)
{
	(void)sig;
	write(1, "\n", 1);
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
