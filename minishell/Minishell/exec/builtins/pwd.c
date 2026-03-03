/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 06:59:42 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 18:49:45 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	exec_pwd(t_minishell *vars, t_token *token)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!dup_redirection_out(vars, token))
		exit_minishell(vars, NULL);
	if (token->redirection && (token->redirection->in_fd == -1))
		exit_minishell(vars, NULL);
	sort_tokens_and_arguments(token);
	if (token->executable_tokens[1] && token->executable_tokens[1][0] == '-')
	{
		ft_putstr_fd("Michell: pwd: no options allowed\n", 2);
		free(pwd);
		vars->exit_value = 1;
		exit_minishell(vars, NULL);
	}
	if (pwd == NULL)
	{
		perror("pwd");
		exit_minishell(vars, NULL);
	}
	ft_putstr_fd(pwd, 1);
	ft_putstr_fd("\n", 1);
	free(pwd);
	vars->exit_value = 0;
	return (close_both_pipe(vars, token), exit_minishell(vars, NULL));
}
