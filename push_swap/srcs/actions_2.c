/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/28 01:24:17 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 01:57:56 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

//the last element become the first one
void	rra_rrb(t_linked **stack, char c)
{
	if (*stack != NULL && stack_len(*stack) > 1)
	{
		movedown(stack);
		if (c == 'a')
			ft_printf("rra\n");
		else if (c == 'b')
			ft_printf("rrb\n");
		else
			return ;
	}
}

void	rrr(t_linked **stack_a, t_linked **stack_b)
{
	rra_rrb(stack_a, 'r');
	rra_rrb(stack_b, 'r');
	ft_printf("rrr\n");
}

void	rr_2(t_linked **stack_a, t_linked **stack_b, t_linked *tmp)
{
	while (*stack_a != tmp && *stack_b != tmp->target)
		rr(stack_a, stack_b);
	set_index(stack_a);
	set_index(stack_b);
}

void	rrr_2(t_linked **stack_a, t_linked **stack_b, t_linked *tmp)
{
	while (*stack_a != tmp && *stack_b != tmp->target)
		rrr(stack_a, stack_b);
	set_index(stack_a);
	set_index(stack_b);
}

void	rotate_solo(t_linked **stack, t_linked *tmp, char c)
{
	while (*stack != tmp)
	{
		if (tmp->med)
			ra_rb(stack, c);
		else
			rra_rrb(stack, c);
	}
}
