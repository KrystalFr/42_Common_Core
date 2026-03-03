/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 05:45:13 by krfranco          #+#    #+#             */
/*   Updated: 2024/07/02 18:46:04 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	if (tab != NULL)
	{
		while (tab[i] != NULL)
		{
			free(tab[i]);
			tab[i] = NULL;
			i++;
		}
		free(tab);
		tab = NULL;
	}
}

void	free_tabi(int **tab, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (tab[i])
			free(tab[i]);
		i++;
	}
	free(tab);
}

int	count_letter(t_game *game, char **map, char c)
{
	int	x;
	int	y;
	int	count;

	count = 0;
	y = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if (map[y][x] == c)
				count++;
			x++;
		}
		y++;
	}
	return (count);
}

char	**copy_array(char **srcs, int size)
{
	int		i;
	char	**cpy;

	i = 0;
	cpy = malloc((size + 1) * sizeof(char *));
	if (!cpy)
		return (NULL);
	while (i < size)
	{
		cpy[i] = ft_strdup(srcs[i]);
		if (!cpy[i])
		{
			while (i-- > 0)
				free(cpy[i]);
			free(cpy);
			return (NULL);
		}
		i++;
	}
	cpy[i] = NULL;
	return (cpy);
}

void	reset_val(char **val, t_game *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if (val[y][x] == '2')
				val[y][x] = '0';
			x++;
		}
		y++;
	}
}
