/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:52:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/06 00:04:49 by aeloyan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "readline-master/readline.h"
# include "readline-master/history.h"
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

typedef struct cmd
{
	char		**arg;
	char		**infile;
	int			fd[3];
	int			hd;
	int			arg_count;
	struct cmd	*next;
}	t_cmd;

typedef struct quote
{
	char	*start;
	char	*end;
}	t_quote;

typedef struct var
{
	char		**envp;
	char		**path_split;
	char		*cmd_path_1;
	char		*cmd_path;
	char		*ptr;
	char		*file;
	char		*file_1;
	char		buffer[200];
	int			redir_error;
	int			open_dollar;
	int			iter_i;
	int			iter_j;
	int			count;
	int			pid;
	int			fd;
	int			fd_input;
	int			fd_output;
	int			fd_err;
}	t_var;

void		exitm(t_cmd *cmd);
long int	msh_atoi(const char *s);
void		print_err_arg(const char *s);
void		env(char **envp);
void		echom(t_cmd *cmd);

void		export(t_cmd *cmd, char **envp);
int			valid(char *arg, char **envp);
int			search_in_env(char ***envp, char **ptr, char *arg);
int			add_envi_mej(char ***envp, char *temp, char *ptr, char *arg);
int			not_coresp(char ***envp, char *ptr, char *arg);

void		unset(t_cmd *cmd, char **e);
int			valid_identifier(char *arg);
void		print_export_formatted_env(char **env);
int			cd(t_cmd *cmd, char **envp);
void		manevr(char **envp);

int			exec(t_cmd *cmd, char **envp);
int			find_and_run(t_cmd *cmd, t_var *var, char **envp);
void		pwd(void);

void		parse_and_run(t_cmd *cmd, char *ch, t_var *var, char **envp);
int			emptyline(char *ch, t_var *var);
void		my_alloc(char **en);
void		change_shlvl(char **envp);
void		run(t_cmd *cmd, t_var *var, char **envp);
void		erease_and_return(t_cmd *cmd, t_var *var);
void		reverse_dup_standart_fd(t_var *var);
void		dup_standart_fd(t_var *var);
void		dup2pipe_and_execute(t_cmd *cmd, char **envp);
int			buildin(t_cmd *cmd, char **envp);
int			check_syntax_error(char *s, t_cmd **cmd, t_var *var);
int			bef_quo(t_cmd **cmd, char **s, t_quote **quote, t_var *var);
int			dev_cmd(char **s, t_cmd **cmd, t_var *var);
t_cmd		*del_cmd(t_cmd *cmd, t_var *var);
void		del_heredoc(t_cmd *cmd);
int			add_cmd(char *str, t_cmd **cmd, t_var *var);
void		cmd_init(t_cmd **cmd);
void		reset_empty_line(t_cmd *cmd);
void		find_dollar(char **arg, char **envp);
void		dollar_into_quote(char **ptr, char **arg, char **e, t_quote **q);
void		replace_dollar(char **arg, t_quote **quote, char **ptr, char **e);
void		replace(char **arg, char **ptr, char **envp);
char		*next_word(char *str);
void		clear_quote(char **arg);
t_quote		*get_quote(char *ptr);
void		my_pipe(t_cmd *cmd, t_cmd *cmd_1);
int			parse_str(t_cmd *cmd, char *str, char *motiv, char ch);
int			syntax_error(char **ptr, char ch);
int			set_redir_error(t_quote **quote, char **ptr, char ch);
int			my_open(t_cmd *cmd, char *str, char *motiv, int red);
void		find_key(char *str, t_var *var);
char		*next_special(char *str);
int			redir_output(t_cmd *cmd, t_var *var, int i);
int			redir_input(t_cmd *cmd, t_var *var, int i);
int			heredoc(t_cmd *cmd, t_var *var);
int			create_buffer(t_var *var);
int			special(char *ch);
char		**my_split(char const *s, char c);
char		**splitting(char **mat, char *s, char c);
int			search_end(t_quote **qu, char **p_e, char *s);
int			search_word(char **p_s, char *s, char c, int i);
int			init_val(t_quote **quote, int *i, char **p_e, char *s);
void		clean(char **mat, int j);
int			rows(char *s, char c);
char		*my_min(char *p1, char *p2);
void		hd_handler(int sig);
void		handler(int sig);
void		quit_handler_fork(int sig);
void		quit_handler_exec(int sig);
void		int_handler_fork(int sig);
void		int_handler_exec(int sig);
char		*my_getenv(char **env, char *key);

#endif
