# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/18 22:11:39 by krfranco          #+#    #+#              #
#    Updated: 2026/05/20 23:30:01 by krfranco         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

COMPOSE_FILE = ./srcs/docker-compose.yml

.PHONY: kill build down clean restart

all: build

build:
	mkdir -p /home/krfranco/data/mariadb
	mkdir -p /home/krfranco/data/wordpress
	docker compose  -f ${COMPOSE_FILE} up --build -d

up:
	docker compose -f ${COMPOSE_FILE} up

down:
	docker compose -f ${COMPOSE_FILE} down

kill:
	docker compose -f ${COMPOSE_FILE} kill

fclean: clean
	rm -r /home/krfranco/data/mariadb
	rm -r /home/krfranco/data/wordpress
	docker system prune -a -f

restart: clean build