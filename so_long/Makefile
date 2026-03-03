# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/25 19:08:33 by krfranco          #+#    #+#              #
#    Updated: 2024/08/17 12:06:07 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= so_long

SRCS	= Get_Next_Line/get_next_line_utils.c Get_Next_Line/get_next_line.c \
		srcs/main.c srcs/errors.c srcs/errors2.c \
		srcs/init_images.c srcs/map.c srcs/game.c \
		srcs/utils.c srcs/path.c

PRINTF_DIR = ft_Printf
OBJS	=	${SRCS:.c=.o}
CC		=	cc
CFLAGS	=	-Wall -Wextra -Werror -g3
RM		=	rm -f
PRINTF	=	$(PRINTF_DIR)/libftprintf.a

MLX_DIR =	./minilibx-linux
INCLUDES	=	-I/usr/include -Imlx
MLX_FLAGS	=	-Lminilibx-linux -lmlx -L/usr/lib/X11 -lXext -lX11
MLX			=	$(MLX_DIR)/libmlx.a


CYAN=\033[0;36m
RED=\033[0;31m
GOLD=\033[0;33m
NC=\033[0m # No Color


all: ${NAME}

${NAME}: ${SRCS} ${MLX} ${PRINTF}
	@${CC} ${CFLAGS} $^ -o ${NAME} ${MLX_FLAGS}
	@echo "${CYAN}⚬ All targets compiled ✔${NC}"
	@echo "${CYAN}⚬ So_long built ✔${NC}"

${PRINTF}:
	@make --no-print-directory -s -C ${PRINTF_DIR}

${MLX}:
	@make --no-print-directory -s -C ${MLX_DIR} > /dev/null 2>&1

clean:
		@${RM} ${OBJS}
		@${MAKE} --no-print-directory -s -C ${PRINTF_DIR} clean
		@${MAKE} --no-print-directory -s -C ${MLX_DIR} clean > /dev/null 2>&1
		@echo "${RED}⚬ Cleaned *.o ✗${NC}"

fclean: clean
		@${RM} ${NAME}
		@${MAKE} --no-print-directory -s -C ${PRINTF_DIR} fclean
		@${MAKE} --no-print-directory -s -C ${MLX_DIR} clean > /dev/null 2>&1
		@echo "${RED}⚬ Cleaned all ✗${NC}"

re: fclean all