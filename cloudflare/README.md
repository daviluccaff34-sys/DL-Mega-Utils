# DL-Mega-Utils Cloudflare API

Worker: `https://dl-mega-utils-api.daviluccaff34.workers.dev`

Storage binding: `DL_MEGA_UTILS` (Workers KV namespace `DL_MEGA_UTILS`).

## Public read endpoints

- `GET /health` — checks that the API is responding.
- `GET /api/packs?type=objects` — lists object packs.
- `GET /api/packs?type=triggers` — lists trigger packs.
- `GET /api/packs/objects/{id}` — reads one object pack.
- `GET /api/packs/triggers/{id}` — reads one trigger pack.

## KV key layout

- `objects:index` — JSON array of object pack summaries.
- `triggers:index` — JSON array of trigger pack summaries.
- `objects:pack:{id}` — JSON for one object pack.
- `triggers:pack:{id}` — JSON for one trigger pack.

Both indexes are initialized as empty arrays. The public Worker is read-only; pack publishing should be done through a protected admin path, not an unauthenticated public write endpoint.

## Deploy

Run Wrangler from this directory with the Cloudflare account connected:

```sh
wrangler deploy
```
