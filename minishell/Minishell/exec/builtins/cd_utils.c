/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:02:12 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/21 22:28:33 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	handle_cd_no_arguments(t_minishell *vars, char **new_path)
{
	t_env	*temp;
	int		i;

	temp = get_env_node(vars->env, "HOME");
	if (!temp)
		return (false);
	i = 4;
	while (temp->data[i])
		i++;
	*new_path = ft_malloc(sizeof(char) * (i + 1));
	if (!*new_path)
		return (false);
	ft_strlcpy(*new_path, temp->data + 5, i + 1);
	return (true);
}
