#pragma once
#include <string>
#include <vector>
enum class DecorationStyle { Null, Hell, Techno, Modern };
struct StylePreset {
    DecorationStyle style;
    std::string displayName;
    std::vector<std::string> tags;
};
