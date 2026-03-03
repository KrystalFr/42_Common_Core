# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/25 19:08:33 by krfranco          #+#    #+#              #
#    Updated: 2024/04/11 02:03:19 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= push_swap

SRCS	= push_swap.c \
		srcs/utils_1.c srcs/utils_2.c srcs/utils_3.c srcs/utils_4.c \
		srcs/utils_5.c \
		srcs/actions_1.c srcs/actions_2.c srcs/ft_split.c \
		srcs/sorting_1.c srcs/sorting_2.c srcs/sorting_3.c

PRINTF_DIR = ft_Printf
OBJS	=	${SRCS:.c=.o}
CC		=	cc
CFLAGS	=	-Wall -Wextra -Werror -g3
RM		=	rm -f
PRINTF	=	${PRINTF_DIR}/libftprintf.a

all: ${NAME}

${NAME}: ${SRCS} ${PRINTF}
	${CC} $(CFLAGS) $^ -o ${NAME}

${PRINTF}:
	make -C ${PRINTF_DIR}

clean:
		${RM} ${OBJS}
		@${MAKE} -C ${PRINTF_DIR} clean

fclean: clean
		${RM} ${NAME}
		@${MAKE} -C ${PRINTF_DIR} fclean

re: fclean all