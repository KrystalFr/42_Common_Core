FROM debian:bookworm

#On met à jour les paquets et on installe nginx pour pouvoir faire fonctionner le serveur web
RUN apt update && apt upgrade -y && apt install nginx -y

#On crée le dossier pour les certificats SSL de nginx
RUN mkdir -p /etc/nginx/ssl

#On installe openssl pour pouvoir générer un certificat auto-signé pour le serveur web.
RUN apt install openssl -y
RUN openssl req -x509 -nodes -out /etc/nginx/ssl/inception.crt -keyout /etc/nginx/ssl/inception.key -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=42/CN=krfranco.42.fr/UID=login"

#On copie les fichiers de configuration personnalisés de nginx pour configurer le serveur web et lui permettre de servir le site wordpress
COPY conf/nginx.conf /etc/nginx/nginx.conf

#On viens modifier les permissions du dossier /var/www pour que le serveur web puisse y accéder et y servir les fichiers de wordpress
RUN chmod -R 755 /var/www/
RUN chown -R www-data:www-data /var/www/

CMD [ "nginx", "-g", "daemon off;" ]