# Developer documentation

This file describes how to set up, build, and manage the project from scratch.

## Prerequisites

- Docker (Engine) and Docker Compose v2
- `make`
- A POSIX shell (bash/zsh)

## Repository layout

- `Makefile` — convenience targets to build, start and stop the stack.
- `srcs/docker-compose.yml` — Compose orchestration for `mariadb`, `wordpress`, `nginx`.
- `srcs/requirements/*` — Dockerfiles, configuration and helper scripts for each service.
- `srcs/.env` — environment variables loaded by Compose.

## Setup from scratch

1. Clone the repository.
2. Create the host data directories (the `Makefile` `build` target will create them automatically at `/home/krfranco/data/...`, or create them manually as your desired host path):

   `mkdir -p /home/krfranco/data/mariadb /home/krfranco/data/wordpress`

3. Provide secrets and environment variables. The stack expects Docker secrets files referenced in `srcs/docker-compose.yml` (for example `./secrets/db_password.txt`) and an env-file at `srcs/.env`.

   Example `.env` recommended variables (place in `srcs/.env`):

   MYSQL_DATABASE=inception_db
   MYSQL_USER=inception_user
   MYSQL_PASSWORD=your_db_password_here
   MYSQL_ROOT_PASSWORD=your_root_password_here
   DOMAIN_NAME=your.domain.tld

   Note: the included `srcs/.env` currently contains `SQL_` variables; update it to include the `MYSQL_` variables shown above or supply equivalent secrets at `./secrets/`.

4. Create the `secrets` directory and files (if you prefer secrets over plaintext env vars):

   `mkdir -p secrets`

   Create files such as `secrets/db_password.txt`, `secrets/db_root_password.txt`, `secrets/wp_admin_password.txt` containing the secret values (one secret per file). Keep these files out of version control.

## Build and launch

- Build and start in detached mode (recommended):

  `make build`

- Start without building:

  `make up`

- Stop containers:

  `make down`

- Fully remove data (destructive):

  `make fclean`

The targets call `docker compose -f srcs/docker-compose.yml ...` — you can run the same commands manually if desired.

## Useful docker-compose commands

- Show status:

  `docker compose -f srcs/docker-compose.yml ps`

- Show logs (follow):

  `docker compose -f srcs/docker-compose.yml logs -f nginx`

- Open a shell in a running container:

  `docker compose -f srcs/docker-compose.yml exec wordpress bash`

- Run WP-CLI inside the WordPress container:

  `docker compose -f srcs/docker-compose.yml exec wordpress wp --info`

## Where project data is stored and persistence

- WordPress files and uploads are bind-mounted to the host path `/home/krfranco/data/wordpress` (see `volumes` in `srcs/docker-compose.yml`).
- MariaDB data is bind-mounted to `/home/krfranco/data/mariadb`.
- Database passwords and admin password can be provided either via Docker secrets (`secrets/`) or environment variables in `srcs/.env`.

## Notes and troubleshooting

- Ports: only `443` is published on the host (nginx). Database and php-fpm are reachable only on the internal Docker network.
- TLS: the `nginx` image currently generates a self-signed certificate inside the image. For production, replace these with valid certificates and mount them into the container.
- Environment variable mismatch: the provided `srcs/.env` uses `SQL_` names; `wordpress` expects `MYSQL_*` env vars (or secrets). Update `srcs/.env` or `docker-compose.yml` to align variable names.

If you want I can prepare an example `srcs/.env.example` and automated secret bootstrap scripts.
