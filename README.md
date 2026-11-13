# DL-Mega-utils

Mod independente para Geometry Dash, direcionado ao Geode SDK 5.11.2 e Geometry Dash 2.2081 (Android64 como alvo inicial).

## Sistemas planejados

- **Auto Decoration:** presets NULL, HELL, TECHNO e MODERN. A decoração automática com objetos reais ainda será implementada.
- **Auto Layout Gen:** gera um plano abstrato a partir de dificuldade, duração, sincronização, triggers e drops. Ainda não insere objetos no nível.
- **Trigger Marketplace:** compartilhar/baixar conjuntos de triggers e aplicá-los a um grupo escolhido (planejado).
- **Object Marketplace:** compartilhar/baixar coleções de objetos independentes de grupos (planejado).
- **Editor Tools:** ferramentas adicionais para criação (planejadas).

### Parâmetros do Auto Layout Gen

- Dificuldade: EASY, NORMAL, HARD, INSANE, DEMON.
- Tempo: SHORT, MEDIUM, LONG.
- Sincronização: LOW, NORMAL, HIGH.
- Triggers: Move, Alpha, Pulse, Toggle, Spawn, Rotate, Follow e Camera.
- Drops: ativação e quantidade.

**Estado atual:** o gerador produz apenas um plano de segmentos para validar os parâmetros. Não cria uma fase jogável nem objetos editáveis dentro do Geometry Dash. A integração futura deverá usar as APIs do editor e incluir pré-visualização e opção de desfazer.

## Build

Requer Geode SDK 5.11.2 configurado em `GEODE_SDK`.

```bash
cmake -B build
cmake --build build --config Release
```

O workflow de GitHub Actions está configurado para tentar compilar Android64 e publicar o artefato de build. A compilação ainda precisa ser confirmada.
