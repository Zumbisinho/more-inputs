#pragma once
#include <unordered_map>
#include <unordered_set>
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "getJson.hpp"
#include "keybindsAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <vector>

namespace KeybindCache {
    extern bool initialized;
    extern int startId;
    extern int value;
    inline std::unordered_set<int> keybinds;
    inline std::vector<std::pair<std::string, int>> keybindsAndAction;
    inline std::map<int,std::vector<int>> keyToActionIds;
    inline std::map<std::string,int> actionNameToID;
    inline std::vector<keybindsAPI::KeyFullSettings> keySettings;
    inline std::unordered_map<int,CCMenuItemSpriteExtra*> mobileKeyNodes;
    inline std::vector<int> mobileKeysToHideOnInit;
    void init(CCLayer* layer);
    void reset();
    void changeLocalKey(const keybindsAPI::KeyFullSettings &key, int keyCode);
}