# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/11/25 19:08:33 by krfranco          #+#    #+#              #
#    Updated: 2025/02/21 22:35:26 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= minishell

SRCS	= Minishell/main.c Minishell/parsing/lexer/lexer.c \
				Minishell/parsing/lexer/lexer_utils.c \
				Minishell/exit/exit.c Minishell/init/init.c \
				Minishell/parsing/here_doc/here_doc.c\
				Minishell/init/init.c\
				Minishell/parsing/Parser/automate.c Minishell/parsing/Parser/automate_utils.c \
				Minishell/parsing/Parser/automate.c Minishell/parsing/Parser/automate.c \
				Minishell/parsing/Parser/parser.c \
				Minishell/parsing/expander/expander.c Minishell/parsing/expander/expander_utils3.c \
				Minishell/parsing/expander/expander_utils.c Minishell/parsing/expander/expander_utils2.c \
				Minishell/exec/adjust_token_for_exec/token_manipulation_utils.c\
				Minishell/exec/adjust_token_for_exec/adjust_token_for_exec.c\
				Minishell/exec/adjust_token_for_exec/segment.c\
				Minishell/exec/adjust_token_for_exec/determine_redirection_and_file.c\
				Minishell/exec/adjust_token_for_exec/determine_redirection_and_file_utils.c\
				Minishell/exec/adjust_token_for_exec/determine_token_option_and_arguments.c\
				Minishell/exec/adjust_token_for_exec/redirection_utils.c\
				Minishell/exec/exec_utils/dup_redirection.c\
				Minishell/exec/exec_utils/path_utils.c\
				Minishell/exec/exec_utils/paths.c\
				Minishell/exec/exec_utils/sort_tokens_and_arguments.c\
				Minishell/exec/exec_utils/exec_utils.c\
				Minishell/exec/executor.c\
				Minishell/exec/builtins/echo.c\
				Minishell/exec/builtins/pwd.c\
				Minishell/exec/builtins/export.c\
				Minishell/exec/builtins/cd.c\
				Minishell/exec/builtins/cd_utils.c\
				Minishell/exec/builtins/export_utils.c\
				Minishell/exec/builtins/env.c\
				Minishell/exec/builtins/unset.c\
				Minishell/exec/builtins/exit.c\
				Minishell/exec/builtins/builtins_utils.c\
				Minishell/signal/signal.c\
				Minishell/signal/heredoc_mode.c\
				Minishell/signal/prompt_mode.c\
				Minishell/signal/exec_mode.c

OBJSDIR = objs/

OBJS = 	${SRCS:.c=.o}

OBJS := $(addprefix $(OBJSDIR), $(OBJS))

LIBFT_DIR = Libft
CC		=	cc
CFLAGS	=	-Wall -Wextra -Werror -g3 -I/usr/include/readline 
LDFLAGS =	-L/usr/lib -lreadline
RM		=	rm -rf
LIBFT	=	$(LIBFT_DIR)/libft.a


CYAN=\033[0;36m
RED=\033[0;31m
GOLD=\033[0;33m
NC=\033[0m # No Color


all: ${NAME}

${NAME}: ${OBJS} ${LIBFT}
	@${CC} ${CFLAGS} $^ ${LDFLAGS} -o ${NAME}
	@echo "${CYAN}⚬ All targets compiled ✔${NC}"
	@echo "${CYAN}⚬ Minishell	 built ✔${NC}"

$(OBJSDIR)%.o: %.c
	@mkdir -p $(@D)
	@${CC} ${CFLAGS} -c $< -o $@

${LIBFT}:
	@${MAKE} --no-print-directory -s -C ${LIBFT_DIR}

clean:
		@${RM} ${OBJS}
		@${RM} ${OBJSDIR}
		@${MAKE} --no-print-directory -s -C ${LIBFT_DIR} clean
		@echo "${RED}⚬ Cleaned *.o ✗${NC}"

fclean: clean
		@${RM} ${NAME}
		@${MAKE} --no-print-directory -s -C ${LIBFT_DIR} fclean
		@echo "${RED}⚬ Cleaned all ✗${NC}"

re: fclean
		@${MAKE} --no-print-directory all