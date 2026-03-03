/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gaperaud <gaperaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/26 12:57:49 by gaperaud          #+#    #+#             */
/*   Updated: 2025/01/17 00:27:48 by gaperaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_strlcpy_gnl(char *dest, const char *src, int l)
{
	int	i;

	i = 0;
	while (i + 1 < l && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	if (l)
		dest[i] = 0;
}

int	ft_okay(t_data *data)
{
	while (data->str[data->line_len] && data->str[data->line_len] != '\n')
		data->line_len++;
	if (data->str[data->line_len] == '\n')
	{
		data->line_len++;
		return (0);
	}
	return (1);
}

int	ft_remp(t_data *data, int fd)
{
	char	*temp;

	data->read_size = BUFFER_SIZE;
	while (ft_okay(data) && data->read_size == BUFFER_SIZE)
	{
		if (data->str_size + data->read_size >= data->str_buffer)
		{
			temp = malloc(sizeof(char) * (data->str_buffer * 2));
			if (!temp)
				return (-1);
			data->str_buffer *= 2;
			ft_strlcpy (temp, data->str, data->str_size + 1);
			free (data->str);
			data->str = temp;
		}
		data->read_size = read(fd, data->str + data->str_size, BUFFER_SIZE);
		if (data->read_size == -1)
			return (-1);
		data->str_size += data->read_size;
		data->str[data->str_size] = 0;
	}
	return (data->line_len);
}

char	*get_next_line(int fd)
{
	t_data		data;
	static char	stash[FD][BUFFER_SIZE] = {0};
	char		*ret;

	if (fd < 0 || fd > FD || BUFFER_SIZE < 1)
		return (NULL);
	data.str_buffer = BUFFER_SIZE + 1;
	data.str = malloc (sizeof(char) * data.str_buffer);
	if (!data.str)
		return (NULL);
	data.str_size = 0;
	while (stash[fd][data.str_size])
		data.str_size++;
	ft_strlcpy(data.str, stash[fd], data.str_size + 1);
	data.line_len = 0;
	data.line_len = ft_remp(&data, fd);
	if (data.line_len == -1 || !data.str[0])
		return (free(data.str), NULL);
	ret = malloc (sizeof(char) * data.line_len + 1);
	if (!ret)
		return (free(data.str), NULL);
	ft_strlcpy(ret, data.str, data.line_len + 1);
	ft_strlcpy(stash[fd], data.str + data.line_len,
		data.str_size - data.line_len + 1);
	return (free(data.str), ret);
}
