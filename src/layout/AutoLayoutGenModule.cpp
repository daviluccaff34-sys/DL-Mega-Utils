#include "AutoLayoutGenModule.hpp"
#include "../core/Logger.hpp"
#include <algorithm>
#include <random>
void AutoLayoutGenModule::initialize() { DLLog::info("Auto Layout Gen: gerador de planos registrado."); }
std::vector<LayoutSegment> AutoLayoutGenModule::generatePlan(LayoutProfile const& profile) {
    unsigned int count = 12;
    switch (profile.length) {
        case LayoutLength::Short: count = 6; break;
        case LayoutLength::Medium: count = 12; break;
        case LayoutLength::Long: count = 20; break;
    }
    float difficulty = 0.9f;
    switch (profile.difficulty) {
        case LayoutDifficulty::Easy: difficulty = 0.65f; break;
        case LayoutDifficulty::Normal: difficulty = 0.9f; break;
        case LayoutDifficulty::Hard: difficulty = 1.2f; break;
        case LayoutDifficulty::Insane: difficulty = 1.55f; break;
        case LayoutDifficulty::Demon: difficulty = 1.9f; break;
    }
    float sync = 1.f;
    switch (profile.sync) {
        case SyncIntensity::Low: sync = 0.65f; break;
        case SyncIntensity::Normal: sync = 1.f; break;
        case SyncIntensity::High: sync = 1.4f; break;
    }
    std::mt19937 rng(profile.seed ? profile.seed : std::random_device{}());
    std::uniform_real_distribution<float> variation(0.85f, 1.15f);
    std::vector<LayoutSegment> result;
    result.reserve(count);
    for (unsigned int i = 0; i < count; ++i) {
        LayoutSegment segment;
        segment.duration = profile.length == LayoutLength::Short ? 2.5f :
            profile.length == LayoutLength::Medium ? 4.f : 5.5f;
        segment.difficultyWeight = difficulty * variation(rng);
        segment.syncWeight = sync * variation(rng);
        segment.isDrop = profile.includeDrops && profile.dropCount > 0 &&
            i >= count - std::min(profile.dropCount, count);
        segment.triggers = profile.triggers;
        result.push_back(std::move(segment));
    }
    return result;
}
