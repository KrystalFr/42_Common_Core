/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 17:17:06 by gaperaud          #+#    #+#             */
/*   Updated: 2025/02/08 02:27:36 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_malloc(int size);

void	*new_gnode(t_garbage **list_head, int size)
{
	t_garbage	*new;
	t_garbage	*temp;
	void		*ptr;

	new = malloc(sizeof(t_garbage));
	if (!new)
		return (NULL);
	new->ptr = malloc(size);
	if (!new->ptr)
		return (free(new), NULL);
	ptr = new->ptr;
	new->next = NULL;
	if (*list_head == NULL)
		*list_head = new;
	else
	{
		temp = *list_head;
		while (temp->next)
			temp = temp->next;
		temp->next = new;
	}
	return (ptr);
}

void	ft_free(void)
{
	t_garbage	**list_head;
	t_garbage	*head;
	t_garbage	*temp;

	list_head = (t_garbage **)ft_malloc(-1);
	head = *list_head;
	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp->ptr);
		free(temp);
	}
	*list_head = NULL;
}

void	*ft_malloc(int size)
{
	static t_garbage	*head = NULL;
	void				*ptr;

	if (size == -1)
		return (&head);
	ptr = new_gnode(&head, size);
	if (!ptr)
	{
		ft_putstr_fd("malloc error\n", 2);
		return (NULL);
	}
	return (ptr);
}

// int	main(void)
// {
// 	char *str;
// 	char *str1;
// 	char *str2;

// 	str = ft_malloc(sizeof(char) * 4);
// 	str[0] = 'a';
// 	str[1] = '\0';
// 	printf("%s\n", str);
// 	str1 = ft_malloc(sizeof(char) * 4);
// 	str1[0] = 'b';
// 	str1[1] = '\0';
// 	printf("%s\n", str1);
// 	ft_free();
// 	str2 = ft_malloc(sizeof(char) * 4);
// 	str2[0] = 'c';
// 	str2[1] = '\0';
// 	printf("%s\n", str2);
// 	ft_free ();
// 	return (0);
// }