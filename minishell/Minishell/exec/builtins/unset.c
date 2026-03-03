/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 08:06:13 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 18:49:54 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	variable_is_in_env(char *variable_to_unset, t_env *env)
{
	int	size_of_variable;
	int	i;

	if (env == NULL)
		return (false);
	size_of_variable = 0;
	while (env->data[size_of_variable] && env->data[size_of_variable] != '=')
		size_of_variable++;
	i = 0;
	while (variable_to_unset[i])
	{
		if (variable_to_unset[i] != env->data[i])
			return (false);
		i++;
	}
	if (i == size_of_variable)
		return (true);
	return (false);
}

void	delete_env(t_minishell *vars, char *variable_to_unset)
{
	t_env	*temp;
	t_env	*temp2;

	if (variable_is_in_env(variable_to_unset, *vars->env))
	{
		temp = (*vars->env)->next;
		free((*vars->env)->data);
		free(*vars->env);
		*vars->env = temp;
	}
	else
	{
		temp = *vars->env;
		while (temp && temp->next)
		{
			if (variable_is_in_env(variable_to_unset, temp->next))
			{
				temp2 = temp->next;
				temp->next = temp->next->next;
				return (free(temp2->data), free(temp2));
			}
			temp = temp->next;
		}
	}
}

void	exec_unset(t_minishell *vars, t_token *token)
{
	int	i;

	i = 1;
	if (token->redirection && (token->redirection->in_fd == -1
			|| token->redirection->out_fd == -1))
		return (close_both_pipe(vars, token));
	while (token->executable_tokens[i])
	{
		if (token->executable_tokens[i][0] == '-')
		{
			ft_putstr_fd("unset: no option allowed\n", 2);
			close_both_pipe(vars, token);
			return ;
		}
		i++;
	}
	i = 1;
	while (token->executable_tokens[i])
	{
		delete_env(vars, token->executable_tokens[i]);
		i++;
	}
	close_both_pipe(vars, token);
	vars->exit_value = 0;
}
