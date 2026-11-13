const json = (data, status = 200) => new Response(JSON.stringify(data), {
  status,
  headers: {
    "content-type": "application/json; charset=utf-8",
    "access-control-allow-origin": "*",
    "access-control-allow-methods": "GET, OPTIONS",
    "access-control-allow-headers": "Content-Type"
  }
});

export default {
  async fetch(request, env) {
    const url = new URL(request.url);

    if (request.method === "OPTIONS") {
      return new Response(null, {
        headers: {
          "access-control-allow-origin": "*",
          "access-control-allow-methods": "GET, OPTIONS",
          "access-control-allow-headers": "Content-Type"
        }
      });
    }

    if (request.method !== "GET") {
      return json({ ok: false, error: "Method not allowed" }, 405);
    }

    if (url.pathname === "/health") {
      return json({ ok: true, service: "DL-Mega-Utils API", storage: "Workers KV" });
    }

    if (url.pathname === "/api/packs") {
      const type = url.searchParams.get("type");
      if (!["objects", "triggers"].includes(type)) {
        return json({ ok: false, error: "Use type=objects or type=triggers" }, 400);
      }

      const raw = await env.DL_MEGA_UTILS.get(type + ":index");
      let packs = [];
      try {
        packs = raw ? JSON.parse(raw) : [];
      } catch {
        return json({ ok: false, error: "Invalid catalog data" }, 500);
      }
      return json({ ok: true, type, packs });
    }

    const match = url.pathname.match(/^\/api\/packs\/(objects|triggers)\/([a-zA-Z0-9_-]+)$/);
    if (match) {
      const value = await env.DL_MEGA_UTILS.get(match[1] + ":pack:" + match[2]);
      if (!value) return json({ ok: false, error: "Pack not found" }, 404);
      try {
        return json({ ok: true, type: match[1], pack: JSON.parse(value) });
      } catch {
        return json({ ok: false, error: "Invalid pack data" }, 500);
      }
    }

    return json({ ok: false, error: "Not found" }, 404);
  }
};
