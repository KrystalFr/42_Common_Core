/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:29:33 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/15 10:10:53 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

size_t	get_env_path_start_index(char *paths_from_env, size_t env_path_end)
{
	size_t	env_path_start;

	env_path_start = env_path_end;
	while (paths_from_env[env_path_start]
		&& paths_from_env[env_path_start] == ':')
		env_path_start++;
	return (env_path_start);
}

size_t	get_env_path_end_index(char *paths_from_env, size_t env_path_start)
{
	size_t	env_path_end;

	env_path_end = env_path_start;
	while (paths_from_env[env_path_end] && paths_from_env[env_path_end] != ':')
		env_path_end++;
	return (env_path_end);
}

void	get_env_tab(t_minishell *vars, char **env_tab)
{
	t_env	*env;
	int		i;

	if (!vars->env)
	{
		env_tab = NULL;
		return ;
	}
	env = *vars->env;
	i = 0;
	while (env)
	{
		env_tab[i] = env->data;
		env = env->next;
		i++;
	}
	env_tab[i] = NULL;
}
