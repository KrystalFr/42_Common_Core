*This project has been created as part of the 42 curriculum by krfranco.*

# Inception — WordPress stack (Nginx + MariaDB + WordPress)

## Description
This repository builds a small WordPress stack using Docker and Docker Compose. The stack provides:
- `mariadb` — MariaDB database (MySQL-compatible)
- `wordpress` — WordPress application
- `nginx` — Reverse proxy and TLS termination serving WordPress

The project is intended for local development and evaluation of container orchestration using Docker Compose. It demonstrates practical setup choices for persistence, networking, and configuration using bind mounts and environment variables.

## Instructions
### Prerequisites
- Docker engine installed and running
- Docker Compose (v1 or v2 CLI) available
- GNU `make` (optional, convenience targets)
- Ensure host ports `443` and `3306` are available or update `srcs/docker-compose.yml`

### Setup from scratch
1. Clone the repository and change to the project directory:
```bash
git clone <repo-url>
cd Inception
```
2. Create a `.env` file in the repository root with at least the database variables:
```
MYSQL_ROOT_PASSWORD=strong_root_password
MYSQL_USER=wp_user
MYSQL_PASSWORD=wp_password
MYSQL_DATABASE=wordpress
```
3. Ensure the host directories for persistence exist (the compose file uses these absolute paths):
- `/home/krfranco/data/mariadb`
- `/home/krfranco/data/wordpress`

### Build and run
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

### Accessing the services
- Website: https://localhost/ (Nginx listens on port 443 per compose)
- WordPress admin: https://localhost/wp-admin

If running remotely, replace `localhost` with the host IP or domain.

### Inspecting status
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
- Exec into a service:
```bash
docker-compose -f srcs/docker-compose.yml exec wordpress sh
```

## Resources
- Docker: https://docs.docker.com/
- Docker Compose: https://docs.docker.com/compose/
- WordPress Developer Resources: https://developer.wordpress.org/
- MariaDB: https://mariadb.org/

## Project description & design choices
This section explains key design decisions and compares alternatives.

- Virtual Machines vs Docker
  - Virtual machines provide strong isolation (separate kernels) and are suitable for production VMs, but are heavier in resources and slower to provision.
  - Docker containers are lightweight, start quickly, and are ideal for packaging services and local development. This project uses Docker to simplify reproducible development environments and fast iteration.

- Secrets vs Environment Variables
  - Environment variables are convenient and supported by Docker Compose (`.env`) for non-sensitive configuration like database names or usernames.
  - Secrets (Docker secrets or external secret stores) are more secure for production secrets (passwords, API keys). For this project we use an `.env` for simplicity; for production migrate secrets to a dedicated secret manager.

- Docker Network vs Host Network
  - Bridge networks (the default) isolate containers and provide DNS-based service discovery. They are preferred for multi-service stacks to avoid host port collisions and to allow finer control of access.
  - Host networking removes network isolation and binds ports directly on the host—useful for low-latency or when container network behavior must match host. This project uses a custom bridge network (`inception`) defined in `srcs/docker-compose.yml`.

- Docker Volumes vs Bind Mounts
  - Docker volumes managed by Docker are portable, easy to backup and are stored in Docker-managed locations.
  - Bind mounts map host directories directly into containers. This project uses bind mounts to host paths under `/home/krfranco/data/...` so data is visible and persistent on the host; update paths if deploying on another machine.

## Where data is stored
- MariaDB data on host: `/home/krfranco/data/mariadb`
- WordPress files on host: `/home/krfranco/data/wordpress`

## How AI was used
AI assistance was used interactively to draft documentation and Makefile suggestions. All content and changes should be reviewed and adjusted to meet your security and production requirements.

## Notes and troubleshooting
- If ports are in use, update `srcs/docker-compose.yml` or stop conflicting services.
- If WordPress cannot connect to the database, verify `.env` values and check `docker-compose -f srcs/docker-compose.yml logs mariadb`.
- If permission issues occur with the bind-mounted directories, ensure the host paths exist and have appropriate ownership/permissions for containers to write.

---

If you want, I can also create a `.env.example` file and update the `Makefile` to list service names automatically.
