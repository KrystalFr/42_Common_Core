# User Documentation

This project provides a WordPress website backed by MariaDB and served through Nginx over HTTPS.

## What the stack provides

- A WordPress site available through Nginx with SSL.
- A MariaDB database to store WordPress content.
- Persistent data stored on the host so the site survives container restarts.

## Before you start

Make sure the configuration files exist before launching the stack:

- `srcs/.env` must contain the expected values.
- The secret files must exist in `secrets/`.

Example values:

```bash
MYSQL_DATABASE=inception_db
MYSQL_USER=inception_user
DOMAIN_NAME=krfranco.42.fr
```

Create the secret files if needed:

```bash
mkdir -p secrets && \
echo "rootpassword" > secrets/db_root_password.txt && \
echo "userpassword" > secrets/db_password.txt && \
echo "adminpassword" > secrets/wp_admin_password.txt
```

## Start and stop the stack

Start the project with:

```bash
sudo make
```

Stop the containers with:

```bash
make down
```

If you want to remove the containers and their volumes:

```bash
make clean
```

## Access the website and admin panel

- Open the domain defined in `srcs/.env` with HTTPS in your browser.
- The website is served on port `443`.
- The admin panel is available at `/wp-admin` after signing in.
- The WordPress admin account is created during the WordPress setup script.

## Manage credentials

- Database and WordPress secrets are stored in the `secrets/` directory.
- Database configuration values are stored in `srcs/.env`.
- To change a password, edit the corresponding secret file and rebuild the stack.

## Basic checks

- Run `docker ps` to confirm the containers are up.
- Visit the website over HTTPS to confirm Nginx and WordPress are responding.
- Check container logs with `docker compose -f srcs/docker-compose.yml logs` if something fails.
