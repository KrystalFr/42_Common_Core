/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 10:18:19 by krfranco          #+#    #+#             */
/*   Updated: 2024/07/02 18:47:50 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"

int	recursive_path(t_game *game, int y, int x)
{
	if (y < 0 || y >= game->mapy || x < 0 || x >= game->mapx)
		return (0);
	if (game->val[y][x] == '1')
		return (0);
	if (game->val[y][x] == '2')
		return (0);
	if (game->px == x && game->py == y)
		return (1);
	game->val[y][x] = '2';
	if (recursive_path(game, y + 1, x) || recursive_path(game, y, x + 1)
		|| recursive_path(game, y - 1, x) || recursive_path(game, y, x - 1))
		return (1);
	return (0);
}

void	path(int **pos, t_game *game)
{
	int	i;

	i = 0;
	if (!recursive_path(game, game->doory, game->doorx))
		game->error = 1;
	reset_val(game->val, game);
	while (i < game->countc && game->error != 1)
	{
		if (!recursive_path(game, pos[i][0], pos[i][1]))
			game->error = 1;
		reset_val(game->val, game);
		i++;
	}
	pos = NULL;
	if (game->error == 1)
		ft_printf("Error\nInvalid map : invalid path\n");
}

int	**collect_position(char **map, t_game *game)
{
	int		**ctab;
	int		c;

	ctab = malloc(game->countc * sizeof(int *));
	if (!ctab)
		return (NULL);
	c = 0;
	ctab = fill_ctab(ctab, game, map, c);
	if (game->error == 1)
	{
		free_tabi(ctab, game->countc);
		error_exit(game);
	}
	return (ctab);
}

int	**fill_ctab(int **ctab, t_game *game, char **map, int c)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->mapy && c != game->countc)
	{
		x = 0;
		while (x < game->mapx && c != game->countc)
		{
			if (map[y][x] == 'C')
			{
				ctab[c] = malloc(2 * sizeof(int));
				if (!ctab[c])
					game->error = 1;
				ctab[c][0] = y;
				ctab[c][1] = x;
				c++;
			}
			x++;
		}
		y++;
	}
	return (ctab);
}
