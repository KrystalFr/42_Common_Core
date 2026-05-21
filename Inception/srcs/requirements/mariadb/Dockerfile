FROM debian:bookworm

RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
RUN apt update -y && apt upgrade -y && apt install mariadb-server mariadb-client -y

COPY conf/50-server.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/mariadb_init.sh /docker-entrypoint-initdb.d/mariadb_init.sh
COPY tools/mariadb.sh /mariadb.sh

RUN chmod +x /docker-entrypoint-initdb.d/mariadb_init.sh /mariadb.sh

RUN mkdir -p /run/mysqld && chown -R mysql:mysql /run/mysqld

ENTRYPOINT ["/mariadb.sh"]