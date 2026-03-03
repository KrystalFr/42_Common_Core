# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/20 03:19:57 by krfranco          #+#    #+#              #
#    Updated: 2025/10/20 17:22:28 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Animals

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp srcs/Animal.cpp srcs/Cat.cpp srcs/Dog.cpp srcs/WrongAnimal.cpp srcs/WrongCat.cpp srcs/Brain.cpp
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