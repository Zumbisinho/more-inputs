#include "2.x.x.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "Geode/loader/Setting.hpp"
#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <array>
#include <matjson.hpp>
#include <string>
#include "../keybindsAPI.hpp"
#include <unordered_map>
#include <utility>
#include <vector>

using namespace geode::prelude;

namespace KeyAPIv2 {

std::unordered_set<int> getLevelKeyBindsRaw(CCLayer *layer) {

    std::unordered_set<int> keyRaw;
    auto configJson =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (configJson["keybinds"].size() == 0)
        return keyRaw;

    auto keybinds =
        configJson["keybinds"].as<std::map<std::string,keybindsAPI::KeybindValue>>();
    for (const auto &[action, settings] : *keybinds) {
        keyRaw.insert(settings.keyCode);
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

    auto keybinds = configJson["keybinds"].as<std::map<std::string, keybindsAPI::KeybindValue>>().unwrapOrDefault();
    for (const auto [action, value] : keybinds) {
        if (value.keyCode == -67 && ignoreEmpty)
            continue;
        std::pair<std::string, int> pair = {
            action, value.keyCode
        };
        keys.push_back(pair);
    };
    return keys;
}

}; // namespace KeyAPIv2
