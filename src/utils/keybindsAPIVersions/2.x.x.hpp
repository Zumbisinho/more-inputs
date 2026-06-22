#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <matjson.hpp>
#include <string>

#include <unordered_set>
#include <utility>

using namespace geode::prelude;

namespace KeyAPIv2 {


    matjson::Value getDefaultJson();


    void addKeyToJson(const std::string actionName, int actionKeyCode, matjson::Value* json);
    void editKeybind(matjson::Value *json, const std::string &oldAction,
                 const std::string &newAction, int newKeyCode);


    std::unordered_set<int> getLevelKeyBindsRaw(CCLayer* layer);
    std::vector<std::pair<std::string, int>> getLevelKeyBinds(CCLayer* layer,bool ignoreEmpty);


    void addLevelKeyBind(LevelEditorLayer* layer, std::string key, int def);
    void editLevelKeyBind(LevelEditorLayer* layer, int actionID,std::pair<std::string,int> newActionAndKey,bool replaceEmpty = true);
    void deleteKeybindsFromLevel(LevelEditorLayer *layer);
}