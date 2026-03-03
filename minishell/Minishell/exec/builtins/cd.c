/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 02:37:03 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 18:49:11 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*

La commande cd permet de changer de répertoire courant.



Pour ce faire, elle utilise la fonction chdir() qui prend en paramètre
le chemin du répertoire dans lequel on veut se rendre.

Si cd fonctionne correctement, elle modifie la variable d'environnement PWD
pour y insérer le nouveau chemin,
	et la variable OLDPWD pour y insérer l'ancien chemin.

*/

bool	cd_parsing(t_token *token)
{
	sort_tokens_and_arguments(token);
	if (!token->executable_tokens[1])
		return (true);
	if (token->executable_tokens[1][0] == '-')
	{
		ft_putstr_fd("Michell: cd: does not take options\n", 2);
		return (false);
	}
	if (matrix_size(token->executable_tokens) > 2)
	{
		ft_putstr_fd("Michell: cd: too many arguments\n", 2);
		return (false);
	}
	return (true);
}

t_env	*get_env_node(t_env **env, char *str)
{
	size_t	env_var_len;
	size_t	i;
	t_env	*tmp;

	tmp = *env;
	while (tmp)
	{
		env_var_len = 0;
		while (str[env_var_len] && str[env_var_len] != '=')
			env_var_len++;
		i = 0;
		while (str[i] && str[i] == tmp->data[i])
			i++;
		if (i == env_var_len)
			return (tmp);
		tmp = tmp->next;
	}
	return (NULL);
}

bool	modify_env_node_data(t_minishell *vars, t_env *node, char *new_data)
{
	if (node == NULL)
	{
		node = create_env_node(new_data);
		free(new_data);
		if (!node)
			return (false);
		add_back_env(vars->env, node);
	}
	else
	{
		free(node->data);
		node->data = new_data;
	}
	return (true);
}

void	update_pwd_env(t_minishell *vars, char *old_path, char *new_path)
{
	t_env	*tempa;
	t_env	*tempb;

	if (new_path == NULL && !handle_cd_no_arguments(vars, &new_path))
	{
		free(old_path);
		printf("No HOME found in env\n");
		return ;
	}
	if (chdir(new_path) == -1)
	{
		perror("Michell: cd");
		free(old_path);
		return ;
	}
	tempa = get_env_node(vars->env, "OLDPWD");
	if (!modify_env_node_data(vars, tempa, ft_strjoin("OLDPWD=", old_path)))
		return ;
	tempb = get_env_node(vars->env, "PWD");
	if (!modify_env_node_data(vars, tempb, ft_strjoin("PWD=", new_path)))
		return (free(tempa->data), free(tempa));
	free(old_path);
}

void	exec_cd(t_minishell *vars, t_token *token)
{
	char		*old_path;
	struct stat	buf;

	vars->exit_value = 1;
	close_both_pipe(vars, token);
	if (!cd_parsing(token))
		return ;
	if (token->redirection && (token->redirection->in_fd == -1
			|| token->redirection->in_fd == -1))
		return ;
	if (token->executable_tokens[1] && stat(token->executable_tokens[1],
			&buf) != 0)
	{
		ft_putstr_fd("Michell: cd: no such file or directory\n", 2);
		return ;
	}
	old_path = getcwd(NULL, 0);
	update_pwd_env(vars, old_path, token->executable_tokens[1]);
	vars->exit_value = 0;
}
