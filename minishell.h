/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:52:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/21 12:26:27 by tumolabs         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/readline.h>
# include <readline/history.h>
# include "libft/libft.h"
# include <sys/ioctl.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <termios.h>
# include <signal.h>
# include <dirent.h>
# include <unistd.h>
# include <curses.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>
# include <term.h>

extern char	**environ;

typedef struct	cmd
{
	char		*pathname;
	char		**arg;
	int			fd[2];
	struct cmd	*next;
}	cmd_t;

struct dirent	*entity;
cmd_t	*add_cmd(char *str, cmd_t *cmd);
cmd_t	*del_cmd(cmd_t *cmd);

	DIR				*directory;
	char 			**ex_inp_spl;
	char 			**path_split;
	char			buffer[200];
	char	 		*env_line;
	char 			*cmd_path;
	char 			*cmd_path_1;
	int				iter_i;
	int				iter_j;

#endif
