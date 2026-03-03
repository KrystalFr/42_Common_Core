# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/06 21:17:57 by krfranco          #+#    #+#              #
#    Updated: 2023/11/19 01:33:58 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	libftprintf.a

SRCS	=	ft_printf.c \
			ft_put_csn.c \
			ft_put_uhp.c

OBJS	=	${SRCS:.c=.o}
CC		=	cc
CFLAGS	=	-Wall -Wextra -Werror
RM		=	rm -f

all:	${NAME}

.c.o:
		${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

${NAME}:		${OBJS}
		ar rc	${NAME} ${OBJS}

clean:
		${RM} ${OBJS}

fclean: clean
		${RM} ${NAME}

re: fclean all