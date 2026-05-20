# User Documentation

## Overview
This stack provides:
- `mariadb` — MariaDB database (MySQL-compatible).
- `wordpress` — WordPress PHP application.
- `nginx` — Reverse proxy / TLS termination serving WordPress.

Services are defined in `srcs/docker-compose.yml`.

## Start / Stop
Start the project (detached, build if needed):
- Using Docker Compose:
```bash
docker-compose -f srcs/docker-compose.yml up -d --build
```
- Using the project's Makefile:
```bash
make build
```

Stop the project:
```bash
docker-compose -f srcs/docker-compose.yml down --volumes --remove-orphans
```
or
```bash
make down
```

For a destructive cleanup (removes containers and volumes):
```bash
make clean
```
or remove manually:
```bash
docker-compose -f srcs/docker-compose.yml down -v
```

## How to Access
- Website (via Nginx): https://localhost (or https://<host-ip> if running remotely). Nginx listens on port 443 per the compose file.
- WordPress admin panel: https://localhost/wp-admin

If you run on a remote host, replace `localhost` with the host IP or domain pointing at the host.

## Credentials
- Database credentials are provided via the repository `.env` file used by docker-compose. Look for:
  - `MYSQL_USER`
  - `MYSQL_PASSWORD`
  - `MYSQL_DATABASE`
Place or update these in your repository root `.env` before first `up`.

If your WordPress admin user was created during build/config scripts, find any defaults or creation steps in:
- `srcs/requirements/wordpress/tools/wordpress_config.sh` (if present).

## Where data is stored / persistence
This project binds volumes to host paths (see `srcs/docker-compose.yml`):
- MariaDB data: `/home/krfranco/data/mariadb`
- WordPress files: `/home/krfranco/data/wordpress`

Backups: copy those host directories or use `mysqldump` inside the MariaDB container.

## Check services are running
- List containers:
```bash
docker ps
```
- Compose status:
```bash
docker-compose -f srcs/docker-compose.yml ps
```
- Follow logs:
```bash
docker-compose -f srcs/docker-compose.yml logs -f
```
- Test HTTP(S):
```bash
curl -k https://localhost/    # -k to ignore self-signed TLS if applicable
```

## Troubleshooting quick tips
- If ports are already in use, free them (443, 3306) or change host mapping in `srcs/docker-compose.yml`.
- If WordPress cannot reach DB, ensure `.env` values match and MariaDB is healthy (`docker-compose logs mariadb`).
- Inspect persisted files under `/home/krfranco/data/...` to confirm write permissions.
