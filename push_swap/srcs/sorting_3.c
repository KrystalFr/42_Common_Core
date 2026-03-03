/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting_3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:43:00 by krfranco          #+#    #+#             */
/*   Updated: 2024/04/11 02:00:05 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

int	get_len(t_linked **stack_a, int i)
{
	int			len;
	t_linked	*tmp;

	len = 0;
	tmp = *stack_a;
	while (tmp)
	{
		if (tmp->med == i)
			len++;
		tmp = tmp->next;
	}
	return (len);
}

void	set_index(t_linked **stack)
{
	t_linked	*tmp;
	int			median;
	int			i;

	tmp = *stack;
	i = 0;
	if (!stack)
		return ;
	median = stack_len(*stack) / 2;
	while (tmp)
	{
		tmp->index = i;
		if (i < median)
			tmp->med = true;
		else
			tmp->med = false;
		tmp = tmp->next;
		i++;
	}
}

void	set_target_a(t_linked **stack_a, t_linked **stack_b)
{
	t_linked	*tmpa;
	t_linked	*tmpb;
	t_linked	*target;
	long		best_targ;

	tmpa = *stack_a;
	while (tmpa)
	{
		best_targ = LONG_MIN;
		tmpb = *stack_b;
		while (tmpb)
		{
			if (tmpb->data < tmpa->data && tmpb->data > best_targ)
			{
				best_targ = tmpb->data;
				target = tmpb;
			}
			tmpb = tmpb->next;
		}
		if (best_targ == LONG_MIN)
			tmpa->target = get_biggest(*stack_b);
		else
			tmpa->target = target;
		tmpa = tmpa->next;
	}
}

void	set_target_b(t_linked **stack_a, t_linked **stack_b)
{
	t_linked	*tmpa;
	t_linked	*tmpb;
	t_linked	*target;
	long		best_targ;

	tmpb = *stack_b;
	while (tmpb)
	{
		best_targ = LONG_MAX;
		tmpa = *stack_a;
		while (tmpa)
		{
			if (tmpa->data > tmpb->data && tmpa->data < best_targ)
			{
				best_targ = tmpa->data;
				target = tmpa;
			}
			tmpa = tmpa->next;
		}
		if (best_targ == LONG_MAX)
			tmpb->target = get_smallest(*stack_a);
		else
			tmpb->target = target;
		tmpb = tmpb->next;
	}
}

void	get_cost_a(t_linked **stack_a, t_linked **stack_b)
{
	int			lena;
	int			lenb;
	t_linked	*tmpa;

	lena = stack_len(*stack_a);
	lenb = stack_len(*stack_b);
	tmpa = *stack_a;
	while (tmpa)
	{
		tmpa->cost = tmpa->index;
		if (!(tmpa->med))
			tmpa->cost = lena - tmpa->index;
		if (tmpa->target->med)
			tmpa->cost += tmpa->target->index;
		else
			tmpa->cost += lenb - tmpa->target->index;
		tmpa = tmpa->next;
	}
}
