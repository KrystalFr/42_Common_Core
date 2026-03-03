/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/02 17:07:30 by krfranco          #+#    #+#             */
/*   Updated: 2024/08/17 12:11:35 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

char	**check_map(char *map, t_game *game)
{
	char	**maptab;

	maptab = read_map(map, game);
	if (!maptab || game->error == 1)
		error_exit(game);
	check_doubles(maptab, game, 'E', "exits");
	check_doubles(maptab, game, 'P', "players");
	check_invalid_char(maptab, game);
	init_pos(maptab, game);
	simple_checks(maptab, game);
	check_borders(maptab, game);
	if (game->error != 1)
	{
		game->val = copy_array(maptab, game->mapy);
		check_path(game);
	}
	return (maptab);
}

void	check_path(t_game *game)
{
	int		**inttab;

	inttab = collect_position(game->val, game);
	if (!inttab)
		error_exit(game);
	path(inttab, game);
	free_tabi(inttab, game->countc);
	if (game->val != NULL)
	{
		free_tab(game->val);
		game->val = NULL;
	}
}

int	check_file_path(char *av)
{
	int		fd;
	char	keep[1];

	fd = open(av, O_RDONLY);
	if (fd == -1)
	{
		ft_printf("Error\nInvalid file path\n");
		return (1);
	}
	read(fd, keep, 1);
	if (errno == EISDIR)
	{
		ft_printf("Error\nTrying to use a directory\n");
		close(fd);
		return (1);
	}
	else
	{
		close (fd);
		return (0);
	}
}

void	check_invalid_char(char **map, t_game *game)
{
	int	y;
	int	x;
	int	error;

	y = 0;
	error = 0;
	while (y < game->mapy)
	{
		x = 0;
		while (x < game->mapx)
		{
			if (!ft_strchr("01CEP", map[y][x]))
				error = 1;
			x++;
		}
		y++;
	}
	if (error == 1)
	{
		ft_printf("Error\nInvalid map : map contain invalid character\n");
		game->error = 1;
	}
}
