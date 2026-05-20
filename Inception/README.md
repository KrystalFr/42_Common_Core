*This project has been created as part of the 42 curriculum by krfranco.*

# Inception (WordPress stack)

Description
- Lightweight Docker Compose stack that deploys a small WordPress site using:
  - `mariadb` (database)
  - `wordpress` (PHP-FPM + WP-CLI)
  - `nginx` (reverse proxy + TLS)

Goal: provide a reproducible development/demo environment that demonstrates
containerized services, persistent data via bind mounts, and basic secrets usage.

Instructions
- Prerequisites: Docker Engine (v20+), Docker Compose (v2+), Git.
- Configure secrets: place the following files under the repository `secrets/` folder:
  - `db_password.txt` — MariaDB user password
  - `db_root_password.txt` — MariaDB root password
  - `wp_admin_password.txt` — WordPress administrator password (optional)
- Update `srcs/.env` if you want to change domain or database names.
- Build and run (from project root):

```bash
make build   # creates data dirs and starts the stack (detached, builds images)
```

- Stop/remove the stack:

```bash
make down    # stops containers
make fclean  # stops containers and removes host data used by the stack
```

Quick verification
- Open https://<DOMAIN_NAME> (value in `srcs/.env`) to see the site.
- To tail logs:

```bash
docker compose -f srcs/docker-compose.yml logs -f
```

Resources
- Docker: https://docs.docker.com/
- Docker Compose: https://docs.docker.com/compose/
- WordPress: https://wordpress.org/
- MariaDB: https://mariadb.org/
- Nginx: https://nginx.org/
- WP-CLI: https://wp-cli.org/

Project design notes
- Virtual Machines vs Docker: Docker was chosen for faster startup, smaller
  resource usage and easier image builds. VMs provide stronger isolation but
  are heavier to run for a small dev/demo site.
- Secrets vs Environment Variables: Sensitive values (DB passwords, admin
  password) are stored in Docker `secrets/` files and consumed by the
  containers. Non-sensitive configuration (DB name, domain) is stored in
  `srcs/.env` to keep deployment flexible and visible to developers.
- Docker Network vs Host Network: The stack uses a dedicated bridge network
  (`inception`) so services can communicate by name (e.g., `mariadb`,
  `wordpress`) while remaining isolated from other host services. Host network
  would expose container ports directly on the host and reduce isolation.
- Docker Volumes vs Bind Mounts: This project uses bind-mounted host folders
  (configured in `docker-compose.yml` `driver_opts.device`) so data is stored
  under `/home/krfranco/data/...` and is directly accessible from the host.
  Bind mounts make inspection and backups easier during development.

Additional notes
- The project expects the host directories referenced in `docker-compose.yml`.
  Edit the paths or create the directories before running the stack if needed.
