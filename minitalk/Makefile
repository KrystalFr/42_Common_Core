# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/25 19:08:33 by krfranco          #+#    #+#              #
#    Updated: 2024/04/29 19:25:45 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME_CLIENT	= client
NAME_SERVER = server
NAME_CLIENT_B = client_bonus
NAME_SERVER_B = server_bonus

SRCS_CLIENT	= srcs/mandatory/client.c srcs/mandatory/utils.c
SRCS_SERVER = srcs/mandatory/server.c srcs/mandatory/utils.c
SRCS_CLIENT_B = srcs/bonus/client_bonus.c srcs/bonus/utils_bonus.c
SRCS_SERVER_B = srcs/bonus/server_bonus.c srcs/bonus/utils_bonus.c

PRINTF_DIR = ft_Printf
OBJS_SERVER	=	${SRCS_SERVER:.c=.o}
OBJS_CLIENT = 	${SRCS_CLIENT:.c=.o}
OBJS_SERVER_B =	${SRCS_SERVER_B:.c=.o}
OBJS_CLIENT_B =	${SRCS_CLIENT_B:.c=.o}
CC		=	@cc
CFLAGS	=	-Wall -Wextra -Werror -g3
RM		=	@rm -f
PRINTF	=	${PRINTF_DIR}/libftprintf.a

CYAN=\033[0;36m
RED=\033[0;31m
GOLD=\033[0;33m
NC=\033[0m # No Color

all: ${NAME_SERVER} ${NAME_CLIENT}
	@echo "${CYAN}⚬ All targets compiled ✔${NC}"

${NAME_SERVER}: ${SRCS_SERVER} ${PRINTF}
	@${CC} $(CFLAGS) $^ -o ${NAME_SERVER}
	@echo "${CYAN}⚬ Server built ✔${NC}"

${NAME_CLIENT}: ${SRCS_CLIENT} ${PRINTF}
	@${CC} $(CFLAGS) $^ -o ${NAME_CLIENT}
	@echo "${CYAN}⚬ Client built ✔${NC}"
	
bonus: ${NAME_SERVER_B} ${NAME_CLIENT_B}
	@echo "${GOLD}⚬ All bonus targets compiled ✔${NC}"

${NAME_SERVER_B}: ${SRCS_SERVER_B} ${PRINTF}
	@${CC} $(CFLAGS) $^ -o ${NAME_SERVER_B}
	@echo "${GOLD}⚬ Server bonus built ✔${NC}"

${NAME_CLIENT_B}: ${SRCS_CLIENT_B} ${PRINTF}
	@${CC} $(CFLAGS) $^ -o ${NAME_CLIENT_B}
	@echo "${GOLD}⚬ Client bonus built ✔${NC}"

${PRINTF}:
	@make --no-print-directory -C ${PRINTF_DIR}

clean:
		@${RM} ${OBJS_CLIENT} ${OBJS_SERVER} ${OBJS_CLIENT_B} ${OBJS_SERVER_B}
		@${MAKE} --no-print-directory -C ${PRINTF_DIR} clean
		@echo "${RED}⚬ Cleaned *.o ✗${NC}"

fclean: clean
		@${RM} ${NAME_SERVER} ${NAME_CLIENT} ${NAME_SERVER_B} ${NAME_CLIENT_B}
		@${MAKE} --no-print-directory -C ${PRINTF_DIR} fclean
		@echo "${RED}⚬ Cleaned all ✗${NC}"

re: fclean all