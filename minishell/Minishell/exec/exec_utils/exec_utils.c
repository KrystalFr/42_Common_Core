/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:13:55 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 15:58:24 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	kill_child(pid_t *tab)
{
	int	i;

	i = 0;
	while (tab[i] > 0)
	{
		kill(tab[i], SIGKILL);
		i++;
	}
}

void	handle_pipe_and_fork_error(t_minishell *vars, int pipe_return_value)
{
	if (pipe_return_value == -1)
		return (exit_minishell(vars, "pipe error\n"));
	close(vars->pipe_fd[0]);
	close(vars->pipe_fd[1]);
	kill_child(vars->pid_tab);
	return (exit_minishell(vars, "fork error\n"));
}

int	do_pipe(t_minishell *vars, t_token *token)
{
	if (token->next != NULL)
	{
		if (pipe(vars->pipe_fd) == -1)
			return (-1);
	}
	return (0);
}

int	do_fork(t_minishell *vars, int i)
{
	if (!ft_strcmp((*vars->head)->token, "exit") && !(*vars->head)->next)
		return (0);
	vars->pid_tab[i] = fork();
	if (vars->pid_tab[i] == -1)
		return (-1);
	return (0);
}

// si un seul token, pas fermer le pipe car pas de pipe ouvert
// si plusieurs tokens:
// pour le premier token, fermer le pipe de lecture
// pour les tokens intermédiaires, fermer le pipe de lecture et d'écriture
// pour le dernier token, fermer le pipe d'écriture

void	close_pipe(t_minishell *vars, t_token *token)
{
	if (!token->prev && !token->next)
		return ;
	if (!token->prev)
	{
		close(vars->pipe_fd[1]);
		vars->pipe_out = vars->pipe_fd[0];
	}
	else if (token->prev && token->next)
	{
		close(vars->pipe_fd[1]);
		close(vars->pipe_out);
		vars->pipe_out = vars->pipe_fd[0];
	}
	else
		close(vars->pipe_out);
}
