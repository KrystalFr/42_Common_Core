/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   segment.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 15:32:48 by legoat            #+#    #+#             */
/*   Updated: 2025/02/21 22:30:09 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// Le but de ces fonctions est de determiner les segments de tokens
// exemple : << test | cat < Makefile -e > test1 < test2 > test3 | cat > test4
// les segments sont :
// << test
// cat < Makefile -e > test1 < test2 > test3
// cat > test4
// chaque segment est séparé par un pipe '|'
// La fonction determine_segment_start() determine le debut du segment.
// La fonction determine_segment_end() determine la fin du segment.

void	determine_segment_start(t_minishell *vars, t_segment **s)
{
	t_token		*start_token;
	t_segment	*segment;

	segment = *s;
	(*s)->stop_analizing = 0;
	if (segment->start == NULL)
		segment->start = *vars->head;
	else
	{
		start_token = segment->end;
		start_token = start_token->next;
		if (start_token && start_token->token_type == NO_COMMAND)
			start_token = start_token->next;
		if (!start_token)
		{
			*s = NULL;
			return ;
		}
		start_token = start_token->next;
		segment->start = start_token;
	}
}

// voir doc en haut

void	determine_segment_end(t_segment **s)
{
	t_token		*end_token;
	t_token		*temp;
	t_segment	*segment;

	segment = *s;
	if (segment == NULL)
		return ;
	end_token = segment->start;
	temp = segment->start;
	while (1)
	{
		end_token = end_token->next;
		if (end_token == NULL || end_token->token_type == PIPE)
		{
			segment->end = temp;
			return ;
		}
		temp = end_token;
	}
}

void	determine_segment(t_minishell *vars, t_segment **s)
{
	determine_segment_start(vars, s);
	determine_segment_end(s);
}

// cree la structure segment et initialise les pointeurs a NULL

void	create_segment_struct(t_minishell *vars, t_segment **segment)
{
	*segment = ft_malloc(sizeof(t_segment));
	if (!(*segment))
		exit_minishell(vars, "malloc error\n");
	(*segment)->start = NULL;
	(*segment)->end = NULL;
	(*segment)->stop_analizing = 0;
	vars->should_execute_command = 1;
}
