#pragma once
#include <vector>
enum class LayoutDifficulty { Easy, Normal, Hard, Insane, Demon };
enum class LayoutLength { Short, Medium, Long };
enum class SyncIntensity { Low, Normal, High };
enum class LayoutTrigger { Move, Alpha, Pulse, Toggle, Spawn, Rotate, Follow, Camera };
struct LayoutProfile {
    LayoutDifficulty difficulty = LayoutDifficulty::Normal;
    LayoutLength length = LayoutLength::Medium;
    SyncIntensity sync = SyncIntensity::Normal;
    std::vector<LayoutTrigger> triggers;
    bool includeDrops = true;
    unsigned int dropCount = 1;
    unsigned int seed = 0;
};
struct LayoutSegment {
    float duration = 0.f;
    float difficultyWeight = 1.f;
    float syncWeight = 1.f;
    bool isDrop = false;
    std::vector<LayoutTrigger> triggers;
};
