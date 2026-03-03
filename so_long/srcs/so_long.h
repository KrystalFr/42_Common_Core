/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 19:40:18 by krfranco          #+#    #+#             */
/*   Updated: 2024/08/17 12:06:50 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H

# include "../ft_Printf/ft_printf.h"
# include "../Get_Next_Line/get_next_line.h"
# include "../minilibx-linux/mlx.h"
# include <stdlib.h>
# include <stdio.h>
# include <stdio.h>
# include <stdlib.h>
# include <X11/X.h>
# include <X11/keysym.h>
# include <errno.h>

# define SIZE 64
# define ESC 65307
# define LEFT2 65361
# define RIGHT2 65363
# define UP2 65362
# define DOWN2 65364
# define LEFT 97
# define RIGHT 100
# define UP 119
# define DOWN 115

//mommy
typedef struct s_game
{
	void		*mlx;
	void		*win;
	void		*player[4];
	void		*textures[5];
	char		**map;
	char		**val;
	int			mapx;
	int			mapy;
	int			px;
	int			py;
	int			opendoor;
	int			doorx;
	int			doory;
	int			countc;
	int			countp;
	int			counte;
	int			moves;
	int			error;
	int			victory;

}t_game;

//game event

int		close_win(t_game *game);
int		k_press(int key, t_game *game);
void	road_to_victory(t_game *game);

//init_textures

void	init_player(t_game *game);
void	init_back(t_game *game);
void	init_all(t_game *game);
void	put_image(t_game *game, int x, int y, char c);
void	delete_all(t_game *game);

//map

char	**read_map(char *map, t_game *game);
void	count_map(char *map, t_game *game);
char	**check_map(char *map, t_game *game);
void	gen_map(char **map, t_game *game);
void	init_pos(char **map, t_game *game);

//moves

void	move_l(t_game *game, char **map);
void	move_r(t_game *game, char **map);
void	move_d(t_game *game, char **map);
void	move_u(t_game *game, char **map);

//utils

void	free_tab(char **tab);
void	free_tabi(int **tab, int size);
int		count_letter(t_game *game, char **map, char c);
char	**copy_array(char **srcs, int size);
void	reset_val(char **val, t_game *game);

//error management

void	simple_checks(char **map, t_game *game);
int		proper_rectangle(char **map, t_game *game);
void	error_exit(t_game *game);
void	check_doubles(char **tab, t_game *game, char c, char *str);
void	check_borders(char **map, t_game *game);
int		**collect_position(char **map, t_game *game);
int		**fill_ctab(int **ctab, t_game *game, char **map, int c);
int		recursive_path(t_game *game, int y, int x);
void	path(int **pos, t_game *game);
void	check_path(t_game *game);
int		check_file_path(char *av);
void	check_invalid_char(char **map, t_game *game);

#endif