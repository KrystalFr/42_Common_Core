FROM docker.io/library/node:24-alpine
WORKDIR /app

RUN npm install -g npm@12.0.2

COPY package.json package-lock.json ./
RUN npm ci

COPY . .

RUN npm run build

EXPOSE 4173

CMD ["npm", "run", "preview", "--", "--host", "0.0.0.0", "--port", "4173"]
