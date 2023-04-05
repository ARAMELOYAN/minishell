/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aeloyan <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/23 14:52:38 by aeloyan           #+#    #+#             */
/*   Updated: 2023/04/05 20:03:24 by aeloyan          ###   ########.fr       */
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
	char		**envp;
	char 		**path_split;
	char 		*cmd_path_1;
	char		*cmd_path;
	char 		*ptr;
	char 		*file;
	char 		*file_1;
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
}	var_t;
  
void		exitm(cmd_t *cmd);
long int	msh_atoi(const char *s);
void		print_err_arg(const char *s);
void		env(char **envp);
void		echom(cmd_t *cmd);

void		export(cmd_t *cmd, char **envp);
int			valid(char *arg, char **envp);
int			search_in_env(char ***envp, char **ptr, char *arg);
int			add_envi_mej(char ***envp, char *temp, char *ptr, char *arg);
int			not_coresp(char ***envp, char *ptr, char *arg);

void		unset(cmd_t *cmd, char **e);
int			valid_identifier(char *arg);
void		print_export_formatted_env(char **env);
int			cd(cmd_t *cmd, char **envp);
void		manevr(char **envp);

int			exec(cmd_t *cmd, char **envp);
int			find_and_run(cmd_t *cmd, var_t *var, char **envp);
void		pwd(void);

void		parse_and_run(cmd_t *cmd, char *ch, var_t *var, char **envp);
int			mptyline(char *ch, var_t *var);
void		my_alloc(char **en);
void		change_shlvl(char **envp);
void		run(cmd_t *cmd, var_t *var, char **envp);
void		erease_and_return(cmd_t *cmd, var_t *var);
void		reverse_dup_standart_fd(var_t *var);
void		dup_standart_fd(var_t *var);
void		dup2pipe_and_execute(cmd_t *cmd, char **envp);
int			buildin(cmd_t *cmd, char **envp);
int			check_syntax_error(char *s, cmd_t **cmd, var_t *var);
int			bef_quo(cmd_t **cmd, char **s, quote_t **quote, var_t *var);
int			dev_cmd(char **s, cmd_t **cmd, var_t *var);
cmd_t		*del_cmd(cmd_t *cmd, var_t *var);
void		del_heredoc(cmd_t *cmd);
int			add_cmd(char *str, cmd_t **cmd, var_t *var);
void		cmd_init(cmd_t **cmd);
void		reset_empty_line(cmd_t *cmd);
void		find_dollar(char **arg, char **envp);
void		dollar_into_quote(char **ptr, char **arg, char **envp, quote_t **quote);
void		replace_dollar(char **arg, quote_t **quote, char **ptr, char **envp);
void		replace(char **arg, char **ptr, char **envp);
char		*next_word(char *str);
void		clear_quote(char **arg);
quote_t		*get_quote(char *ptr);
void		my_pipe(cmd_t *cmd, cmd_t *cmd_1);
int			parse_str(cmd_t *cmd, char *str, char *motiv, char ch);
int			syntax_error(char **ptr, char ch);
int			set_redir_error(quote_t **quote, char **ptr, char ch);
int			my_open(cmd_t *cmd, char *str, char *motiv, int red);
void		find_key(char *str, var_t *var);
char		*next_special(char *str);
int			redir_output(cmd_t *cmd, var_t *var, int i);
int			redir_input(cmd_t *cmd, var_t *var, int i);
int			heredoc(cmd_t *cmd, var_t *var);
int			create_buffer(var_t *var);
int			special(char *ch);
char		**my_split(char const *s, char c);
char	**splitting(char **mat, char *s, char c);
int			search_end(quote_t **qu, char **p_e, char *s);
int			search_word(char **p_s, char *s, char c, int i);
int			init_val(quote_t **quote, int *i, char **p_e, char *s);
void	clean(char **mat, int j);
int	rows(char *s, char c);
char		*my_min(char *p1, char *p2);
void		hd_handler(int sig);
void		handler(int sig);
void		quit_handler_fork(int sig);
void		quit_handler_exec(int sig);
void		int_handler_fork(int sig);
void		int_handler_exec(int sig);
char		*my_getenv(char **env, char *key);

#endif
