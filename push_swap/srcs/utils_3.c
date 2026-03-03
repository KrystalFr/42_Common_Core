/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/18 19:21:03 by krfranco          #+#    #+#             */
/*   Updated: 2024/03/22 00:11:32 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

void	givetoplink(t_linked **give, t_linked **take)
{
	t_linked	*tmp;

	tmp = *give;
	*give = (*give)->next;
	if (*give)
		(*give)->prev = NULL;
	tmp->next = (*take);
	if (*take)
		(*take)->prev = tmp;
	*take = tmp;
}

void	moveup(t_linked **stack)
{
	t_linked	*tmp1;
	t_linked	*tmp2;

	tmp1 = (*stack)->next;
	tmp2 = *stack;
	while (tmp2->next)
		tmp2 = tmp2->next;
	tmp2->next = *stack;
	tmp1->prev = NULL;
	(*stack)->prev = tmp2;
	(*stack)->next = NULL;
	*stack = tmp1;
}

void	movedown(t_linked **stack)
{
	t_linked	*tmp1;
	t_linked	*tmp2;

	tmp2 = *stack;
	while (tmp2->next)
		tmp2 = tmp2->next;
	tmp1 = tmp2->prev;
	tmp2->prev = NULL;
	tmp1->next = NULL;
	(*stack)->prev = tmp2;
	tmp2->next = *stack;
	*stack = tmp2;
}

void	free_stack(t_linked **stack)
{
	t_linked	*tmp;

	while (*stack)
	{
		tmp = (*stack)->next;
		free(*stack);
		*stack = tmp;
	}
	*stack = NULL;
}

int	stack_len(t_linked *stack)
{
	int	len;

	len = 0;
	if (stack)
	{
		while (stack->next != NULL)
		{
			len++;
			stack = stack->next;
		}
		len++;
	}
	return (len);
}
