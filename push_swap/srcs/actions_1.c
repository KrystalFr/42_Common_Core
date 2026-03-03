/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/06 19:58:37 by krfranco          #+#    #+#             */
/*   Updated: 2024/03/24 15:08:04 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../push_swap.h"

//swap first 2 element
void	sa_sb(t_linked **stack, char c)
{
	t_linked	*head;

	head = *stack;
	if (head != NULL && head->next != NULL)
	{
		ft_swap(&head->data, &head->next->data);
		if (c == 'a')
			ft_printf("sa\n");
		else if (c == 'b')
			ft_printf("sb\n");
		else
			return ;
	}
	else
		return ;
}

void	ss(t_linked **stack_a, t_linked **stack_b)
{
	sa_sb(stack_a, 's');
	sa_sb(stack_b, 's');
	ft_printf("ss\n");
}

//pa = first element of b sent on top of a
//pb = first element of a sent on top of b
void	pa_pb(t_linked **give, t_linked **take, char c)
{
	if (*give != NULL)
	{
		givetoplink(give, take);
		if (c == 'a')
			ft_printf("pa\n");
		else if (c == 'b')
			ft_printf("pb\n");
	}
}

//the first element become the last one
void	ra_rb(t_linked **stack, char c)
{
	if (*stack != NULL && stack_len(*stack) > 1)
	{
		moveup(stack);
		if (c == 'a')
			ft_printf("ra\n");
		else if (c == 'b')
			ft_printf("rb\n");
		else
			return ;
	}
}

void	rr(t_linked **stack_a, t_linked **stack_b)
{
	ra_rb(stack_a, 'r');
	ra_rb(stack_b, 'r');
	ft_printf("rr\n");
}
