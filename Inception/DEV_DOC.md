# Developer Documentation

## Prerequisites
- Docker (engine) installed and running.
- Docker Compose (v1 or v2 CLI) available.
- GNU `make` (optional, for convenience).
- Git to clone the repository.
- Ensure ports 443 and 3306 are available on the host (or adjust compose ports).

## Setup from scratch
1. Clone the repo:
```bash
git clone <repo-url>
cd Inception
```
2. Create a `.env` in repo root with at least the DB variables:
```
MYSQL_ROOT_PASSWORD=strong_root_password
MYSQL_USER=wp_user
MYSQL_PASSWORD=wp_password
MYSQL_DATABASE=wordpress
```
3. Ensure host directories for volumes exist (compose may create them, but paths from compose are):
- `/home/krfranco/data/mariadb`
- `/home/krfranco/data/wordpress`
(These are declared in `srcs/docker-compose.yml` as the bind `device` paths.)

## Build and launch (Makefile + Compose)
- Build & start (detached):
```bash
make build
# or directly:
docker-compose -f srcs/docker-compose.yml up -d --build
```
- Stop:
```bash
make down
# or:
docker-compose -f srcs/docker-compose.yml down
```
- Clean (stop + remove volumes):
```bash
make clean
# or:
docker-compose -f srcs/docker-compose.yml down -v
```
- Full cleanup (remove host data and prune):
```bash
make fclean
```
Note: The repository `Makefile` includes `build`, `down`, `clean`, `fclean`, `kill`, and `restart` targets — use them as convenience wrappers.

## Useful developer commands
- View container list:
```bash
docker-compose -f srcs/docker-compose.yml ps
```
- Follow logs:
```bash
docker-compose -f srcs/docker-compose.yml logs -f
```
- Exec into a container:
```bash
docker-compose -f srcs/docker-compose.yml exec wordpress sh
# or for mariadb:
docker-compose -f srcs/docker-compose.yml exec mariadb sh
```
- Rebuild an image (no cache):
```bash
docker-compose -f srcs/docker-compose.yml build --no-cache <service>
```
- Remove dangling resources:
```bash
docker system prune -af --volumes
```

## Data persistence & backups
- WordPress files on host: `/home/krfranco/data/wordpress`
- Database files on host: `/home/krfranco/data/mariadb`
To dump the database:
```bash
docker-compose -f srcs/docker-compose.yml exec mariadb mysqldump -u${MYSQL_USER} -p${MYSQL_PASSWORD} ${MYSQL_DATABASE} > backup.sql
```
(Replace env vars with actual values or export them before running.)

## Where to edit components
- Nginx config: `srcs/requirements/nginx/conf/nginx.conf`
- Nginx Dockerfile: `srcs/requirements/nginx/Dockerfile`
- WordPress Dockerfile and helper scripts: `srcs/requirements/wordpress/`
- MariaDB Dockerfile and config: `srcs/requirements/mariadb/`

Changes to configs generally require rebuilding the image and restarting the service:
```bash
docker-compose -f srcs/docker-compose.yml build <service>
docker-compose -f srcs/docker-compose.yml up -d <service>
```

## Recommended development workflow
1. Edit service config or Dockerfile.
2. Rebuild the service image:
```bash
docker-compose -f srcs/docker-compose.yml build <service>
```
3. Restart the service:
```bash
docker-compose -f srcs/docker-compose.yml up -d --no-deps --build <service>
```
4. Check logs and verify behavior:
```bash
docker-compose -f srcs/docker-compose.yml logs -f <service>
```

## Notes / Caveats
- The compose file binds volumes to absolute host paths owned by `krfranco`. If you run as another user or on a different machine, update the `device` paths or ensure correct ownership/permissions.
- The Makefile in repo provides convenience targets but references `DOCKER_COMPOSE`/`DOCKER_COMPOSE_FILE` variables; if `make` fails, prefer using the explicit `docker-compose -f srcs/docker-compose.yml ...` commands shown above.
- If TLS is configured in `nginx`, your browser may warn about certificates; for local testing `curl -k` or accept the warning.
