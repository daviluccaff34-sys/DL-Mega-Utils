#include "AutoLayoutGenModule.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

void AutoLayoutGenModule::initialize() {
    DLLog::info("Auto Layout Gen: gerador procedural v2 registrado.");
}

std::vector<LayoutSegment> AutoLayoutGenModule::generatePlan(LayoutProfile const& profile) {
    unsigned int count = 12;
    switch (profile.length) {
        case LayoutLength::Short: count = 8; break;
        case LayoutLength::Medium: count = 16; break;
        case LayoutLength::Long: count = 28; break;
    }

    float baseDifficulty = 1.f;
    switch (profile.difficulty) {
        case LayoutDifficulty::Easy: baseDifficulty = .62f; break;
        case LayoutDifficulty::Normal: baseDifficulty = .88f; break;
        case LayoutDifficulty::Hard: baseDifficulty = 1.12f; break;
        case LayoutDifficulty::Insane: baseDifficulty = 1.42f; break;
        case LayoutDifficulty::Demon: baseDifficulty = 1.78f; break;
    }

    float baseSync = 1.f;
    switch (profile.sync) {
        case SyncIntensity::Low: baseSync = .65f; break;
        case SyncIntensity::Normal: baseSync = 1.f; break;
        case SyncIntensity::High: baseSync = 1.4f; break;
    }

    std::mt19937 rng(profile.seed ? profile.seed : std::random_device{}());
    std::uniform_real_distribution<float> variation(.92f, 1.08f);
    std::uniform_int_distribution<int> patternRoll(0, 4);
    std::vector<LayoutSegment> result;
    result.reserve(count);

    for (unsigned int i = 0; i < count; ++i) {
        LayoutSegment segment;
        const float progress = count > 1 ? static_cast<float>(i) / (count - 1) : 0.f;
        const float ramp = .88f + progress * .18f;
        segment.duration = profile.length == LayoutLength::Short ? 2.8f :
            profile.length == LayoutLength::Medium ? 3.8f : 4.6f;
        segment.difficultyWeight = baseDifficulty * ramp * variation(rng);
        segment.syncWeight = baseSync * variation(rng);
        segment.pattern = static_cast<unsigned int>(patternRoll(rng));
        segment.gapAfter = segment.difficultyWeight >= 1.35f && (i % 4 == 3);
        segment.jumpCount = std::clamp(static_cast<int>(std::round(segment.difficultyWeight * 2.f)), 1, 4);
        segment.decorationDensity = .35f + (segment.syncWeight * .25f);
        segment.isDrop = profile.includeDrops && profile.dropCount > 0 &&
            i >= count - std::min(profile.dropCount, count);
        segment.triggers = profile.triggers;
        result.push_back(std::move(segment));
    }
    return result;
}
