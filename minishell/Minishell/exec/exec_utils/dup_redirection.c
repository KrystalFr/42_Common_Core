/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dup_redirection.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 18:49:32 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 11:18:54 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// pour debug, ft_putstr ver STDERR

bool	dup_redirection_in(t_minishell *vars, t_token *token)
{
	t_redirection	*redirection;

	redirection = token->redirection;
	if (token->prev)
		dup2(vars->pipe_out, 0);
	if (redirection)
	{
		if (redirection->in_fd == -1)
			return (false);
		if (redirection->in_fd >= 0)
		{
			open_infile(redirection);
			dup2(redirection->in_fd, STDIN_FILENO);
			close_file(redirection->in_fd);
		}
	}
	return (true);
}

// pour debug, ft_putstr_fd STDERR

bool	dup_redirection_out(t_minishell *vars, t_token *token)
{
	t_redirection	*redirection;

	redirection = token->redirection;
	if (token->next)
		dup2(vars->pipe_fd[1], 1);
	if (redirection)
	{
		if (redirection->out_fd == -1)
			return (false);
		if (redirection->out_fd >= 0)
		{
			open_outfile(redirection);
			dup2(redirection->out_fd, STDOUT_FILENO);
			close_file(redirection->out_fd);
		}
	}
	return (true);
}

void	close_both_pipe(t_minishell *vars, t_token *token)
{
	if (token->next || token->prev)
	{
		close(vars->pipe_fd[0]);
		close(vars->pipe_fd[1]);
	}
	if (token->prev)
		close(vars->pipe_out);
}

void	dup_redirection(t_minishell *vars, t_token *token)
{
	dup_redirection_in(vars, token);
	dup_redirection_out(vars, token);
	close_both_pipe(vars, token);
	if (token->token_type == NO_COMMAND)
		exit_minishell(vars, NULL);
	if (token->redirection && (token->redirection->out_fd == -1
			|| token->redirection->in_fd == -1))
		exit_minishell(vars, NULL);
	if (!token->redirection)
		return ;
}
