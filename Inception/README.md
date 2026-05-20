*This project has been created as part of the 42 curriculum by <login1>, <login2>[, <login3>...].*

# Inception — WordPress stack (Nginx + PHP-FPM + MariaDB)

## Description

Inception is a small Docker Compose stack that deploys a WordPress site served by Nginx and PHP-FPM with a MariaDB backend. The goal is to provide a reproducible, documented environment using Docker Compose for development and demonstration purposes.

Services included:
- `mariadb` — database server used by WordPress
- `wordpress` — PHP-FPM + WordPress
- `nginx` — reverse proxy and TLS termination

This repository contains Dockerfiles and service configuration under `srcs/` and orchestration via `srcs/docker-compose.yml`.

## Instructions

- Build and start the stack (creates host data directories and starts containers in background):

  `make build`

- Start services without building:

  `make up`

- Stop services:

  `make down`

- Force-stop containers:

  `make kill`

- Remove data and prune images (destructive):

  `make fclean`

See [Makefile](Makefile) and [srcs/docker-compose.yml](srcs/docker-compose.yml) for details and available targets.

## Resources

- Docker: https://docs.docker.com/
- Docker Compose: https://docs.docker.com/compose/
- WordPress: https://wordpress.org/
- WP-CLI: https://wp-cli.org/
- MariaDB: https://mariadb.org/
- Nginx: https://nginx.org/

AI usage
- If AI-assisted tools were used to create or revise documentation or scripts, document what was done here (which files or sections AI helped with). If none, you can remove this line.

## Project design choices

This section explains the main design choices and a short comparison according to the project requirements.

- Virtual Machines vs Docker

  - Chosen: Docker. Docker provides fast, lightweight isolation and is well suited for packaging the web stack and its dependencies. VMs would add more overhead and are unnecessary for this service-oriented stack.

- Secrets vs Environment Variables

  - Chosen: mixture. Secrets are declared in `docker-compose.yml` for database passwords (see `secrets:`) and the stack also loads values from `srcs/.env`. Use secrets for sensitive data (database passwords), and env files for non-sensitive configuration (domain name, database name, usernames). Note: the current `.env` file uses `SQL_` variable names — update it to provide `MYSQL_DATABASE`, `MYSQL_USER`, `MYSQL_PASSWORD`, `MYSQL_ROOT_PASSWORD` if you prefer env-file based credentials instead of Docker secrets.

- Docker Network vs Host Network

  - Chosen: bridge network (`inception`). Services communicate internally on a user-defined bridge network so we can keep the database unexposed to the host while exposing Nginx on port `443` only.

- Docker Volumes vs Bind Mounts

  - Chosen: bind mounts (host paths) for persistence. Volumes in `docker-compose.yml` are configured with `driver_opts` to bind `/home/krfranco/data/mariadb` and `/home/krfranco/data/wordpress` on the host. This makes it easy to inspect and persist data outside the containers.

## Where to look next

- Configuration and service definitions: [srcs/docker-compose.yml](srcs/docker-compose.yml)
- Service Dockerfiles and scripts: [srcs/requirements](srcs/requirements)
- Environment file: [srcs/.env](srcs/.env)

If you'd like I can update `.env` to include recommended `MYSQL_` variables or move all secrets to environment files — tell me which approach you prefer.
