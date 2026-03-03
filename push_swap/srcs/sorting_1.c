/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting_1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 12:42:57 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/12 22:29:46 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

void	pick_sort(t_linked **stack_a, t_linked **stack_b)
{
	if (is_sorted(*stack_a) || stack_len(*stack_a) == 1)
		return ;
	else
	{
		if (stack_len(*stack_a) <= 5)
			diet_algo(stack_a, stack_b);
		else
			algo_vegan(stack_a, stack_b);
	}
}

void	diet_algo(t_linked **stack_a, t_linked **stack_b)
{
	if (stack_len(*stack_a) < 3)
		sa_sb(stack_a, 'a');
	else
	{
		while (stack_len(*stack_a) > 3 && !is_sorted(*stack_a))
		{
			set_index(stack_a);
			if (*stack_b)
				set_index(stack_b);
			put_smallest_on_top(stack_a);
			if (!is_sorted(*stack_a))
				pa_pb(stack_a, stack_b, 'b');
		}
		if (!is_sorted(*stack_a))
			three(stack_a);
		while (*stack_b)
			pa_pb(stack_b, stack_a, 'a');
	}
}

void	three(t_linked **stack_a)
{
	int	big;
	int	small;

	big = find_biggest(stack_a);
	small = find_smallest(stack_a);
	if (last_data(stack_a) == big)
	{
		sa_sb(stack_a, 'a');
		return ;
	}
	else if (last_data(stack_a) == small)
	{
		if ((*stack_a)->data == big)
			sa_sb(stack_a, 'a');
		rra_rrb(stack_a, 'a');
		return ;
	}
	else if ((*stack_a)->data == small)
	{
		rra_rrb(stack_a, 'a');
		sa_sb(stack_a, 'a');
		return ;
	}
	else if ((*stack_a)->data == big && (*stack_a)->next->data == small)
		ra_rb(stack_a, 'a');
}

int	last_data(t_linked **stack)
{
	t_linked	*tmp;

	tmp = *stack;
	while (tmp->next)
		tmp = tmp->next;
	return (tmp->data);
}

void	put_smallest_on_top(t_linked **stack_a)
{
	t_linked	*small;

	small = get_smallest(*stack_a);
	while (*stack_a != small)
	{
		if (small->med)
			ra_rb(stack_a, 'a');
		else
			rra_rrb(stack_a, 'a');
	}
}
