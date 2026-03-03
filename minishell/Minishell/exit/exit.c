/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 10:58:19 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 16:40:08 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	unlink_here_doc(t_minishell *vars)
{
	t_heredoc	*tmp;

	tmp = vars->heredoc_list;
	while (tmp)
	{
		unlink(tmp->name);
		tmp = tmp->next;
	}
	vars->heredoc_list = NULL;
}

void	free_env(t_minishell *vars)
{
	t_env	*tmp;
	t_env	*tmp2;

	if (!vars->env)
		return ;
	tmp = *vars->env;
	while (tmp)
	{
		tmp2 = tmp->next;
		free(tmp->data);
		free(tmp);
		tmp = tmp2;
	}
	free(vars->env);
	vars->env = NULL;
}

void	exit_minishell(t_minishell *vars, char *error_msg)
{
	if (error_msg)
		ft_putstr_fd(error_msg, STDERR_FILENO);
	unlink_here_doc(vars);
	vars->here_doc_counter = -1;
	vars->heredoc_list = NULL;
	vars->redirection = NULL;
	vars->pid_tab = NULL;
	vars->head = NULL;
	vars->should_execute_command = 0;
	ft_free();
	if (vars->should_exit_minishell)
	{
		free_env(vars);
		exit(vars->exit_value);
	}
}
