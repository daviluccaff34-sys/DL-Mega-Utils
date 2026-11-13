#pragma once
#include <Geode/Geode.hpp>
class DLLog {
public:
    static void info(char const* message) { geode::log::info("[DL-Mega-utils] {}", message); }
    static void warn(char const* message) { geode::log::warn("[DL-Mega-utils] {}", message); }
};
