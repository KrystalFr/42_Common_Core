/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_tokens_and_arguments.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 12:41:02 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/15 10:09:19 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	matrix_size(char **matrix)
{
	int	i;

	i = 0;
	while (matrix[i])
		i++;
	return (i);
}

bool	is_option(char *str)
{
	if (str[0] == '-')
		return (true);
	return (false);
}

void	swap_strings(char **str1, char **str2)
{
	char	*tmp;

	tmp = *str1;
	*str1 = *str2;
	*str2 = tmp;
}

char	**sort_executable_tokens(char **tab)
{
	int	i;

	i = 2;
	while (tab[i])
	{
		if (is_option(tab[i]) && !is_option(tab[i - 1]))
		{
			swap_strings(&tab[i], &tab[i - 1]);
			i = 1;
		}
		i++;
	}
	return (tab);
}

void	sort_tokens_and_arguments(t_token *token)
{
	int	tab_size;

	tab_size = matrix_size(token->executable_tokens);
	if (tab_size < 2 || ft_strncmp(token->token, "echo", 4) == 0)
		return ;
	token->executable_tokens = sort_executable_tokens(token->executable_tokens);
}
