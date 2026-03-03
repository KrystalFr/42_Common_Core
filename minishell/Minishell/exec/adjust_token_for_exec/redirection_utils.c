/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/04 14:24:57 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/08 01:20:12 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	init_redirection(t_redirection **redirection)
{
	*redirection = ft_malloc(sizeof(t_redirection));
	if (!*redirection)
		exit(1);
	(*redirection)->in_fd = -2;
	(*redirection)->out_fd = -2;
	(*redirection)->redirection_type = -1;
	(*redirection)->next = NULL;
}

void	add_redirection_to_linked_list(t_minishell *vars,
		t_redirection *redirection)
{
	t_redirection	*tmp;

	tmp = vars->redirection;
	if (!tmp)
	{
		vars->redirection = redirection;
		return ;
	}
	while (tmp->next)
		tmp = tmp->next;
	tmp->next = redirection;
}
