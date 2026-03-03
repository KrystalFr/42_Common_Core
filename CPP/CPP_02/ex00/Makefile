# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/18 14:32:23 by krfranco          #+#    #+#              #
#    Updated: 2025/10/19 01:14:02 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Bits

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp Fixed.cpp
OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} -o ${NAME} ${OBJS}

%.o: %.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

clean:
	rm -f ${OBJS}

fclean: clean
	rm -f ${NAME}

re: fclean all