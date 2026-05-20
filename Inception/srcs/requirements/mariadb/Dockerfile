FROM debian:bookworm

#On met à jour les paquets et on installe mariadb-server et mariadb-client pour pouvoir faire fonctionner la base de données nécessaire à wordpress
RUN apt update -y && apt upgrade -y && apt install mariadb-server mariadb-client -y

#On copie le fichier de configuration personnalisé de mariadb pour permettre les connexions à distance depuis le conteneur wordpress
COPY conf/50-server.cnf /etc/mysql/mariadb.conf.d/50-server.cnf

#On copie le script d'initialisation de la base de données et on le rend exécutable 
COPY tools/mariadb.sh /docker-entrypoint-initdb.d/mariadb.sh
RUN chmod +x /docker-entrypoint-initdb.d/mariadb.sh

#On crée le dossier de données de mariadb pour wordpress et on change les permissions pour que mariadb puisse y accéder et y écrire
RUN mkdir -p /run/mysqld && chown -R mysql:mysql /run/mysqld

#Lance le script d'initialisation personnalisé
ENTRYPOINT ["/docker-entrypoint-initdb.d/mariadb.sh"]