/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 04:12:59 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:27:42 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	export_string_is_valid(char *str)
{
	int	equal_flag;
	int	i;

	i = 0;
	equal_flag = 0;
	if (!str || (!ft_isalpha(str[0]) && str[0] != '_'))
		return (false);
	while (str[i])
	{
		if (str[i] == '=' && !equal_flag)
			equal_flag = 1;
		else if (!equal_flag && !ft_isalnum(str[i]) && str[i] != '_')
			return (false);
		i++;
	}
	return (true);
}

bool	export_string_exist(t_env **env, char *str)
{
	size_t	env_var_len;
	size_t	i;
	t_env	*tmp;

	tmp = *env;
	i = 0;
	while (str[i] && str[i] != '=')
		i++;
	while (tmp)
	{
		env_var_len = 0;
		while (tmp->data[env_var_len] && str[env_var_len]
			&& tmp->data[env_var_len] != '='
			&& str[env_var_len] == tmp->data[env_var_len])
			env_var_len++;
		if (i == env_var_len)
		{
			free(tmp->data);
			tmp->data = ft_strdup(str);
			return (true);
		}
		tmp = tmp->next;
	}
	return (false);
}

void	handle_env_modification(t_minishell *vars, t_token *token)
{
	t_env	*tmp;
	char	**tab;
	int		i;

	tab = token->executable_tokens;
	i = 0;
	while (tab[++i])
	{
		if (export_string_is_valid(tab[i]))
		{
			if (export_string_exist(vars->env, tab[i]))
				continue ;
			else
			{
				tmp = create_env_node(tab[i]);
				if (!tmp)
					printf("malloc error, %s has not been added to the env\n",
						tab[i]);
				else
					add_back_env(vars->env, tmp);
			}
		}
		else
			printf("Michell: export: `%s': not a valid identifier\n", tab[i]);
	}
}
