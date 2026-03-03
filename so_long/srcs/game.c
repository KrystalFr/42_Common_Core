/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 03:19:11 by krfranco          #+#    #+#             */
/*   Updated: 2024/05/31 04:39:33 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

void	move_l(t_game *game, char **map)
{
	if (map[game->py][game->px - 1] != '1')
	{
		if (map[game->py][game->px] == 'E')
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[3], game->px * SIZE, game->py * SIZE);
		else
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[4], game->px * SIZE, game->py * SIZE);
		game->px -= 1;
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[2], game->px * SIZE, game->py * SIZE);
		if (game->px == game->doorx && game->py == game->doory
			&& game->opendoor == 1)
			close_win(game);
		if (map[game->py][game->px] == 'C')
		{
			game->countc--;
			map[game->py][game->px] = '0';
			road_to_victory(game);
		}
		ft_printf("moves : %d\n", game->moves++);
	}
	else
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[2], game->px * SIZE, game->py * SIZE);
}

void	move_r(t_game *game, char **map)
{
	if (map[game->py][game->px + 1] != '1')
	{
		if (map[game->py][game->px] == 'E')
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[3], game->px * SIZE, game->py * SIZE);
		else
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[4], game->px * SIZE, game->py * SIZE);
		game->px += 1;
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[3], game->px * SIZE, game->py * SIZE);
		if (game->px == game->doorx && game->py == game->doory
			&& game->opendoor == 1)
			close_win(game);
		if (map[game->py][game->px] == 'C')
		{
			game->countc--;
			map[game->py][game->px] = '0';
			road_to_victory(game);
		}
		ft_printf("moves : %d\n", game->moves++);
	}
	else
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[3], game->px * SIZE, game->py * SIZE);
}

void	move_u(t_game *game, char **map)
{
	if (map[game->py - 1][game->px] != '1')
	{
		if (map[game->py][game->px] == 'E')
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[3], game->px * SIZE, game->py * SIZE);
		else
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[4], game->px * SIZE, game->py * SIZE);
		game->py -= 1;
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[1], game->px * SIZE, game->py * SIZE);
		if (game->px == game->doorx && game->py == game->doory
			&& game->opendoor == 1)
			close_win(game);
		if (map[game->py][game->px] == 'C')
		{
			game->countc--;
			map[game->py][game->px] = '0';
			road_to_victory(game);
		}
		ft_printf("moves : %d\n", game->moves++);
	}
	else
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[1], game->px * SIZE, game->py * SIZE);
}

void	move_d(t_game *game, char **map)
{
	if (map[game->py + 1][game->px] != '1')
	{
		if (map[game->py][game->px] == 'E')
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[3], game->px * SIZE, game->py * SIZE);
		else
			mlx_put_image_to_window(game->mlx, game->win,
				game->textures[4], game->px * SIZE, game->py * SIZE);
		game->py += 1;
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[0], game->px * SIZE, game->py * SIZE);
		if (game->px == game->doorx && game->py == game->doory
			&& game->opendoor == 1)
			close_win(game);
		if (map[game->py][game->px] == 'C')
		{
			game->countc--;
			map[game->py][game->px] = '0';
			road_to_victory(game);
		}
		ft_printf("moves : %d\n", game->moves++);
	}
	else
		mlx_put_image_to_window(game->mlx, game->win,
			game->player[0], game->px * SIZE, game->py * SIZE);
}

void	road_to_victory(t_game *game)
{
	if (game->countc == 0)
	{
		mlx_put_image_to_window(game->mlx, game->win,
			game->textures[2], game->doorx * SIZE, game->doory * SIZE);
		game->opendoor = 1;
	}
}
