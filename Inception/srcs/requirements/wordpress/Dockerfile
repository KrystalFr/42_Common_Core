FROM debian:bookworm



#On met à jour les paquets et on installe wget pour pouvoir télécharger wordpress
RUN apt update -y && apt upgrade -y && apt install wget tar -y

#On télécharge et on installe php et les extensions nécessaires pour faire fonctionner wordpress
RUN apt install php8.2 php8.2-mysql php8.2-fpm mariadb-client -y

RUN sed -i 's|^listen = .*|listen = 0.0.0.0:9000|' /etc/php/8.2/fpm/pool.d/www.conf

#On télécharge et on installe wordpress, puis on supprime l'archive téléchargée pour ne pas encombrer l'image finale.
RUN mkdir -p /var/www/html/wordpress && wget https://fr.wordpress.org/wordpress-6.9.4-fr_FR.tar.gz 

RUN tar -xvzf wordpress-6.9.4-fr_FR.tar.gz && \
    mv wordpress /var/www/  && \
    rm wordpress-6.9.4-fr_FR.tar.gz


#On installe WP-CLI pour pouvoir gérer wordpress en ligne de commande, ce qui nous permettra de faire l'installation et la configuration de wordpress plus facilement dans le script d'auto-configuration
RUN wget https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar && \
    mv wp-cli.phar /usr/local/bin/wp && \
    chmod +x /usr/local/bin/wp


#On copie le script d'auto-configuration de wordpress et on le rend exécutable
COPY tools/wordpress_config.sh /wordpress_config.sh
RUN chmod +x /wordpress_config.sh


#On lance le serveur php-fpm en mode no-daemon ( -F ) pour que le conteneur reste actif et puisse servir les requêtes PHP de wordpress
CMD ["/usr/sbin/php-fpm8.2", "-F"]

#On définit le point d'entrée du conteneur pour exécuter le script d'auto-configuration de wordpress
ENTRYPOINT [ "/wordpress_config.sh" ]