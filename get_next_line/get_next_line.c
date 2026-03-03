/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/11 19:02:37 by krfranco          #+#    #+#             */
/*   Updated: 2023/11/22 17:27:59 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	cleanup(char *line, char *keep)
{
	int	i;
	int	j;
	int	end;

	j = 0;
	i = 0;
	while (line[i] != '\n' && line[i])
		i++;
	end = i + 1;
	while (line[i++])
	{
		keep[j] = line[i];
		j++;
	}
	line[end] = '\0';
	keep[j] = '\0';
	return ;
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	keep[BUFFER_SIZE + 1];

	line = NULL;
	if (keep[0] != 0)
	{
		line = ft_strdup(keep);
		if (ft_strchr(line, '\n'))
			return (cleanup(line, keep), line);
	}
	ft_bzero(keep, BUFFER_SIZE + 1);
	while (read(fd, keep, BUFFER_SIZE) > 0)
	{
		if (!line)
			line = ft_strdup("");
		line = ft_strjoin(line, keep);
		if (ft_strchr(line, '\n') || ft_strchr(line, '\0'))
			return (cleanup(line, keep), line);
		ft_bzero(keep, BUFFER_SIZE + 1);
	}
	return (line);
}
