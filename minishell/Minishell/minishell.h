/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/29 00:11:08 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:38:39 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../Libft/libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <time.h>
# include <unistd.h>

/* COLORS */

# define RESET "\033[0m"
# define BLACK "\033[0;30m"
# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33m"
# define BLUE "\033[0;34m"
# define MAGENTA "\033[0;35m"
# define CYAN "\033[0;36m"
# define WHITE "\033[0;37m"

/*POLICE EN GRAS*/

# define BBLACK "\033[1;30m"
# define BRED "\033[1;31m"
# define BGREEN "\033[1;32m"
# define BYELLOW "\033[1;33m"
# define BBLUE "\033[1;34m"
# define BMAGENTA "\033[1;35m"
# define BCYAN "\033[1;36m"
# define BWHITE "\033[1;37m"

/*BACKGROUND*/

# define BG_BLACK "\033[40m"
# define BG_RED "\033[41m"
# define BG_GREEN "\033[42m"
# define BG_YELLOW "\033[43m"
# define BG_BLUE "\033[44m"
# define BG_MAGENTA "\033[45m"
# define BG_CYAN "\033[46m"
# define BG_WHITE "\033[47m"

/*CARACTERE SPECIAUX*/

# define ARROW "\u279C"

/*TOKEN TYPE*/

# define PIPE 5
# define WHITESPACE 4
# define DQUOTE 3
# define QUOTE 2
# define REDIRECTION 1
# define WORD 0
# define NO_COMMAND 99

/*REDIR TOKEN EXACT TYPE*/

# define IN_REDIR 6
# define OUT_REDIR 7
# define OUT_APPEND_REDIR 8
# define HERE_DOC 9

/*WORD TOKEN EXACT TYPE*/

# define COMMAND 10
# define OPTION 11
# define FILE 12
# define ARGUMENT 13

/*SIGNAL MODE*/

# define PROMPT_MODE 0
# define HEREDOC_MODE 1
# define EXEC_MODE 2
# define HD_C_MODE 3

/*BRICOLAGE*/

# define HD_D_ERROR "minishell: warning: here-document\
delimited by end-of-file (wanted `%s')\n"

typedef struct s_heredoc
{
	char						*name;
	int							fd;
	struct s_heredoc			*next;
}								t_heredoc;

typedef struct s_redirection
{
	int							in_fd;
	int							out_fd;
	char						*in_file;
	char						*out_file;
	int							redirection_type;
	struct s_redirection		*next;
}								t_redirection;

typedef struct s_token
{
	char						*token;
	char						*new_token;
	char						*exitval;
	int							fi;
	int							in_dquote;
	int							in_squote;
	int							final_size;
	char						**executable_tokens;
	int							token_type;
	t_redirection				*redirection;
	int							index;
	struct s_token				*prev;
	struct s_token				*next;
}								t_token;

typedef struct t_segment
{
	t_token						*start;
	t_token						*end;
	int							stop_analizing;
	t_redirection				*redirection;
}								t_segment;

typedef struct s_env
{
	char						*data;
	struct s_env				*prev;
	struct s_env				*next;
}								t_env;

typedef struct s_minishell
{
	int							ac;
	char						**av;
	t_env						**env;
	char						*input;
	t_token						**head;
	t_heredoc					*heredoc_list;
	int							here_doc_counter;
	t_redirection				*redirection;
	int							should_execute_command;
	int							pipe_fd[2];
	int							pipe_out;
	pid_t						*pid_tab;
	int							should_exit_minishell;
	int							is_absolute_path;
	int							exit_value;
}								t_minishell;

typedef struct s_lexer
{
	t_minishell					*vars;
	t_token						*new_node;
	t_token						**head;
	t_token						**command_table;
	char						*input;
	int							input_size;
	int							start;
	int							end;
}								t_lexer;

extern volatile sig_atomic_t	g_signal_state;

/*					EXIT					*/

// exit.c

/*					INIT					*/

// init.c

void		init_shell(int ac, char **av, char **env, t_minishell *vars);
void		copy_env(t_minishell *vars, char **env);
t_env		*create_env_node(char *str);
void		add_back_env(t_env **env, t_env *new);
t_minishell	*get_vars(t_minishell *vars);

/*					PARSING					*/

/*expander*/

// expander.c

void		expander(t_minishell *vars);
void		expand(t_minishell *vars);
int			clean_token(t_token *temp, t_minishell *vars);
int			is_expandable(t_token *temp, t_minishell *vars);
int			count_expand(t_token *temp, t_minishell *vars);

int			expand_exitval(t_token *temp, int i);
int			expandable_dquote(t_token *temp, int i);
int			expandable_squote(t_token *temp, int i);
int			expandable_env(t_token *temp, int i, t_minishell *vars);
int			write_env(t_minishell *vars, char *to_replace, int i,
				t_token *temp);

void		env_size(t_minishell *vars, char *to_replace, t_token *temp);
int			count_env(t_token *temp, int i, t_minishell *vars);
int			count_exitval(t_token *temp, int i);
int			count_squote(t_token *temp, int i);
int			count_dquote(t_token *temp, int i);

void		expand_delimiter(t_token *temp, t_minishell *vars);
int			is_maj(char c);
int			is_whitespace(char c);
int			is_whitedollar(char c);
void		failed_malloc(t_minishell *vars);

/*here_doc*/

// here_doc.c

int			get_here_doc(t_minishell *vars, t_token *token);
char		*get_here_doc_name(t_token *token, t_heredoc *hd,
				t_minishell *vars);
char		*generate_here_doc_name(char *name, int *size);
int			get_here_doc_data(t_token *token, t_heredoc *hd, t_minishell *vars);
void		store_here_doc(t_minishell *vars, t_heredoc *new_heredoc);

/*lexer*/

// lexer.c

void		lexer(t_minishell *vars);
void		get_tokens(t_lexer *d);
int			get_token_end_index(char *input, int start);
int			get_token_start_index(char *input, int end);
int			get_token_type(char c);

// lexer_utils.c

void		add_token_to_linked_list(t_lexer *d);
t_token		*create_token(t_minishell *vars, char *str, int token_size);
int			handle_words(char *input, int start);
int			handle_dquotes(char *input, int end);
int			handle_quotes(char *input, int end);

/*parser*/

// parser.c

void		parser(t_minishell *vars);

// automate.c

int			is_word(t_token *temp, t_minishell *vars);
int			is_redirection(t_token *temp, t_minishell *vars);
int			is_pipe(t_token *temp, t_minishell *vars);
int			parse_error(t_token *temp, t_minishell *vars);
void		check_token(t_minishell *vars);

// automate_utils.c

int			single_quote(int i, t_token *temp, t_minishell *vars);
int			double_quote(int i, t_token *temp, t_minishell *vars);
int			invalid_quotes(t_token *temp, t_minishell *vars);
int			invalid_word(t_token *temp);
void		write_tokentype(int type);

/*						EXEC					*/

/*adjust_token_for_exec*/

// adjust_token_for_exec.c

void		adjust_token_for_exec(t_minishell *vars);
void		delete_useless_tokens(t_minishell *vars);
void		determine_options_and_arguments(t_minishell *vars, t_segment **s);
void		determine_command(t_minishell *vars, t_segment *segment);
void		adjust_heredoc(t_minishell *vars);

// determine_redirection_and_file_utils.c

void		close_file(int fd);
void		open_outfile(t_redirection *redirection);
void		open_infile(t_redirection *redirection);
int			get_std_fd(char *str);
bool		is_std(char *str);

// determine_redirection_and_file.c

void		determine_redirection_and_file(t_minishell *vars, t_segment **s);
void		handle_out_redirection(t_minishell *vars, t_token *token,
				t_redirection *redirection, t_segment **segment);
void		handle_in_redirection(t_minishell *vars, t_token *token,
				t_redirection *redirection, t_segment **segment);
t_heredoc	*get_right_heredoc(t_minishell *vars, t_token *token);

// determine_token_option_and_arguments.c

t_token		*get_command_token(t_segment *s);
char		**add_str_to_tab(t_minishell *vars, char **tab, char *str);
char		**create_str_tab(t_minishell *vars, char *str);

// redirection_utils.c

void		add_redirection_to_linked_list(t_minishell *vars,
				t_redirection *redirection);
void		init_redirection(t_redirection **redirection);

// token_manipulation_utils.c

int			token_list_size(t_token *head);
int			get_redirection_token_type(t_token *token);
void		insert_token(t_token *token, t_token *token_insert);
void		delete_token(t_minishell *vars, t_token *token);
void		replace_token(t_minishell *vars, t_token *old_token,
				t_token *new_token);

// segment.c

void		create_segment_struct(t_minishell *vars, t_segment **segment);
void		determine_segment(t_minishell *vars, t_segment **s);
void		determine_segment_end(t_segment **s);
void		determine_segment_start(t_minishell *vars, t_segment **s);

// token_manipulation_utils.c

void		determine_segment_start(t_minishell *vars, t_segment **s);
int			get_redirection_token_type(t_token *token);
void		insert_token(t_token *token, t_token *token_insert);
void		delete_token(t_minishell *vars, t_token *token);
void		replace_token(t_minishell *vars, t_token *old_token,
				t_token *new_token);

/*builtins*/

// builtins_utils.c

bool		is_builtin(t_token *token);
void		exec_builtin(t_minishell *vars, t_token *token);
bool		command_should_be_execute_in_the_parent(t_token *token);

// echo.c

void		exec_echo(t_minishell *vars, t_token *token);

// env.c

void		exec_env(t_minishell *vars, t_token *token);
bool		env_variable_is_valid(char *str);

// exit.c

void		exec_exit(t_minishell *vars, t_token *token);
void		handle_no_numeric_argument(t_minishell *vars, char *str);
void		handle_exit_too_many_args(t_minishell *vars);
bool		is_number(char *str);
long		ft_atol(const char *nptr);

// export.c

void		exec_export(t_minishell *vars, t_token *token);
void		print_sorted_env(t_minishell *vars);
void		init_tab(t_minishell *vars, t_env **tab);
void		exit_export(t_minishell *vars, t_token *token);

// export_utils.c

void		handle_env_modification(t_minishell *vars, t_token *token);
void		update_env(t_env *env, char *str);
bool		export_string_exist(t_env **env, char *str);
bool		export_string_is_valid(char *str);

// pwd.c

void		exec_pwd(t_minishell *vars, t_token *token);

// unset.c

void		exec_unset(t_minishell *vars, t_token *token);
void		delete_env(t_minishell *vars, char *variable_to_unset);
// cd.c

void		exec_cd(t_minishell *vars, t_token *token);
bool		handle_cd_no_arguments(t_minishell *vars, char **new_path);
t_env		*get_env_node(t_env **env, char *str);

/*exec_utils*/

// dup_redirection.c

void		dup_redirection(t_minishell *vars, t_token *token);
void		close_both_pipe(t_minishell *vars, t_token *token);
bool		dup_redirection_out(t_minishell *vars, t_token *token);
bool		dup_redirection_in(t_minishell *vars, t_token *token);

// exec_utils.c

void		close_pipe(t_minishell *vars, t_token *token);
int			do_fork(t_minishell *vars, int i);
int			do_pipe(t_minishell *vars, t_token *token);
void		handle_pipe_and_fork_error(t_minishell *vars,
				int pipe_return_value);
void		kill_child(pid_t *tab);

// path_utils.c

size_t		get_env_path_start_index(char *paths_from_env, size_t env_path_end);
size_t		get_env_path_end_index(char *paths_from_env, size_t env_path_start);
void		get_env_tab(t_minishell *vars, char **env_tab);

// paths.c

void		get_path(t_minishell *vars, t_token *token, char *path);
void		determine_command_path(t_minishell *vars, char *paths_from_env,
				char *path, char *command);
char		*get_paths_from_env(t_minishell *vars);
bool		is_absolute_path(t_minishell *vars, t_token *token, char *path);

// sort_tokens_and_arguments.c

void		sort_tokens_and_arguments(t_token *token);
char		**sort_executable_tokens(char **tab);
void		swap_strings(char **str1, char **str2);
bool		is_option(char *str);
int			matrix_size(char **matrix);

// executor.c

void		exec(t_minishell *vars);
void		exec_tokens(t_minishell *vars);
void		exec_command(t_minishell *vars, t_token *token);
void		wait_childs(t_minishell *vars);

/*					exit					*/

void		exit_minishell(t_minishell *vars, char *error_msg);
void		free_env(t_minishell *vars);
void		unlink_here_doc(t_minishell *vars);
void		*print_segment(t_segment *s);
void		print_lc_tokens(t_token **head);

/*					signal					*/

// signal.c

void		signal_handler(int flag);

// heredoc_mode.c

void		handle_sigint_heredoc_mode(int sig);

// prompt_mode.c

void		handle_sigint_prompt_mode(int sig);

// exec_mode.c

void		handle_sigquit_exec_mode(int sig);
void		handle_sigint_exec_mode(int sig);
#endif