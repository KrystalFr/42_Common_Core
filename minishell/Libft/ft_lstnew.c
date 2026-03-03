/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leG <leG@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 13:18:20 by leG               #+#    #+#             */
/*   Updated: 2023/11/18 22:57:49 by leG              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*new;

	new = (t_list *)malloc(sizeof(t_list));
	if (new == NULL)
		return (NULL);
	new->content = content;
	new->next = NULL;
	return (new);
}

/*int	main(void)
{
	char *data;
	t_list *new;

	data = (char *)malloc(sizeof(char) * 6);
	data = "Hello";
	new = ft_lstnew(data);
	printf("%s\n", (char *)new->content);
	return (0);
}*/