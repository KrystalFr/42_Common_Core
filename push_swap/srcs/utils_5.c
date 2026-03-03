/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/06 22:55:01 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 02:01:16 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

t_linked	*get_smallest(t_linked *stack)
{
	t_linked	*ret;
	long		min;

	if (!stack)
		return (NULL);
	min = LONG_MAX;
	while (stack)
	{
		if (stack->data < min)
		{
			min = stack->data;
			ret = stack;
		}
		stack = stack->next;
	}
	return (ret);
}

t_linked	*get_biggest(t_linked *stack)
{
	t_linked	*ret;
	long		max;

	if (!stack)
		return (NULL);
	max = LONG_MIN;
	while (stack)
	{
		if (stack->data > max)
		{
			max = stack->data;
			ret = stack;
		}
		stack = stack->next;
	}
	return (ret);
}
