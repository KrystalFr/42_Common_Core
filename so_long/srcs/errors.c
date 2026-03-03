/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 13:42:53 by krfranco          #+#    #+#             */
/*   Updated: 2024/08/17 12:37:00 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	error_exit(t_game *game)
{
	delete_all(game);
	if (game->map != NULL)
	{
		free_tab(game->map);
		game->map = NULL;
	}
	mlx_destroy_display(game->mlx);
	free(game->mlx);
	game->mlx = NULL;
	exit(EXIT_FAILURE);
}

void	check_doubles(char **map, t_game *game, char c, char *str)
{
	int	count;
	int	y;
	int	x;

	y = 0;
	count = 0;
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
	if (count > 1)
	{
		game->error = 1;
		ft_printf("Error\nInvalid map : multiple %s\n", str);
	}
}

void	check_borders(char **map, t_game *game)
{
	int	y;
	int	x;
	int	error;

	error = 0;
	y = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if ((x == 0 || x == game->mapx - 1
					|| y == 0 || y == game->mapy - 1)
				&& map[y][x] != '1')
			{
				game->error = 1;
				error = 1;
			}
			x++;
		}
		y++;
	}
	if (error == 1)
		ft_printf("Error\nInvalid map : map not closed by walls.\n");
}

void	simple_checks(char **map, t_game *game)
{
	if (game->countc == 0)
	{
		ft_printf("Error\nInvalid map : no collectibles.\n");
		game->error = 1;
	}
	if (game->countp == 0)
	{
		ft_printf("Error\nInvalid map : no player.\n");
		game->error = 1;
	}
	if (game->counte == 0)
	{
		ft_printf("Error\nInvalid map : no exit.\n");
		game->error = 1;
	}
	if (game->mapx == game->mapy || !proper_rectangle(map, game))
	{
		ft_printf("Error\nInvalip map : map should be a rectangle.\n");
		game->error = 1;
	}
}

int	proper_rectangle(char **map, t_game *game)
{
	int	y;

	y = 0;
	while (y < game->mapy)
	{
		if (y == game->mapy - 1)
		{
			if (ft_strlen(map[y]) != game->mapx)
				return (0);
		}
		else
		{
			if (ft_strlen(map[y]) != game->mapx + 1)
				return (0);
		}
		y++;
	}
	return (1);
}
