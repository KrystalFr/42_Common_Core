/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 18:43:43 by krfranco          #+#    #+#             */
/*   Updated: 2024/08/17 13:38:13 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

int	close_win(t_game *game)
{
	delete_all(game);
	free_tab(game->map);
	mlx_destroy_window(game->mlx, game->win);
	mlx_destroy_display(game->mlx);
	if (game->px == game->doorx && game->py == game->doory
		&& game->opendoor == 1)
		game->victory = 1;
	if (game->victory == 1)
		ft_printf("\n- YOU WON ! CONGRATS !!! I know it was hard... -\n");
	else if (game->victory == 0 && game->error == 0)
		ft_printf("\n- You closed the game early.. Dont leave, come back ! -\n");
	free(game->mlx);
	exit(0);
	return (0);
}

int	k_press(int key, t_game *game)
{
	if (key == ESC)
		close_win(game);
	else if (key == LEFT || key == LEFT2)
		move_l(game, game->map);
	else if (key == RIGHT || key == RIGHT2)
		move_r(game, game->map);
	else if (key == UP || key == UP2)
		move_u(game, game->map);
	else if (key == DOWN || key == DOWN2)
		move_d(game, game->map);
	return (0);
}

int	main(int ac, char **av)
{
	t_game	game;
	int		x;
	int		y;

	if (ac != 2)
	{
		ft_printf("Need : <./so_long> <map path>\n");
		return (1);
	}
	if (check_file_path(av[1]))
		return (1);
	game.mlx = mlx_init();
	init_all(&game);
	game.map = check_map(av[1], &game);
	if (game.error == 1)
		error_exit(&game);
	x = game.mapx * SIZE;
	y = game.mapy * SIZE;
	game.win = mlx_new_window(game.mlx, x, y, "So_long");
	gen_map(game.map, &game);
	mlx_hook(game.win, KeyPress, KeyPressMask, &k_press, &game);
	mlx_hook(game.win, DestroyNotify, StructureNotifyMask, &close_win, &game);
	mlx_loop(game.mlx);
	return (0);
}
