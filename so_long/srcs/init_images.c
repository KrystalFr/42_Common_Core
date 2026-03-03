/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_images.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/30 22:18:48 by krfranco          #+#    #+#             */
/*   Updated: 2024/08/17 13:18:31 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	init_all(t_game *game)
{
	int	i;

	i = 0;
	game->error = 0;
	game->victory = 0;
	game->opendoor = 0;
	game->map = NULL;
	while (i < 4)
	{
		game->player[i] = NULL;
		game->textures[i] = NULL;
		i++;
	}
	game->textures[i] = NULL;
	init_back(game);
	init_player(game);
	if (game->error == 1)
		error_exit(game);
	game->moves = 1;
}

// 0.front 1.back 2.left 3.right
void	init_player(t_game *game)
{
	int	x;
	int	y;

	game->player[0] = mlx_xpm_file_to_image(game->mlx,
			"textures/Player/Front/walk/F-1.xpm", &x, &y);
	game->player[1] = mlx_xpm_file_to_image(game->mlx,
			"textures/Player/Back/walk/B-1.xpm", &x, &y);
	game->player[2] = mlx_xpm_file_to_image(game->mlx,
			"textures/Player/Left/walk/L-1.xpm", &x, &y);
	game->player[3] = mlx_xpm_file_to_image(game->mlx,
			"textures/Player/Right/walk/R-1.xpm", &x, &y);
	if (!game->player[0] || !game->player[1]
		|| !game->player[2] || !game->player[3])
	{
		ft_printf("Error\nFailed to initialize player textures\n");
		game->error = 1;
	}
}

//0.collectibles 1.walls  2.opendoor  3.closedoor 4.Floor
void	init_back(t_game *game)
{
	int	x;
	int	y;

	game->textures[0] = mlx_xpm_file_to_image(game->mlx,
			"textures/Surfaces/Collect/C1.xpm", &x, &y);
	game->textures[1] = mlx_xpm_file_to_image(game->mlx,
			"textures/Surfaces/Walls.xpm", &x, &y);
	game->textures[2] = mlx_xpm_file_to_image(game->mlx,
			"textures/Surfaces/Open.xpm", &x, &y);
	game->textures[3] = mlx_xpm_file_to_image(game->mlx,
			"textures/Surfaces/Closed.xpm", &x, &y);
	game->textures[4] = mlx_xpm_file_to_image(game->mlx,
			"textures/Surfaces/Floor.xpm", &x, &y);
	if (!game->textures[0] || !game->textures[1] || !game->textures[2]
		|| !game->textures[3] || !game->textures[4])
	{
		ft_printf("Error\nFailed to initialize map textures\n");
		game->error = 1;
	}
}

void	put_image(t_game *game, int x, int y, char c)
{
	if (c == '1')
		mlx_put_image_to_window(game->mlx, game->win,
			game->textures[1], x * SIZE, y * SIZE);
	else if (c == 'C')
		mlx_put_image_to_window(game->mlx, game->win,
			game->textures[0], x * SIZE, y * SIZE);
	else if (c == 'P')
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[0], x * SIZE, y * SIZE);
	else if (c == 'E')
	{
		mlx_put_image_to_window(game->mlx, game->win,
			game->textures[3], x * SIZE, y * SIZE);
	}
}

void	delete_all(t_game *game)
{
	int	i;

	i = 0;
	while (i < 4)
	{
		if (game->player[i])
			mlx_destroy_image(game->mlx, game->player[i]);
		if (game->textures[i])
			mlx_destroy_image(game->mlx, game->textures[i]);
		i++;
	}
	if (game->textures[i])
		mlx_destroy_image(game->mlx, game->textures[i]);
}
