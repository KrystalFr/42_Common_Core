# User documentation

This short guide explains, in simple terms, what the stack provides and how an end user or administrator can operate it.

## What services are provided

- Web site: WordPress served by Nginx over HTTPS.
- Database: MariaDB to store WordPress data.

The stack is orchestrated by Docker Compose and the configuration lives in `srcs/docker-compose.yml`.

## Start and stop the project

- Start (build + run detached):

  `make build`

- Start (no build):

  `make up`

- Stop services:

  `make down`

- Completely remove data and prune images (destructive):

  `make fclean`

## Access the website and administration panel

- Open a browser to `https://<DOMAIN_NAME>` replacing `<DOMAIN_NAME>` with the value in `srcs/.env` (for local testing you may use the host IP or edit `/etc/hosts` to point the domain to the host).
- Admin panel: `https://<DOMAIN_NAME>/wp-admin`

Default admin user configured by the provisioning script is `krfranco` (see `srcs/requirements/wordpress/tools/wordpress_config.sh`). The admin password is read from a Docker secret `wp_admin_password` if present, otherwise a fallback `krfranco` is used. For production change the password immediately.

## Locate and manage credentials

- Secrets files (if used) are expected in `./secrets/` and are referenced by `srcs/docker-compose.yml`.
- Example secret names referenced: `db_password`, `db_root_password`, `wp_admin_password`.
- Non-sensitive configuration is in `srcs/.env`. Update domain and database name there.

## Check that services are running correctly

- List containers and status:

  `docker compose -f srcs/docker-compose.yml ps`

- Check logs for a service (example nginx):

  `docker compose -f srcs/docker-compose.yml logs nginx --tail=200`

- Quick health check (from host):

  `curl -k https://$(grep DOMAIN_NAME srcs/.env | cut -d'=' -f2)`

If you need help resetting the admin password, exporting/importing a database dump, or migrating content, tell me the preferred workflow and I can add step-by-step instructions.
