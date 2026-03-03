/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/24 11:54:32 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 02:03:08 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

int	is_sorted(t_linked	*stack)
{
	while (stack->next != NULL)
	{
		if (stack->next->data < stack->data)
			return (0);
		stack = stack->next;
	}
	return (1);
}

int	find_smallest(t_linked **stack)
{
	t_linked	*tmp;
	int			keep;

	tmp = *stack;
	keep = tmp->data;
	while (tmp->next != NULL)
	{
		if (tmp->next->data < keep)
			keep = tmp->next->data;
		tmp = tmp->next;
	}
	return (keep);
}

int	find_biggest(t_linked **stack)
{
	t_linked	*tmp;
	int			keep;

	tmp = *stack;
	keep = tmp->data;
	while (tmp->next != NULL)
	{
		if (tmp->next->data > keep)
			keep = tmp->next->data;
		tmp = tmp->next;
	}
	return (keep);
}

void	set_cheapest(t_linked **stack)
{
	t_linked	*tmp;
	t_linked	*cheap_link;
	long		cheapest;

	tmp = *stack;
	cheapest = LONG_MAX;
	while (tmp)
	{
		if (tmp->cost < cheapest)
		{
			cheapest = tmp->cost;
			cheap_link = tmp;
		}
		tmp = tmp->next;
	}
	cheap_link->cheap = true;
}

t_linked	*get_cheapest(t_linked **stack)
{
	t_linked	*tmp;

	tmp = *stack;
	while (tmp)
	{
		if (tmp->cheap)
			break ;
		tmp = tmp->next;
	}
	return (tmp);
}
