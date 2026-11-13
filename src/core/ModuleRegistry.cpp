#include "ModuleRegistry.hpp"
#include "Logger.hpp"
#include "../decoration/DecorationModule.hpp"
#include "../layout/AutoLayoutGenModule.hpp"
#include "../marketplace/TriggerMarketModule.hpp"
#include "../marketplace/ObjectMarketModule.hpp"
#include "../editor/EditorToolsModule.hpp"
void ModuleRegistry::initialize() {
    DecorationModule::initialize();
    AutoLayoutGenModule::initialize();
    TriggerMarketModule::initialize();
    ObjectMarketModule::initialize();
    EditorToolsModule::initialize();
    DLLog::info("Registro de módulos concluído.");
}
