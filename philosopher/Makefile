# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/06 17:14:30 by krfranco          #+#    #+#              #
#    Updated: 2025/06/01 10:02:54 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= philo

SRCS	= philo.c philo_utils.c philo_error.c philo_routine.c philo_init.c \
			philo_death_check.c philo_check_args.c
OBJS	=	${SRCS:.c=.o}
CFLAGS	=	-Wall -Wextra -g3 -pthread
RM		=	@rm -f

CYAN=\033[0;36m
RED=\033[0;31m
GOLD=\033[0;33m
NC=\033[0m # No Color

all: ${NAME}
	@echo "${CYAN}⚬ All targets compiled ✔${NC}"

${NAME}: ${OBJS}
	@${CC} $(CFLAGS) $^ -o ${NAME}
	@echo "${CYAN}⚬ philo built ✔${NC}"

clean:
		@${RM} ${OBJS}
		@echo "${RED}⚬ Cleaned *.o ✗${NC}"

fclean: clean
		@${RM} ${NAME}
		@echo "${RED}⚬ Cleaned all ✗${NC}"

re: fclean all