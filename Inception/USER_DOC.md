# User documentation

This document explains how an end user or administrator can use the project.

Services provided
- `mariadb` — stores WordPress data (database).
- `wordpress` — PHP-FPM container serving the WordPress application; contains
  WP-CLI for automated setup.
- `nginx` — TLS-terminating reverse proxy serving the site over HTTPS and
  forwarding PHP requests to `wordpress`.

Start and stop the project
- Start (build images and run in background):

```bash
make build
```

- Stop:

```bash
make down
```

- Remove everything (data will be removed):

```bash
make fclean
```

- Force-stop without graceful shutdown:

```bash
make kill
```

- Rebuild from scratch (clean then build):

```bash
make restart
```

Access the website and admin panel
- Website: open `https://<DOMAIN_NAME>` where `<DOMAIN_NAME>` is set in
  `srcs/.env` (default in repository: `krfranco.42.fr`).
- Admin panel: `https://<DOMAIN_NAME>/wp-admin`
- Default admin user created by the setup script: `krfranco`.
  - Password is read from `secrets/wp_admin_password.txt` if present; otherwise
    a fallback password is used. Check `secrets/wp_admin_password.txt` or the
    `wordpress_config.sh` script for details.

Locate and manage credentials
- Non-sensitive config: `srcs/.env` (database name, user, domain).
- Sensitive credentials: the repo `secrets/` folder contains the files used by
  the containers. Do not commit real passwords.
  - `secrets/db_password.txt` — password for the DB user
  - `secrets/db_root_password.txt` — MariaDB root password
  - `secrets/wp_admin_password.txt` — WordPress admin password (optional)

Check services running correctly
- Quick container status:

```bash
docker compose -f srcs/docker-compose.yml ps
```

- Tail logs for a specific service:

```bash
docker compose -f srcs/docker-compose.yml logs -f nginx
```

- Verify site in browser on HTTPS. If TLS errors appear in local dev, the
  stack uses a self-signed certificate generated inside the `nginx` image.

Common tasks
- Reset WordPress admin password using WP-CLI (run inside `wordpress` container):

```bash
docker compose -f srcs/docker-compose.yml exec wordpress wp user update krfranco --user_pass=<newpass> --allow-root --path=/var/www/wordpress
```

- Connect to MariaDB (from host):

```bash
docker compose -f srcs/docker-compose.yml exec mariadb mysql -u root -p
# then enter the root password from secrets/db_root_password.txt
```
