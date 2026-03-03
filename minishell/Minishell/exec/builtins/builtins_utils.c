/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 06:24:00 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 15:10:08 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	is_builtin(t_token *token)
{
	if (ft_strcmp(token->token, "echo") == 0)
		return (true);
	if (ft_strcmp(token->token, "cd") == 0)
		return (true);
	if (ft_strcmp(token->token, "pwd") == 0)
		return (true);
	if (ft_strcmp(token->token, "export") == 0)
		return (true);
	if (ft_strcmp(token->token, "unset") == 0)
		return (true);
	if (ft_strcmp(token->token, "env") == 0)
		return (true);
	if (ft_strcmp(token->token, "exit") == 0)
		return (true);
	return (false);
}

void	exec_builtin(t_minishell *vars, t_token *token)
{
	if (ft_strcmp(token->token, "exit") == 0)
		exec_exit(vars, token);
	if (ft_strcmp(token->token, "echo") == 0)
		exec_echo(vars, token);
	if (ft_strcmp(token->token, "cd") == 0)
		exec_cd(vars, token);
	if (ft_strcmp(token->token, "pwd") == 0)
		exec_pwd(vars, token);
	if (ft_strcmp(token->token, "export") == 0)
		exec_export(vars, token);
	if (ft_strcmp(token->token, "unset") == 0)
		exec_unset(vars, token);
	if (ft_strcmp(token->token, "env") == 0)
		exec_env(vars, token);
}

bool	command_should_be_execute_in_the_parent(t_token *token)

{
	if (ft_strcmp(token->token, "unset") == 0)
		return (true);
	if (ft_strcmp(token->token, "export") == 0)
	{
		if (token->executable_tokens[1])
			return (true);
		return (false);
	}
	if (!ft_strcmp(token->token, "exit"))
		return (!token->next && !token->prev);
	else if (ft_strcmp(token->token, "cd") == 0)
		return (true);
	else
		return (false);
}
