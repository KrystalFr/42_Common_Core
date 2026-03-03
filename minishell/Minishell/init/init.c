/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 11:06:57 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 16:39:27 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*create_env_node(char *env)
{
	t_env	*new;

	new = malloc(sizeof(t_env));
	if (!new)
		return (NULL);
	new->data = NULL;
	new->data = ft_strdup(env);
	if (!new->data)
		return (free(new), NULL);
	new->prev = NULL;
	new->next = NULL;
	return (new);
}

void	add_back_env(t_env **env, t_env *new)
{
	t_env	*tmp;

	if (!env || !new)
		return ;
	if (!*env)
	{
		*env = new;
		return ;
	}
	tmp = *env;
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = new;
	new->prev = tmp;
}

void	copy_env(t_minishell *vars, char **env)
{
	t_env	*new;
	int		i;

	vars->env = malloc(sizeof(t_env *));
	if (!vars->env)
		return ;
	*vars->env = NULL;
	i = 0;
	while (env[i])
	{
		new = create_env_node(env[i]);
		if (!new)
		{
			ft_putstr_fd("Malloc failed can't init env\n", 2);
			free_env(vars);
			return ;
		}
		add_back_env(vars->env, new);
		i++;
	}
}

t_minishell	*get_vars(t_minishell *vars)
{
	static t_minishell	*vars_static = NULL;

	if (vars == NULL)
		return (vars_static);
	vars_static = vars;
	return (vars_static);
}

void	init_shell(int ac, char **av, char **env, t_minishell *vars)
{
	vars->ac = ac;
	vars->av = av;
	vars->here_doc_counter = -1;
	vars->exit_value = 1;
	vars->heredoc_list = NULL;
	vars->redirection = NULL;
	vars->pid_tab = NULL;
	vars->should_execute_command = 0;
	vars->should_exit_minishell = 1;
	copy_env(vars, env);
	if (!vars->env)
	{
		ft_putstr_fd("Malloc failed can't init env\n", 2);
		exit_minishell(vars, NULL);
	}
	vars->should_exit_minishell = 0;
	g_signal_state = 0;
	get_vars(vars);
}
