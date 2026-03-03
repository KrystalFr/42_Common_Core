/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/05 12:58:48 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 02:40:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

void	algo_vegan(t_linked **stack_a, t_linked **stack_b)
{
	t_linked	*stack;

	step_one(stack_a, stack_b);
	if (!is_sorted(*stack_a))
		three(stack_a);
	step_two(stack_a, stack_b);
	stack = get_smallest(*stack_a);
	if (stack->med)
	{
		while (*stack_a != stack)
			ra_rb(stack_a, 'a');
	}
	else
	{
		while (*stack_a != stack)
			rra_rrb(stack_a, 'a');
	}
}

void	step_one(t_linked **stack_a, t_linked **stack_b)
{
	while (stack_len(*stack_b) != 3)
		pa_pb(stack_a, stack_b, 'b');
	while (stack_len(*stack_a) > 3 && !is_sorted(*stack_a))
	{
		set_index(stack_a);
		set_index(stack_b);
		set_target_a(stack_a, stack_b);
		get_cost_a(stack_a, stack_b);
		set_cheapest(stack_a);
		move_a_to_b(stack_a, stack_b);
	}
}

void	move_a_to_b(t_linked **stack_a, t_linked **stack_b)
{
	t_linked	*tmp;

	tmp = get_cheapest(stack_a);
	if (tmp->med && tmp->target->med)
		rr_2(stack_a, stack_b, tmp);
	else if (!(tmp->med) && !(tmp->target->med))
		rrr_2(stack_a, stack_b, tmp);
	rotate_solo(stack_a, tmp, 'a');
	rotate_solo(stack_b, tmp->target, 'b');
	pa_pb(stack_a, stack_b, 'b');
}

void	step_two(t_linked **stack_a, t_linked **stack_b)
{
	while (*stack_b)
	{
		set_index(stack_a);
		set_index(stack_b);
		set_target_b(stack_a, stack_b);
		rotate_solo(stack_a, (*stack_b)->target, 'a');
		pa_pb(stack_b, stack_a, 'a');
	}
	set_index(stack_a);
}
