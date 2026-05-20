#!/bin/bash

# Attendre que MariaDB soit prêt avant de configurer WordPress
sleep 10

# Utiliser WP-CLI pour créer le fichier de configuration wp-config.php
wp config create --allow-root \
    --dbname=$MYSQL_DATABASE \
    --dbuser=$MYSQL_USER \
    --dbpass=$MYSQL_PASSWORD \
    --dbhost=mariadb:3306 --path=/var/www/wordpress

# Utiliser WP-CLI pour creer l'utilisateur admin et configurer le site WordPress
wp core install --allow-root \
    --url=https://$DOMAIN_NAME \
    --title="Krystal WordPress" \
    --admin_user=krfranco \
    --admin_password=krfranco \
    --admin_email=krfranco@example.com \
    --skip-email \
    --path=/var/www/wordpress

# Utiliser WP-CLI pour créer un utilisateur supplémentaire 
wp user create --allow-root \
    kruser \
    kruser@example.com \
    --user_pass=kruser \
    --path=/var/www/wordpress

exec "$@"