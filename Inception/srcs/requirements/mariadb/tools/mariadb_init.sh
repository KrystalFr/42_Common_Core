#!/bin/bash
set -e

# Lire les mots de passe depuis les secrets Docker
MYSQL_ROOT_PASSWORD=$(cat /run/secrets/db_root_password)
MYSQL_PASSWORD=$(cat /run/secrets/db_password)

# Initialiser le répertoire de données de MariaDB
echo "Initializing data directory..."
mysql_install_db --user=mysql --datadir=/var/lib/mysql > /dev/null

# Démarrer MariaDB temporairement pour la configuration
echo "Starting temporary MariaDB for setup..."
mysqld --skip-networking --socket=/run/mysqld/mysqld.sock --user=mysql &
pid="$!"

# Attendre que MariaDB soit prêt
until mysqladmin --socket=/run/mysqld/mysqld.sock ping >/dev/null 2>&1; do
    sleep 1
done

# Exécuter les commandes SQL pour configurer la base de données et les utilisateurs
echo "Running setup SQL..."
mysql --socket=/run/mysqld/mysqld.sock -u root <<-EOF
    ALTER USER 'root'@'localhost' IDENTIFIED BY '${MYSQL_ROOT_PASSWORD}';
    CREATE DATABASE IF NOT EXISTS \`${MYSQL_DATABASE}\`;
    CREATE USER IF NOT EXISTS '${MYSQL_USER}'@'%' IDENTIFIED BY '${MYSQL_PASSWORD}';
    GRANT ALL PRIVILEGES ON \`${MYSQL_DATABASE}\`.* TO '${MYSQL_USER}'@'%';
    FLUSH PRIVILEGES;
EOF

# Arrêter MariaDB temporaire après la configuration
echo "Shutting down temporary MariaDB..."
mysqladmin --socket=/run/mysqld/mysqld.sock -u root -p"${MYSQL_ROOT_PASSWORD}" shutdown
wait "$pid" || true

echo "Initialization complete!"