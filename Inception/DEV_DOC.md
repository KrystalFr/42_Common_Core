# Developer documentation

This file explains how to set up, build, run and manage the project as a developer.

Prerequisites
- Linux (instructions assume a POSIX shell)
- Docker Engine
- Docker Compose (v2+ or the built-in `docker compose` command)
- Git

Clone and prepare

```bash
git clone <repo-url>
cd Inception
```

Secrets and environment
- The project consumes these secret files (place them in the repo `secrets/` folder):
  - `db_password.txt` — MariaDB user password
  - `db_root_password.txt` — MariaDB root password
  - `wp_admin_password.txt` — WordPress admin password (optional)
- Non-sensitive configuration is in `srcs/.env` (database name, user, domain).

Host data directories
- The compose file mounts host directories for persistent data. By default
  the paths are set to `/home/krfranco/data/mariadb` and
  `/home/krfranco/data/wordpress` (see `srcs/docker-compose.yml`). Either:
  - Create those directories with proper owner/permissions, or
  - Edit `srcs/docker-compose.yml` volumes to point to paths you prefer.

Build and launch

```bash
# from project root
make build     # creates host data dirs and runs `docker compose -f srcs/docker-compose.yml up --build -d`
```

Or run compose directly:

```bash
docker compose -f srcs/docker-compose.yml up --build -d
```

Manage containers
- Stop:

```bash
make down
# or
docker compose -f srcs/docker-compose.yml down
```

- Remove host data and images (destructive):

```bash
make fclean
```

- Tail logs:

```bash
docker compose -f srcs/docker-compose.yml logs -f
```

- Access a running container shell:

```bash
docker compose -f srcs/docker-compose.yml exec <service> sh
# example: docker compose -f srcs/docker-compose.yml exec mariadb sh
```

Inspect and debug
- List containers: `docker compose -f srcs/docker-compose.yml ps`
- Check volumes: `docker volume ls` and inspect bind-mounted host folders
- View MariaDB logs and interact using `mysql` client inside the mariadb container

Where data is stored
- MariaDB data: host path mapped to the `mariadb` volume (see compose `driver_opts.device`).
- WordPress files: host path mapped to the `wordpress` volume (see compose file).

Rebuild a single service

```bash
docker compose -f srcs/docker-compose.yml build wordpress
docker compose -f srcs/docker-compose.yml up -d wordpress
```

Notes for contributors
- If you change any host paths in `docker-compose.yml`, document them in this file.
- Use the `secrets/` folder for sensitive data and avoid committing real passwords.