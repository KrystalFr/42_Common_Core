/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paths.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 17:29:44 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:21:58 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	is_absolute_path(t_minishell *vars, t_token *token, char *path)
{
	int	i;

	i = 0;
	ft_strlcpy(path, token->token, ft_strlen(token->token) + 1);
	while (token->token[i])
	{
		if (token->token[i] == '/')
		{
			vars->is_absolute_path = 1;
			return (true);
		}
		i++;
	}
	return (false);
}

char	*get_paths_from_env(t_minishell *vars)
{
	t_env	*tmp;

	tmp = *vars->env;
	while (tmp)
	{
		if (ft_strncmp(tmp->data, "PATH=", 5) == 0)
			return (tmp->data);
		tmp = tmp->next;
	}
	return (NULL);
}

void	determine_command_path(t_minishell *vars, char *paths_from_env,
		char *path, char *command)
{
	size_t	env_path_start;
	size_t	env_path_end;

	env_path_end = 5;
	if (vars->is_absolute_path)
	{
		if (access(path, X_OK) != -1)
			return ;
		return (perror(command), exit_minishell(vars, NULL));
	}
	while (1)
	{
		env_path_start = get_env_path_start_index(paths_from_env, env_path_end);
		if (env_path_start >= ft_strlen(paths_from_env))
			break ;
		env_path_end = get_env_path_end_index(paths_from_env, env_path_start);
		ft_strlcpy(path, paths_from_env + env_path_start, env_path_end
			- env_path_start + 1);
		ft_strlcpy(path + ft_strlen(path), "/", 2);
		ft_strlcpy(path + ft_strlen(path), command, ft_strlen(command) + 1);
		if (access(path, R_OK) != -1)
			return ;
	}
	ft_putstr_fd(command, 2);
	exit_minishell(vars, ": Command not found\n");
}

void	get_path(t_minishell *vars, t_token *token, char *path)
{
	char	*paths_from_env;

	paths_from_env = NULL;
	vars->is_absolute_path = 0;
	is_absolute_path(vars, token, path);
	if (!vars->is_absolute_path && vars->env)
	{
		paths_from_env = get_paths_from_env(vars);
		if (!paths_from_env)
			return ;
	}
	vars->exit_value = 127;
	determine_command_path(vars, paths_from_env, path, token->token);
	return ;
}
