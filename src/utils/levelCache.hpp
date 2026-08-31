#pragma once

#include "Geode/utils/StringMap.hpp"
#include "getJson.hpp"
#include "macro.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include <unordered_map>
#include <vector>


// to grab custom trigger i need objectApi::getRegister

using namespace geode::prelude;

namespace LevelCache {
static short IdentityMaterialId = getConfig()["keyBindsIdentityMaterialId"].asInt().unwrapOrDefault();
static int StartControlId = getConfig()["keyBindsIdentityStartControlId"].asInt().unwrapOr(67);
inline std::set<int> freeControlIds;
inline int maxFreeControlId = StartControlId;
inline std::unordered_map<int,MacroTriggers::macroTrigger*> m_macroTriggers;
inline std::unordered_map<int,std::function<void(std::vector<EffectGameObject*>)>> onLevelLoadSetupAuxTriggersCallbacks; // {ControlId : list of all auxTriggers Pointers}

int getNextFreeControlId();

void freeControlId(int controlId);
void init(LevelEditorLayer *layer);

void addLLSATCBs(std::function<void(std::vector<EffectGameObject*>)> func, int controlId);

inline void reset(){
    maxFreeControlId = StartControlId;
    freeControlIds.clear();
    m_macroTriggers.clear();
};

}; // namespace LevelCache