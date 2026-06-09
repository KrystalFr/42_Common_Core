# Developer Documentation

## Prerequisites

Install the following before working on the project:

- Docker and Docker Compose
- `make`
- A `secrets/` folder at the repository root containing:
  - `db_root_password.txt`
  - `db_password.txt`
  - `wp_admin_password.txt`

## Project layout

The main configuration files are split between:

- [Makefile](Makefile)
- [srcs/docker-compose.yml](srcs/docker-compose.yml)
- [srcs/.env](srcs/.env)
- [srcs/requirements/mariadb/](srcs/requirements/mariadb/)
- [srcs/requirements/nginx/](srcs/requirements/nginx/)
- [srcs/requirements/wordpress/](srcs/requirements/wordpress/)

## Setup and launch

The simplest way to start the project is:

```bash
sudo make
```

This Makefile creates the host directories for persistent data and then launches the stack with Docker Compose.

You can also use the compose file directly:

```bash
docker compose -f srcs/docker-compose.yml up --build -d
```

## Makefile usage

- `make` or `sudo make` builds and starts the stack.
- `make up` starts the stack without rebuilding.
- `make down` stops the containers.
- `make clean` stops the containers and removes volumes.
- `make restart` performs a clean start.

## Docker Compose commands

- `docker compose -f srcs/docker-compose.yml up --build -d` builds and starts the stack in detached mode.
- `docker compose -f srcs/docker-compose.yml down` stops the stack.
- `docker compose -f srcs/docker-compose.yml logs` shows service logs.
- `docker compose -f srcs/docker-compose.yml ps` shows container status.

## Data persistence

- MariaDB data is stored in the `mariadb` volume, which is bound to `/home/krfranco/data/mariadb`.
- WordPress files are stored in the `wordpress` volume, which is bound to `/home/krfranco/data/wordpress`.
- The bind-mounted volumes ensure data persists across container rebuilds and restarts.

## Service layout

- MariaDB runs as the database backend and initializes users and databases from Docker secrets.
- WordPress runs PHP-FPM and configures itself with WP-CLI during startup.
- Nginx terminates HTTPS and proxies PHP requests to the WordPress container.

## Where data and configuration live

- Database connection values: [srcs/.env](srcs/.env)
- Secrets: [secrets/](secrets/)
- Nginx config: [srcs/requirements/nginx/conf/nginx.conf](srcs/requirements/nginx/conf/nginx.conf)
- MariaDB init scripts: [srcs/requirements/mariadb/tools/](srcs/requirements/mariadb/tools/)
- WordPress startup script: [srcs/requirements/wordpress/tools/](srcs/requirements/wordpress/tools/)
