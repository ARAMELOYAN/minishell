/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:52:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/01/29 12:46:57 by tumolabs         ###   ########.fr       */
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
# include <fcntl.h>
# include <term.h>

extern char	**environ;

typedef struct	cmd
{
	char		*pathname;
	char		**arg;
	char		**hd;
	char		**infile;
	int			fd[2];
	struct cmd	*next;
}	cmd_t;

typedef struct var
{
	DIR			*directory;
	char 		**ex_inp_spl;
	char 		**path_split;
	char 		*env_line;
	char		*cmd_path;
	char 		*cmd_path_1;
	char 		*ptr;
	char 		*file;
	char 		*file_1;
	char		buffer[200];
	int			iter_i;
	int			iter_j;
	int			count;
	int			fd;
	int			fd_input;
	int			fd_output;
}	var_t;

struct dirent	*entity;


#endif
