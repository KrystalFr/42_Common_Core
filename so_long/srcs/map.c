/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 08:16:08 by krfranco          #+#    #+#             */
/*   Updated: 2024/07/02 18:41:57 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	init_pos(char **map, t_game *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if (map[y][x] == 'P')
			{
				game->px = x;
				game->py = y;
			}
			else if (map[y][x] == 'E')
			{
				game->doorx = x;
				game->doory = y;
			}
			x++;
		}
		y++;
	}
}

char	**read_map(char *map, t_game *game)
{
	int		fd;
	char	*gnl;
	char	**map_tab;
	int		i;

	count_map(map, game);
	fd = open(map, O_RDONLY);
	gnl = get_next_line(fd);
	map_tab = malloc(((*game).mapy + 1) * sizeof(char *));
	if (!map_tab)
		return (NULL);
	i = 0;
	while (gnl)
	{
		map_tab[i] = gnl;
		gnl = get_next_line(fd);
		i++;
	}
	map_tab[i] = NULL;
	close(fd);
	game->countc = count_letter(game, map_tab, 'C');
	game->countp = count_letter(game, map_tab, 'P');
	game->counte = count_letter(game, map_tab, 'E');
	return (map_tab);
}

void	count_map(char *map, t_game *game)
{
	int		fd;
	char	*gnl;
	int		x;
	int		y;

	fd = open(map, O_RDONLY);
	gnl = get_next_line(fd);
	x = 0;
	y = 0;
	while (gnl)
	{
		if (y == 1)
			x = ft_strlen(gnl) - 1;
		free(gnl);
		gnl = get_next_line(fd);
		y++;
	}
	close(fd);
	free(gnl);
	(*game).mapx = x;
	(*game).mapy = y;
}

void	gen_map(char **map, t_game *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if (map[y][x] == '1')
				put_image(game, x, y, '1');
			else if (map[y][x] == 'C')
				put_image(game, x, y, 'C');
			else if (map[y][x] == 'P')
			{
				put_image(game, x, y, 'P');
				game->px = x;
				game->py = y;
			}
			else if (map[y][x] == 'E')
				put_image(game, x, y, 'E');
			x++;
		}
		y++;
	}
}
