# User Documentation

This project provides a WordPress website backed by MariaDB and served through Nginx over HTTPS.

## What the stack provides

- A WordPress site available through Nginx with SSL.
- A MariaDB database to store WordPress content.
- Persistent data stored on the host so the site survives container restarts.

## How to start and stop the project

Before starting, make sure `srcs/.env` contains the expected values and the secret files exist in `secrets/`.

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

## How to access the website and admin panel

- Open the domain defined in `srcs/.env` with HTTPS in your browser.
- The admin panel is available at `/wp-admin` after signing in.
- The admin login is created during the WordPress setup script.
- The site is served over port 443.

## How to locate and manage credentials

- Database and WordPress secrets are stored in the `secrets/` directory.
- Database configuration values are stored in `srcs/.env`.
- If you need to update a password, edit the corresponding secret file and rebuild the stack.

## How to check that services are running correctly

- Run `docker ps` to confirm the containers are up.
- Visit the website over HTTPS to confirm Nginx and WordPress are responding.
- Check container logs with `docker compose -f srcs/docker-compose.yml logs` if something fails.
