/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 00:46:32 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:28:05 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	exit_export(t_minishell *vars, t_token *token)
{
	if (!command_should_be_execute_in_the_parent(token))
		exit_minishell(vars, NULL);
}

void	init_tab(t_minishell *vars, t_env **tab)
{
	t_env	*tmp;
	int		i;

	tmp = *vars->env;
	i = 0;
	while (tmp)
	{
		tab[i] = tmp;
		tmp = tmp->next;
		i++;
	}
	tab[i] = NULL;
	tab[i + 1] = NULL;
}

void	print_sorted_env(t_minishell *vars)
{
	t_env	*tab[1024];
	t_env	*tmp;
	int		i;

	init_tab(vars, tab);
	i = 1;
	while (tab[i])
	{
		if (ft_strncmp(tab[i - 1]->data, tab[i]->data,
				ft_strlen(tab[i - 1]->data)) > 0)
		{
			tmp = tab[i - 1];
			tab[i - 1] = tab[i];
			tab[i] = tmp;
			i = 0;
		}
		i++;
	}
	i = 0;
	while (tab[i])
	{
		printf("export %s\n", tab[i]->data);
		i++;
	}
	vars->exit_value = 0;
}

void	exec_export(t_minishell *vars, t_token *token)
{
	vars->exit_value = 1;
	if (token->redirection && (token->redirection->in_fd == -1))
		return (exit_export(vars, token));
	if (!dup_redirection_out(vars, token))
	{
		printf ("dup out failed\n");
		return (exit_export(vars, token));
	}
	if (!token->executable_tokens[1])
	{
		print_sorted_env(vars);
		return (exit_export(vars, token));
	}
	handle_env_modification(vars, token);
	vars->exit_value = 0;
	return (exit_export(vars, token));
}
