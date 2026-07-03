#include "1.x.x.hpp"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <array>
#include <matjson.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace geode::prelude;





namespace KeyAPIv1 {


std::unordered_set<int> getLevelKeyBindsRaw(CCLayer *layer) {

    std::unordered_set<int> keyRaw;
    auto configJson =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (configJson["keybinds"].size() == 0)
        return keyRaw;

    auto keybinds =
        configJson["keybinds"].as<std::unordered_map<std::string, int>>();
    for (const auto [action, keycode] : *keybinds) {
        keyRaw.insert(keycode);
    };

    return keyRaw;
}

std::vector<std::pair<std::string, int>>
getLevelKeyBinds(CCLayer *layer, bool ignoreEmpty) {

    std::vector<std::pair<std::string, int>> keys;
    auto configJson =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    geode::log::warn("{}", configJson.dump());
    if (configJson["keybinds"].size() == 0)
        return keys;

    auto keybinds = configJson["keybinds"];
    for (const auto [action, keycode] : keybinds) {
        if (keycode == -67 && ignoreEmpty)
            continue;
        std::pair<std::string, int> pair = {
            action, keycode.asInt().unwrapOr(-1)
        };
        keys.push_back(pair);
    };
    return keys;
}


}; // namespace keybindsAPI
