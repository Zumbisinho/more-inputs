#include <Geode/Geode.hpp>
#include <matjson.hpp>
#include <unordered_set>
#include <unordered_map>
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace geode::prelude;
namespace keybindsAPI
{
    std::unordered_set<int> getLevelKeyBindsRaw();

    std::unordered_map<std::string, int> getLevelKeyBinds();

    void setLevelKeyBinds(LevelEditorLayer* layer,matjson::Value& json);
}
