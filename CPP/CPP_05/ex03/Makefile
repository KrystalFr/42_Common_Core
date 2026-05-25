# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/07 23:12:24 by krfranco          #+#    #+#              #
#    Updated: 2026/05/25 16:54:39 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = Interns

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp srcs/Bureaucrat.cpp srcs/AForm.cpp srcs/ShrubberyCreationForm.cpp srcs/RobotomyRequestForm.cpp srcs/PresidentialPardonForm.cpp srcs/Intern.cpp
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
	rm -f *_shrubbery

re: fclean all