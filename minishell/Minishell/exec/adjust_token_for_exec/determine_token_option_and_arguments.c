/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   determine_token_option_and_arguments.c             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:29:41 by legoat            #+#    #+#             */
/*   Updated: 2025/02/15 10:17:21 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	**create_str_tab(t_minishell *vars, char *str)
{
	char	**tab;

	tab = ft_malloc(sizeof(char *) * 2);
	if (!tab)
		exit_minishell(vars, "malloc error\n");
	tab[0] = str;
	tab[1] = NULL;
	return (tab);
}

char	**add_str_to_tab(t_minishell *vars, char **tab, char *str)
{
	char	**new_tab;
	int		i;

	i = 0;
	if (!tab)
		return (create_str_tab(vars, str));
	while (tab[i])
		i++;
	new_tab = ft_malloc(sizeof(char *) * (i + 2));
	if (!new_tab)
		exit_minishell(vars, "malloc error\n");
	i = 0;
	while (tab[i])
	{
		new_tab[i] = tab[i];
		i++;
	}
	new_tab[i] = str;
	new_tab[i + 1] = NULL;
	return (new_tab);
}

t_token	*get_command_token(t_segment *s)
{
	t_token	*token;

	token = s->start;
	while (token)
	{
		if (token->token_type == COMMAND)
			return (token);
		if (token == s->end)
			break ;
		token = token->next;
	}
	return (NULL);
}
