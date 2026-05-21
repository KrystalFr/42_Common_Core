#!/bin/bash
set -e

mkdir -p /run/mysqld
chown -R mysql:mysql /run/mysqld

# Vérifier si la base de données a déjà été initialisée
if [ ! -f "/var/lib/mysql/.initialized" ]; then
    echo "First run: running initialization..."
    /docker-entrypoint-initdb.d/mariadb_init.sh
    touch /var/lib/mysql/.initialized
else
    echo "Already initialized, skipping setup."
fi

# Démarrer MariaDB
echo "Starting MariaDB..."
exec mysqld --user=mysql --datadir=/var/lib/mysql --socket=/run/mysqld/mysqld.sock