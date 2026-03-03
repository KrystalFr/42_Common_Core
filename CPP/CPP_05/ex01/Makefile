# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/07 23:12:24 by krfranco          #+#    #+#              #
#    Updated: 2025/11/11 17:04:31 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Forms

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp srcs/Bureaucrat.cpp srcs/Form.cpp
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