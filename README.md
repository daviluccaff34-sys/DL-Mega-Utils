# DL-Mega-utils

Mod independente para Geometry Dash, direcionado ao Geode SDK 5.11.2 e Geometry Dash 2.2081 (Android64 como alvo inicial).

## Interface

- Bola flutuante arrastável no menu principal e no editor.
- Toque na bola para abrir o painel; botão **X** para fechar.
- Navegação entre Auto Decoration, Auto Layout Gen, Trigger Market, Object Market e Editor Tools.

## Estado das ferramentas

- **Auto Decoration:** seleção de presets NULL, HELL, TECHNO e MODERN está disponível na interface. A aplicação automática de objetos no nível ainda não está implementada.
- **Auto Layout Gen:** os controles de dificuldade, duração, sincronização e drops funcionam. O botão gera um plano de segmentos e mostra o resumo; ainda não cria objetos reais nem altera a fase.
- **Trigger Marketplace:** tela de entrada pronta; envio, download e aplicação de packs ainda não estão conectados.
- **Object Marketplace:** tela de entrada pronta; biblioteca online e importação ainda não estão conectadas.
- **Editor Tools:** tela de entrada criada; ferramentas específicas ainda serão implementadas.

## Build

Requer Geode SDK 5.11.2 configurado em `GEODE_SDK`.

```bash
cmake -B build
cmake --build build --config Release
```

O workflow do GitHub Actions compila Android64 em pushes para `main` e em tags `v*`.
