#!/bin/bash


# Démarre MariaDB en arrière-plan (sans réseau)
mysqld --skip-networking &
pid="$!"

# Attend que le socket soit prêt
for i in {30..0}; do
    if [ -S "/run/mysqld/mysqld.sock" ]; then
        break
    fi
    sleep 1
done
if [ ! -S "/run/mysqld/mysqld.sock" ]; then
    echo "MariaDB socket non disponible, échec du démarrage."
    exit 1
fi


# Créer la base de données
mysql -u root -e "CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;"

# Créer l'utilisateur
mysql -u root -e "CREATE USER IF NOT EXISTS \`${MYSQL_USER}\`@'%' IDENTIFIED BY '${MYSQL_PASSWORD}';"

# Donner tous les droits à l'utilisateur sur la base
mysql -u root -e "GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO \`${MYSQL_USER}\`@'%';"

# Modifier le mot de passe root
mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY '${MYSQL_ROOT_PASSWORD}';"

# Rafraîchir les privilèges
mysql -u root -e "FLUSH PRIVILEGES;"

# Arrêter MariaDB temporaire
mysqladmin -u root -p${MYSQL_ROOT_PASSWORD} shutdown

# Lancer MariaDB en mode foreground
exec mysqld