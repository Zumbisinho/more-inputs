#include "keybindsAPI.hpp"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// ! IDENTIFICAR URGENTE COMO DETECTA JSON VAZIO

using namespace geode::prelude;

namespace keybindsAPI {
std::string versionStr = Mod::get()->getVersion().toVString();
matjson::Value getDefaultJson() {
    matjson::Value keybinds = matjson::Value::object();

    matjson::Value templateJson = matjson::Value::object();
    templateJson["version"] = versionStr;
    templateJson["keybinds"] = keybinds;

    return templateJson;
};
std::string containsValue(const matjson::Value &obj, int target) {
    if (!obj.isObject())
        return "";

    for (auto const &[actionName, value] : obj) {
        if (value.isNumber() && value.asInt().unwrapOr(0) == target) {
            return actionName;
        }
    }
    return "";
}
void addKeyToJson(
    std::string actionName,
    int actionKeyCode,
    matjson::Value *json,
    bool replaceEmpty = true
) {
    if ((*json)["keybinds"].isObject() == false)
        return;
    if (replaceEmpty) {
        std::string emptyActionName = containsValue((*json)["keybinds"], -67);
        log::info("AddKeyToJson, empty: {}", emptyActionName);
        if (emptyActionName != "") // replaces empty section
        {
            editKeybind(json, emptyActionName, actionName, actionKeyCode);
            return;
        }
    }
    (*json)["keybinds"][actionName] = actionKeyCode;
}
void editKeybind(
    matjson::Value *json,
    const std::string &oldAction,
    const std::string &newAction,
    int newKeyCode
) {

    if (!json || !json->isObject()) // how do you even get here?
        return;

    matjson::Value newObj = getDefaultJson();
    bool asFounded = false;
    for (auto const &[key, value] : (*json)["keybinds"]) {
        if (key == oldAction && !asFounded) {
            newObj["keybinds"][newAction] = newKeyCode;
            asFounded = true;
        } else {
            newObj["keybinds"][key] = value.asInt().unwrap();
        }
    }

    *json = newObj;
}

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
    if (configJson["keybinds"].size() == 0)
        return keys;

    auto keybinds = configJson["keybinds"];
    for (const auto [action, keycode] : keybinds) {
        if (keycode == -67 && ignoreEmpty)
            continue;
        std::pair<std::string, int> pair = {action, keycode.asInt().unwrap()};
        keys.push_back(pair);
    };
    return keys;
}

void addLevelKeyBind(LevelEditorLayer *layer, std::string key, int def) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (savedDict.isObject() &&
        savedDict.size() ==
            0) // First key added | needs to create a default json
    {
        auto newDict = getDefaultJson();
        addKeyToJson(key, def, &newDict, false);
        alpha::level_storage::setSavedValue(layer, "config", newDict);
        return;
    }
    addKeyToJson(key, def, &savedDict, true);
    alpha::level_storage::setSavedValue(layer, "config", savedDict);
    return;
}
void editLevelKeyBind(
    LevelEditorLayer *layer,
    std::string oldActionName,
    std::pair<std::string, int> newActionAndKey,
    bool replaceEmpty
) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    log::info(
        "edotLevelKey {} / isObj {} | isNull {}",
        savedDict.dump(),
        savedDict.isObject(),
        savedDict.isNull()
    );
    // 2 cases, same or different actionName
    if (oldActionName == newActionAndKey.first)
        addKeyToJson(
            newActionAndKey.first,
            newActionAndKey.second,
            &savedDict,
            replaceEmpty
        ); // Just Overrite the old value //! Error I used edit...
           // to delete the key, if another key is deleted, it
           // gonna override that key because of this if, just add
           // a bool to enable emptyFill
    else
        editKeybind(
            &savedDict,
            oldActionName,
            newActionAndKey.first,
            newActionAndKey.second
        ); // transverse the entire json until
           // oldactionName is founded and replaced
    alpha::level_storage::setSavedValue(layer, "config", savedDict);

    return;
}
void deleteKeybindsFromLevel(LevelEditorLayer *layer) {
    alpha::level_storage::setSavedValue(layer,"config",{});
};

}; // namespace keybindsAPI
