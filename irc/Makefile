CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
NAME := ircserv

SRC_DIR := src
INC_DIR := include

SRCS := $(SRC_DIR)/main.cpp \
	$(SRC_DIR)/Server.cpp \
	$(SRC_DIR)/Client.cpp \
	$(SRC_DIR)/Channel.cpp \
	$(SRC_DIR)/CommandHandler.cpp \
	$(SRC_DIR)/Utils.cpp \
	$(SRC_DIR)/Logger.cpp

OBJS := $(SRCS:.cpp=.o)

CPPFLAGS := -I$(INC_DIR)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
