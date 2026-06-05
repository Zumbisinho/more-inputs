#pragma once
#include "Geode/loader/Log.hpp"
#include "Geode/utils/general.hpp"
#include "keybindsCache.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/CountTriggerGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

using namespace geode::prelude;

namespace macroTriggers {

static std::string configToTriggerStr(
    int actionIndex,
    int pressGroupId,
    int releaseGroupId,
    bool touchTriggered,
    bool spawnTriggered,
    bool multiTriggered,
    bool disarmOnFirst
) {

    return fmt::format(
        "more_inputs: {} {} {} {} {} {} {}",
        actionIndex,
        pressGroupId,
        releaseGroupId,
        static_cast<int>(touchTriggered),
        static_cast<int>(spawnTriggered),
        static_cast<int>(multiTriggered),
        static_cast<int>(disarmOnFirst)
    );
};
static std::vector<int> triggerStrToConfig(std::string string) {
    std::vector<int> toReturn;
    if (!string.starts_with("more_inputs:"))
        return toReturn;
    if (string.size() <= 12) // trigger recent added/ not opened
        return toReturn;
    string = string.substr(13);
    for (const auto word : std::views::split(string, ' ')) {
        toReturn.push_back(
            numFromString<int>(std::string_view(word)).unwrapOr(0)
        );
    };

    return toReturn;
};

class touchMacro {
public:
    TextGameObject *macroObj;
    CountTriggerGameObject *pressObj;
    CountTriggerGameObject *releaseObj;

    int getPressGroup() {
        auto configStr = triggerStrToConfig(this->macroObj->m_text);
        if (configStr.empty())
            return 0;
        return configStr[1];
    };
    int getReleaseGroup() {
        auto configStr = triggerStrToConfig(this->macroObj->m_text);
        if (configStr.empty())
            return 0;
        return configStr[2];
    };
};
} // namespace macroTriggers