#pragma once

#include <Geode/Geode.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <matjson.hpp>
#include <string>

#include <unordered_set>
#include <utility>

using namespace geode::prelude;

namespace KeyAPIv2 {
    std::unordered_set<int> getLevelKeyBindsRaw(CCLayer* layer);
    std::vector<std::pair<std::string, int>> getLevelKeyBinds(CCLayer* layer,bool ignoreEmpty);
}