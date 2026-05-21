#!/bin/bash

# Attendre que MariaDB soit prêt avant de configurer WordPress
sleep 10

if [ -f /run/secrets/db_password ]; then
    export MYSQL_PASSWORD=$(cat /run/secrets/db_password)
else
    MYSQL_PASSWORD=${MYSQL_PASSWORD:-krfranco}
fi

# Utiliser WP-CLI pour créer le fichier de configuration wp-config.php
wp config create --allow-root \
    --dbname=$MYSQL_DATABASE \
    --dbuser=$MYSQL_USER \
    --dbpass=$MYSQL_PASSWORD \
    --dbhost=mariadb:3306 --path=/var/www/wordpress


# Lire le mot de passe admin depuis le secret Docker si disponible
if [ -f /run/secrets/wp_admin_password ]; then
    export WP_ADMIN_PASSWORD=$(cat /run/secrets/wp_admin_password)
else
    WP_ADMIN_PASSWORD=${WP_ADMIN_PASSWORD:-krfranco}
fi

# Utiliser WP-CLI pour creer l'utilisateur admin et configurer le site WordPress
wp core install --allow-root \
        --url=https://$DOMAIN_NAME \
        --title="Krystal WordPress" \
        --admin_user=krfranco \
        --admin_password="$WP_ADMIN_PASSWORD" \
        --admin_email=krfranco@example.com \
        --skip-email \
        --path=/var/www/wordpress

# Utiliser WP-CLI pour créer un utilisateur supplémentaire 
wp user create --allow-root \
    kruser \
    kruser@example.com \
    --user_pass=kruser \
    --path=/var/www/wordpress\
    --role=author

exec "$@"