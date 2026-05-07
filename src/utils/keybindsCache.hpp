#pragma once
#include <unordered_set>
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "getJson.hpp"
#include "keybindsAPI.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>

namespace KeybindCache {
    extern bool initialized;
    extern int startId;
    extern int value;
    inline std::unordered_set<int> keybinds;
    inline std::vector<std::pair<std::string, int>> keybindsAndAction;
    inline std::map<int,std::vector<int>> keyToActionIds;
    inline std::map<std::string,int> actionNameToID;

    void init(CCLayer* layer);
    void reset();
    void changeLocalKey(std::string actionName, int newKeyCode);
}