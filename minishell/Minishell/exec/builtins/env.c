/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 07:40:39 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 14:20:09 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	env_variable_is_valid(char *str)
{
	int	equal_flag;
	int	i;

	equal_flag = 0;
	i = 0;
	if (str[i] == '=')
		return (false);
	while (str[i])
	{
		if (str[i] == '=')
			equal_flag = 1;
		else if (!ft_isalnum(str[i]) && str[i] != '_' && !equal_flag)
			return (false);
		i++;
	}
	if (!equal_flag)
		return (false);
	return (true);
}

void	exec_env(t_minishell *vars, t_token *token)
{
	t_env	*tmp;

	if (token->executable_tokens[1])
	{
		ft_putstr_fd("env: too many arguments\n", 2);
		vars->exit_value = 1;
		exit_minishell(vars, NULL);
	}
	if (!dup_redirection_out(vars, token))
		exit_minishell(vars, NULL);
	close_both_pipe(vars, token);
	if (token->redirection && (token->redirection->in_fd == -1
			|| token->redirection->out_fd == -1))
		return ;
	tmp = *vars->env;
	while (tmp)
	{
		if (tmp->data != NULL && env_variable_is_valid(tmp->data))
			printf("%s\n", tmp->data);
		tmp = tmp->next;
	}
	vars->exit_value = 0;
	exit_minishell(vars, NULL);
}
