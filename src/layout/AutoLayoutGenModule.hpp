#pragma once
#include "LayoutProfile.hpp"
#include <vector>
class AutoLayoutGenModule {
public:
    static void initialize();
    static std::vector<LayoutSegment> generatePlan(LayoutProfile const& profile);
};
