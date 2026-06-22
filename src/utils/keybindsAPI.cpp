#include "keybindsAPI.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/VersionInfo.hpp"
#include "alphalaneous.level-storage-api/include/LevelStorageAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <array>
#include <matjson.hpp>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
// Las grandes versiones del hpps
#include "keybindsAPIVersions/1.x.x.hpp"
#include "keybindsAPIVersions/2.x.x.hpp"
#include "keybindsCache.hpp"

using namespace geode::prelude;

// ? This was refactored to not handle the API, but redirect it to the desired
// ? version, sooooooo the code for this is on the keybindsAPIVersions folder,
// ? the 1.x.x is the old one and the 2.x.x is the new one with mobile features

namespace keybindsAPI {
// ? helper functions
auto curVersion = Mod::get()->getVersion();

VersionInfo getLevelVersion(CCLayer *layer) {
    auto configJson =
        alpha::level_storage::getSavedValue<matjson::Value>(layer, "config");
    if (auto ver = configJson["version"].asString().unwrapOr("");
        ver.size() != 0) {
        return VersionInfo::parse(ver).unwrap();
    };
    return VersionInfo::parse("v0.0.0").unwrap();
};

// ! This need a better aprouch, like a list idk version theshold

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
            return KeyAPIv2::getLevelKeyBinds(layer,false); // i gonna remove the 2 param
        };
        case 1: {
            return KeyAPIv1::getLevelKeyBinds(layer,ignoreEmpty);
        };
    }
    return {};
};

void addLevelKeyBind(LevelEditorLayer *layer, std::string key, int def){
    switch (getLevelVersion(layer).getMajor()) {
        case 2: {
            KeyAPIv2::addLevelKeyBind(layer,key,def); // i gonna edit the 2 param
        };
        case 1: {
            KeyAPIv1::addLevelKeyBind(layer,key,def);
        };
    }
};
void editLevelKeyBind(
    LevelEditorLayer *layer,
    std::pair<std::string, int>actionNameAndId,
    std::pair<std::string, int> newActionAndKey,
    bool replaceEmpty
){
    switch (getLevelVersion(layer).getMajor()) {
        case 2: { // uses actionId
            KeyAPIv2::editLevelKeyBind(layer,actionNameAndId.second,newActionAndKey,replaceEmpty); // i gonna remove the 4 param
        };
        case 1: { // uses actionName
            KeyAPIv1::editLevelKeyBind(layer,actionNameAndId.first,newActionAndKey,replaceEmpty); 
        };
    }
}
void deleteKeybindsFromLevel(LevelEditorLayer *layer){
    switch (getLevelVersion(layer).getMajor()) {
        case 2: {
            KeyAPIv2::deleteKeybindsFromLevel(layer); // i gonna remove nothing
        };
        case 1: {
            KeyAPIv1::deleteKeybindsFromLevel(layer);
        };
    }
};
} // namespace keybindsAPI