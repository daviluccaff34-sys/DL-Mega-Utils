#include <Geode/Geode.hpp>
#include "core/ModuleRegistry.hpp"
#include "core/Logger.hpp"
using namespace geode::prelude;
$on_mod(Loaded) {
    DLLog::info("DL-Mega-utils carregado.");
    ModuleRegistry::initialize();
}
