FROM docker.io/library/node:24-alpine AS build
WORKDIR /app
RUN npm install -g npm@12.0.2
RUN apk add --no-cache openssl libc6-compat
COPY package.json package-lock.json ./
RUN npm ci
COPY prisma.config.ts ./
COPY prisma ./prisma
RUN npx prisma generate
COPY . .
RUN npm run build

FROM docker.io/library/node:24-alpine AS runtime
WORKDIR /app
ENV NODE_ENV=production
RUN npm install -g npm@12.0.2
RUN apk add --no-cache openssl libc6-compat
RUN apk add --no-cache ffmpeg
COPY package.json ./
COPY --from=build /app/node_modules ./node_modules
COPY --from=build /app/dist ./dist
COPY --from=build /app/prisma ./prisma
COPY --from=build /app/prisma.config.ts ./prisma.config.ts
RUN mkdir -p /app/uploads/avatars && chown -R node:node /app
USER node
EXPOSE 3000
CMD ["sh", "-c", "npx prisma migrate deploy && npx prisma db seed && exec node dist/main.js"]
