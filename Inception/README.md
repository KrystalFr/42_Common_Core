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

AI usage
- I used AI to catch some synthax error, but also to help me debug by explaining error messages and give me suggestions on what in my code could be going wrong.
- This README was also typed and phrased with assistance from an AI to ensure I gave all the required information, but I still read and approved all content and no AI-generated code was introduced without review.

Additional notes
- The project expects the host directories referenced in `docker-compose.yml`.
  Edit the paths or create the directories before running the stack if needed.
