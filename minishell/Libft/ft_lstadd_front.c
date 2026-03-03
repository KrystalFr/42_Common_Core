/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 18:27:36 by leG               #+#    #+#             */
/*   Updated: 2023/11/20 15:35:11 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new)
{
	if (lst == NULL || new == NULL)
		return ;
	new->next = *lst;
	*lst = new;
}

// int	main(void)
// {
// 	char *data1;
// 	char *data2;

// 	data1 = (char *)malloc(sizeof(char) * 6);
// 	data2 = (char *)malloc(sizeof(char) * 6);
// 	data1 = "Hello";
// 	data2 = "World";
// 	t_list *m1;
// 	t_list *m2;
// 	t_list *depart;
// 	m1 = ft_lstnew(data1);
// 	m2 = ft_lstnew(data2);
// 	depart = m2;
// 	ft_lstadd_front(&depart, m1);
// 	while (depart != NULL)
// 	{
// 		printf("%s\n", (char *)depart->content);
// 		depart = depart->next;
// 	}
// 	return (0);
// }