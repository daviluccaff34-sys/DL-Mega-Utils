#pragma once
#include <string>
#include <vector>
enum class PackKind { TriggerCombination, ObjectCollection };
struct PackManifest { std::string id, title, author, description, version; PackKind kind; std::vector<std::string> tags; };
