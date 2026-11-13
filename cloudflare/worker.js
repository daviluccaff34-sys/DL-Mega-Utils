const CORS = {
  "access-control-allow-origin": "*",
  "access-control-allow-methods": "GET, POST, OPTIONS",
  "access-control-allow-headers": "Content-Type, X-Publish-Password"
};

const json = (data, status = 200) => new Response(JSON.stringify(data), {
  status,
  headers: { ...CORS, "content-type": "application/json; charset=utf-8" }
});

const validType = type => type === "objects" || type === "triggers";
const validId = id => typeof id === "string" && /^[a-zA-Z0-9_-]{3,48}$/.test(id);
const validName = name => typeof name === "string" && name.trim().length >= 2 && name.trim().length <= 48;

async function readIndex(kv, type) {
  const raw = await kv.get(type + ":index");
  if (!raw) return [];
  const parsed = JSON.parse(raw);
  return Array.isArray(parsed) ? parsed : [];
}

export default {
  async fetch(request, env) {
    const url = new URL(request.url);
    if (request.method === "OPTIONS") return new Response(null, { headers: CORS });

    if (url.pathname === "/health" && request.method === "GET") {
      return json({ ok: true, service: "DL-Mega-Utils API", storage: "Workers KV", features: ["trigger-packs", "object-packs"] });
    }

    if (url.pathname === "/api/packs" && request.method === "GET") {
      const type = url.searchParams.get("type");
      if (!validType(type)) return json({ ok: false, error: "Use type=objects or type=triggers" }, 400);
      try {
        return json({ ok: true, type, packs: await readIndex(env.DL_MEGA_UTILS, type) });
      } catch {
        return json({ ok: false, error: "Invalid catalog data" }, 500);
      }
    }

    const match = url.pathname.match(/^\/api\/packs\/(objects|triggers)\/([a-zA-Z0-9_-]+)$/);
    if (match && request.method === "GET") {
      const value = await env.DL_MEGA_UTILS.get(match[1] + ":pack:" + match[2]);
      if (!value) return json({ ok: false, error: "Pack not found" }, 404);
      try { return json({ ok: true, type: match[1], pack: JSON.parse(value) }); }
      catch { return json({ ok: false, error: "Invalid pack data" }, 500); }
    }

    if (url.pathname === "/api/admin/packs" && request.method === "POST") {
      if (!env.ADMIN_PASSWORD) return json({ ok: false, error: "Password publishing is not configured" }, 503);
      const suppliedPassword = request.headers.get("x-publish-password") || "";
      if (suppliedPassword !== env.ADMIN_PASSWORD) return json({ ok: false, error: "Invalid publishing password" }, 403);

      let body;
      try { body = await request.json(); }
      catch { return json({ ok: false, error: "Invalid JSON" }, 400); }

      const { type, id, name, description = "", data } = body || {};
      if (!validType(type) || !validId(id) || !validName(name)) {
        return json({ ok: false, error: "Invalid type, id or name" }, 400);
      }
      if (typeof data !== "string" || data.length < 1 || data.length > 45000) {
        return json({ ok: false, error: "Pack data must be a string up to 45 KB" }, 400);
      }
      if (typeof description !== "string" || description.length > 240) {
        return json({ ok: false, error: "Description is too long" }, 400);
      }

      const pack = {
        id, type, name: name.trim(), description: description.trim(),
        data, updatedAt: new Date().toISOString()
      };
      try {
        const index = await readIndex(env.DL_MEGA_UTILS, type);
        const entry = { id, name: pack.name, description: pack.description, updatedAt: pack.updatedAt };
        const next = [entry, ...index.filter(item => item && item.id !== id)].slice(0, 500);
        await env.DL_MEGA_UTILS.put(type + ":pack:" + id, JSON.stringify(pack));
        await env.DL_MEGA_UTILS.put(type + ":index", JSON.stringify(next));
        return json({ ok: true, published: entry });
      } catch {
        return json({ ok: false, error: "Storage operation failed" }, 500);
      }
    }

    if (url.pathname.startsWith("/api/admin/") && request.method !== "POST") {
      return json({ ok: false, error: "Method not allowed" }, 405);
    }
    return json({ ok: false, error: "Not found" }, 404);
  }
};
