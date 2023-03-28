/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:52:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/03/28 12:47:16 by tumolabs         ###   ########.fr       */
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

int	g_status;

typedef struct	cmd
{
	char		**arg;
	char		**infile;
	int			fd[3];
	int			hd;
	int			arg_count;
	struct cmd	*next;
}	cmd_t;

typedef struct	quote
{
	char	*start;
	char	*end;
}	quote_t;

typedef struct	var
{
	char 		**path_split;
	char		*cmd_path;
	char 		*cmd_path_1;
	char 		*ptr;
	char 		*file;
	char 		*file_1;
	char		buffer[200];
	int			redir_error;
	int			open_dollar;
	int			iter_i;
	int			iter_j;
	int			count;
	int			fd;
	int			fd_input;
	int			fd_output;
	int			fd_err;
}	var_t;

#endif
