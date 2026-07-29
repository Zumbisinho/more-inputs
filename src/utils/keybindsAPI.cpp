#include "keybindsAPI.hpp"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/VersionInfo.hpp"
#include "Geode/utils/general.hpp"
#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include "keybindsCache.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <cstddef>
#include <matjson.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Las grandes versiones del hpps
#include "keybindsAPIVersions/1.x.x.hpp"
#include "keybindsAPIVersions/2.x.x.hpp"

using namespace geode::prelude;

// ? This was refactored to not handle the API, but redirect it to the desired
// ? version, sooooooo the code for this is on the keybindsAPIVersions folder,
// ? the 1.x.x is the old one and the 2.x.x is the new one with mobile features

namespace keybindsAPI {
// ? helper functions

matjson::Value formatInJson(const KeybindValue *values) {
    auto newObj = matjson::Value::object();
    auto posArr = matjson::Value::array();
    posArr.push(values->pos.x);
    posArr.push(values->pos.y);
    auto sizeArr = matjson::Value::array();
    sizeArr.push(values->contentSize.width);
    sizeArr.push(values->contentSize.height);

    newObj["name"] = values->name;
    newObj["keyCode"] = values->keyCode;
    newObj["isSpr"] = values->isSpr;
    newObj["position"] = posArr;
    newObj["contentSize"] = sizeArr;
    newObj["buttonLabel"] = values->buttonLabel;

    return newObj;
};
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

VersionInfo getLevelVersion(CCLayer *layer) {
    auto configJson =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (auto ver = configJson["version"].asString().unwrapOr(""); ver.size() != 0) {
        return VersionInfo::parse(ver).unwrap();
    };
    return VersionInfo::parse("v0.0.0").unwrap();
};
struct IDAndPos {
    int actionId;
    int pos;
};

IDAndPos getNextFreeActionId(matjson::Value *json) {
    if ((*json)["keybinds"].isObject() == false)
        return {0, 0};
    int lastId = -1;
    int index = -1;
    log::warn("{}", json->dump());
    for (auto const &[key, value] : (*json)["keybinds"]) {
        log::info("Cur {}", key);
        index++;
        int actionId = numFromString<int>(key).unwrapOr(0);
        log::info("CurConverted {}", actionId);
        if (lastId == -1) {
            lastId = actionId;
            continue;
        };
        if (int dif = actionId - lastId; dif != 1) {
            return {lastId + 1, index};
        } else {
            lastId = actionId;
        }
    };
    if (lastId != -1)            // was in the loop
        return {lastId + 1, -1}; // last
    else                         // first interraction
        return {KeybindCache::startId + 1, 0};
};

void addKeyToJson(
    matjson::Value *json, KeybindValue *keySettings

) {
    if ((*json)["keybinds"].isObject() == false)
        return;
    ;
    IDAndPos nextFree = getNextFreeActionId(json);
    log::warn("NextFree: {} {}", nextFree.actionId, nextFree.pos);
    if (nextFree.actionId == 0)
        return; // Error that can be supress

    if (nextFree.pos == 0 ||
        nextFree.pos == -1) { // first interraction or last pos
        (*json)["keybinds"][std::to_string(nextFree.actionId)] = formatInJson(keySettings);
        geode::log::warn("AddKeyToJson 0 or -1 condition : {}", json->dump());
        return;
    }

    size_t idx = 0;
    matjson::Value newObj = getDefaultJson();
    for (auto const &[key, value] : (*json)["keybinds"]) {
        if (idx++ == nextFree.pos) {
            geode::log::warn("Item {} é a posição, mudando", key);
            newObj["keybinds"][std::to_string(nextFree.actionId)] = formatInJson(keySettings);
        };
        newObj["keybinds"][key] = value;
        geode::log::warn("Item {}, {}", key, newObj.dump());
    };
    *json = newObj;
    geode::log::warn("AddKeyToJson : {}", json->dump());
}
void editKeybind(
    matjson::Value *json, const KeybindValue *newKeySettings, int actionId
) {
    log::warn("Edit keybinds Id: {}", actionId);
    if (!json || !json->isObject()) // how do you even get here?
        return;

    matjson::Value newObj = getDefaultJson();
    bool asFounded = false;
    for (auto const &[key, value] : (*json)["keybinds"]) {
        int curActionId = numFromString<int>(key).unwrapOr(0);
        if (curActionId == actionId && !asFounded) {
            newObj["keybinds"][key] = formatInJson(newKeySettings);
            asFounded = true;
        } else {
            newObj["keybinds"][key] = value;
        }
    }
    if (!asFounded)
        return;

    *json = newObj;
}

std::unordered_set<int> getLevelKeyBindsRaw(CCLayer *layer) {
    switch (getLevelVersion(layer).getMajor()) {
        case 2: {
            return KeyAPIv2::getLevelKeyBindsRaw(layer);
        };
        case 1: {
            return KeyAPIv1::getLevelKeyBindsRaw(layer);
        };
    }
    return {};
};
std::vector<std::pair<std::string, int>>
getLevelKeyBinds(CCLayer *layer, bool ignoreEmpty) {
    switch (getLevelVersion(layer).getMajor()) {
        case 2: {
            return KeyAPIv2::getLevelKeyBinds(
                layer, false
            ); // i gonna remove the 2 param
        };
        case 1: {
            return KeyAPIv1::getLevelKeyBinds(layer, ignoreEmpty);
        };
    }
    return {};
};
std::vector<KeyFullSettings> getLevelKeySettings(CCLayer *layer) {
    std::vector<KeyFullSettings> keys;
    resetCurButtonIdx();
    switch (getLevelVersion(layer).getMajor()) {
        case 2: {
            auto keybinds =
                alpha::level_storage::getSavedValue<matjson::Value>(layer, "config")["keybinds"].as<std::unordered_map<std::string, KeybindValue>>().unwrapOrDefault();
            if (keybinds.size() == 0)
                return keys;
            for (const auto &[key, value] : keybinds) {
                geode::log::warn("{} {} {} {} {}", key, value.keyCode, value.buttonLabel, value.contentSize, value.name);
                int curActionId = numFromString<int>(key).unwrapOr(0);
                keys.push_back({curActionId, value});
            };
            return keys;
        }
        case 1: {
            auto keybinds = KeyAPIv1::getLevelKeyBinds(layer, false);
            int idx = KeybindCache::startId + 1;
            for (const auto [actionName, keycode] : keybinds) {
                if (keycode == -67) { // empty
                    idx++;
                    continue;
                }
                auto dummy = createPcValue(actionName, keycode, curButtonIdx);
                keys.push_back({idx++, dummy});
            };
        };
    };
    return keys;
};
void addLevelKeyBind(LevelEditorLayer *layer, KeybindValue *keySettings) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (savedDict["keybinds"].size() == 0) {
        savedDict = getDefaultJson();
    }

    addKeyToJson(&savedDict, keySettings);
    alpha::level_storage::setSavedValue(layer, "config", savedDict);
    return;
}
void editLevelKeyBind(
    LevelEditorLayer *layer,
    int actionID,
    const KeybindValue *newKeySettings
) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    editKeybind(&savedDict, newKeySettings, actionID);
    alpha::level_storage::setSavedValue(layer, "config", savedDict);

    return;
}

void deleteLevelKeybind(LevelEditorLayer *layer, int actionID) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    auto newObj = getDefaultJson();
    for (auto const &[key, value] : savedDict["keybinds"]) {
        int curActionId = numFromString<int>(key).unwrapOr(0);
        if (curActionId == actionID) {
            continue;
        }
        newObj["keybinds"][key] = value;
    }
    alpha::level_storage::setSavedValue(layer, "config", newObj);
};
void deleteKeybindsFromLevel(LevelEditorLayer *layer) {
    alpha::level_storage::setSavedValue(layer, "config", {});
};

void convertLevelKeybinds(LevelEditorLayer *layer) {
    auto savedDict =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    auto newObj = getDefaultJson();
    auto anotherIndexThing = 0;

    int idx = KeybindCache::startId + 1;
    for (const auto [actionName, keycode] : savedDict["keybinds"]) {
        int keycodeInt = keycode.asInt().unwrapOr(-67);
        if (keycode == -67) { // empty
            idx++;
            continue;
        }

        auto dummy = createPcValue(actionName, keycodeInt, anotherIndexThing);
        newObj["keybinds"][std::to_string(idx++)] = formatInJson(&dummy);
    };
    alpha::level_storage::setSavedValue(layer, "config", newObj);
    KeybindCache::reset();

};

inline keybindsAPI::KeybindValue createPcValue(std::string name, int keyCode, int &index) {

    auto screenSize = CCDirector::sharedDirector()->getWinSize();
    int threshold = floor((screenSize.width / 60));
    geode::log::warn("Agr o index dos buttoms {}", index);

    int buttonRow = floor(index / threshold);
    float absX = 25 + (index - threshold * buttonRow) * 60;

    float absY = buttonRow != 0 ? screenSize.height + 25 - buttonRow * 60 : 30;

    CCPoint relPos = absToRel(absX, absY);

    index++;
    return keybindsAPI::KeybindValue::parse(name, keyCode, false, relPos, CCSize{50, 50}, name);
};

} // namespace keybindsAPI