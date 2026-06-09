*This project has been created as part of the 42 curriculum by krfranco.*

Description
-----------

This repository contains a Docker-based deployment for a small webstack used in the 42 "Inception" project. The goal is to run a production-like WordPress site using multiple containers orchestrated with `docker-compose`: an Nginx reverse proxy, a WordPress container, and a MariaDB database. The setup demonstrates container networking, persistent storage, and basic service configuration.

Instructions
------------

Prerequisites:
- Install Docker and Docker Compose on your machine.
- Set the required values in [srcs/.env](srcs/.env) and create the secret files under [secrets/](secrets/).

Create a secrets folder at root repo and set passwords:
```bash
mkdir -p secrets && \
echo "rootpassword" > secrets/db_root_password.txt && \
echo "userpassword" > secrets/db_password.txt && \
echo "adminpassword" > secrets/wp_admin_password.txt
```

Build and run the stack:

The simple way is:

```bash
sudo make
```

But if you want to do it manually:

```bash
docker-compose -f srcs/docker-compose.yml build
docker-compose -f srcs/docker-compose.yml up -d
```

Stop and remove containers:

```bash
docker-compose -f srcs/docker-compose.yml down
```

If you need to re-create volumes (data will be removed):

```bash
docker-compose -f srcs/docker-compose.yml down -v
```

Where to look for configuration files:
- Nginx configuration: [srcs/requirements/nginx/conf/nginx.conf](srcs/requirements/nginx/conf/nginx.conf)
- MariaDB Dockerfile and init scripts: [srcs/requirements/mariadb/](srcs/requirements/mariadb/)
- WordPress Dockerfile and setup scripts: [srcs/requirements/wordpress/](srcs/requirements/wordpress/)
- User documentation: [USER_DOC.md](USER_DOC.md)
- Developer documentation: [DEV_DOC.md](DEV_DOC.md)

Resources
---------

- Docker documentation: https://docs.docker.com/
- Docker Compose: https://docs.docker.com/compose/
- Nginx: https://nginx.org/en/docs/
- MariaDB: https://mariadb.org/
- WordPress: https://wordpress.org/
- My left neighbour

Project description and design choices
--------------------------------------

Virtual Machines vs Docker
- Virtual Machines: provide full OS isolation (heavier, larger images, slower startup). Good for running different OS kernels or full OS-level isolation.
- Docker (containers): lightweight process-level isolation using the host kernel, faster startup, smaller images, easier to distribute and scale. This project uses Docker for faster iteration and reproducible environments.

Secrets vs Environment Variables
- Environment variables are simple and commonly used for configuration (e.g., DB_USER, DB_PASSWORD). They can be exposed in process lists or compose files if not handled carefully.
- Docker secrets (or external secret managers) provide safer handling for sensitive data, preventing accidental leakage in images or repo. Use secrets for production credentials; env vars are acceptable for local development with caution.

Docker Network vs Host Network
- Docker bridge/network isolates container networking and allows port mappings and controlled connectivity between containers.
- Host network gives containers direct access to the host network stack (no NAT). It can solve some networking limitations but reduces isolation and may cause port conflicts. This project uses an isolated Docker network so services can communicate securely and predictably.

Docker Volumes vs Bind Mounts
- Volumes managed by Docker are portable and recommended for persistent data in production (managed by Docker, can be backed up and migrated).
- Bind mounts map host directories into containers and are useful for local development and debugging (easy to edit files on the host), but can cause permission inconsistencies.

How AI was used
-----------------

- AI assistance was used to draft and structure this README file and to summarize design choices and example commands. Implementation, configuration files, and scripts in the repository were not modified by AI.