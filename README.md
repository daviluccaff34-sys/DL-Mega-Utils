# DL-Mega-utils

Mod independente para Geometry Dash 2.2081, usando Geode SDK 5.11.2 e compilação Android64.

## Interface
- Bola flutuante arrastável no menu e no editor.
- Toque na bola para abrir o painel; botão X para fechar.

## Ferramentas do editor
- **Auto Layout Gen:** gera segmentos a partir da dificuldade, duração, sincronização e drops e tenta inserir blocos, espinhos e orbes como objetos reais no editor aberto.
- **Auto Decoration:** os presets HELL, TECHNO e MODERN inserem padrões simples de objetos no entorno dos objetos existentes. NULL não adiciona elementos.
- **Trigger Market e Object Market:** as telas ainda aguardam o backend online; upload, download e aplicação de packs ainda não estão disponíveis.
- **Editor Tools:** ainda não implementado.

## Cloudflare
O armazenamento planejado usa R2 para os arquivos dos packs e um Worker como API de listagem, upload e download. A ativação do R2 na conta Cloudflare é necessária antes de criar o bucket e publicar a API.

## Build
O workflow do GitHub Actions compila Android64 com Geode SDK 5.11.2 em pushes para main e tags v*.
