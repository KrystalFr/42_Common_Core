/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 23:28:33 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/14 17:34:32 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*sub;
	size_t	size;

	sub = NULL;
	if (!s)
		return (NULL);
	if (start > ft_strlen(s))
		return (ft_strdup(""));
	size = 0;
	while (s[start + size] && size < len)
		size++;
	sub = malloc((size + 1) * sizeof(char));
	if (!sub)
		return (NULL);
	s += start;
	ft_strlcpy(sub, s, size + 1);
	return (sub);
}
