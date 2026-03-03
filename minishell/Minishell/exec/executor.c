/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 03:51:45 by legoat            #+#    #+#             */
/*   Updated: 2025/02/21 22:34:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	wait_childs(t_minishell *vars)
{
	int	status;
	int	i;

	i = 0;
	if (!vars->pid_tab)
		return ;
	while (vars->pid_tab[i] > -1)
	{
		waitpid(vars->pid_tab[i], &status, 0);
		if (WIFEXITED(status))
			vars->exit_value = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			vars->exit_value = 128 + WTERMSIG(status);
		else if (WIFSTOPPED(status))
			vars->exit_value = 128 + WSTOPSIG(status);
		i++;
	}
}

void	exec_command(t_minishell *vars, t_token *token)
{
	char	*env_tab[1024];
	char	path[1024];

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	vars->should_exit_minishell = 1;
	if (command_should_be_execute_in_the_parent(token))
		return (close_both_pipe(vars, token), exit_minishell(vars, NULL));
	if (is_builtin(token))
		return (exec_builtin(vars, token));
	dup_redirection(vars, token);
	sort_tokens_and_arguments(token);
	get_path(vars, token, path);
	get_env_tab(vars, env_tab);
	execve(path, token->executable_tokens, env_tab);
	if (vars->is_absolute_path == 0)
		perror(path);
	exit_minishell(vars, NULL);
}

void	exec_tokens(t_minishell *vars)
{
	t_token	*token;
	int		pipe_return_value;
	int		i;

	i = 0;
	token = *vars->head;
	while (token)
	{
		pipe_return_value = do_pipe(vars, token);
		if (pipe_return_value == -1)
			break ;
		do_fork(vars, i);
		if (vars->pid_tab[i] == -1)
			break ;
		if (command_should_be_execute_in_the_parent(token)
			&& vars->pid_tab[i] != 0)
			exec_builtin(vars, token);
		if (vars->pid_tab[i] == 0)
			exec_command(vars, token);
		close_pipe(vars, token);
		i++;
		token = token->next;
	}
	if (pipe_return_value == -1 || vars->pid_tab[i] == -1)
		handle_pipe_and_fork_error(vars, pipe_return_value);
}

// l'execution fonctionne, en 4 etapes:
// 1/ Les tokens sont nettoyes pour ne laisser que les commandes.
// 2/ Un tableau de pid est alloue pour stocker les pid des childs.
// 3/ Les tokens sont executes, les pipes sont crees, les childs sont fork.
// 4/ Les childs sont wait a la fin
// 5/ On exit proprement

// gerer < infile echo aaa | ls | < infile ls -la

void	exec(t_minishell *vars)
{
	int	*pid_tab;

	if (vars->head == NULL)
		return ;
	adjust_token_for_exec(vars);
	if (!vars->head)
		return ;
	pid_tab = ft_malloc(sizeof(pid_t) * (token_list_size(*vars->head) + 2));
	if (!pid_tab)
		return (exit_minishell(vars, "malloc error\n"));
	memset(pid_tab, -2, sizeof(pid_t) * (token_list_size(*vars->head) + 1));
	vars->pid_tab = pid_tab;
	signal_handler(EXEC_MODE);
	exec_tokens(vars);
	wait_childs(vars);
	return (exit_minishell(vars, NULL));
}
