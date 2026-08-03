#pragma once

#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Mod.hpp"
#include "Geode/loader/ModEvent.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include "Geode/utils/general.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/CountTriggerGameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <algorithm>
#include <fmt/format.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace geode::prelude;

namespace customTriggers {

class CustomTrigger{
protected:
    bool logEnabled = true;

    template <class... Args>
    void ZLogInfo(fmt::format_string<Args...> fmt, Args &&...args) const {
        if (logEnabled) {
            geode::log::info(fmt, std::forward<Args>(args)...);
        }
    };

    template <typename... Args>
    std::string formatValues(int objId, Args &&...args) const {
        std::string toReturn = fmt::format("{} {}", objId, fmt::join(std::array{fmt::format("{}", static_cast<int>(std::forward<Args>(args)))...}, " "));
        ZLogInfo("{}", toReturn);
        return toReturn;
    }

    template <typename T>
    void parseOne(std::stringstream &ss, T &value) const {
        std::string token;

        if (!(ss >> token))
            return;
        ZLogInfo("String : {}, Token: {}", ss.str(), token);

        if constexpr (std::is_same_v<T, int>) {
            value = numFromString<int>(token).unwrapOrDefault();
        } else if constexpr (std::is_same_v<T, bool>) {
            value = numFromString<int>(token).unwrapOrDefault() != 0;
        } else if constexpr (std::is_same_v<T, float>) {
            value = numFromString<float>(token).unwrapOrDefault();
        } else if constexpr (std::is_same_v<T, double>) {
            value = numFromString<double>(token).unwrapOrDefault();
        } else if constexpr (std::is_same_v<T, std::string>) {
            value = token;
        }
    }

    template <typename... Args>
    bool parseValues(std::string_view str, bool &createBtn, int &objId, Args &...args) const {

        if (str.empty()) {
            createBtn = false;
            return false;
        }
        std::stringstream ss((std::string(str)));

        parseOne(ss, objId); // get obj id

        if (std::string_view sv = ss.str(); sv == "-1") {
            createBtn = true;
            ZLogInfo("CreateBtn {}", str);
            return true;
        }

        // Expande o pack:
        (parseOne(ss, args), ...);

        createBtn = false;
        return true;
    }
    template <typename... Args>
    std::string formatGroup(std::string_view formatString, Args &&...args) const {
        if (isCreateBtnCall)
            return "";
        return fmt::vformat(
            formatString,
            fmt::make_format_args(std::forward<Args>(args)...)
        );
    }

public:
    TextGameObject *mainObject;
    int customObjID = 0;
    bool isCreateBtnCall;
    const char *macroSpriteStr = "";
    CCPoint groupLabelOffset;

    static std::unique_ptr<CustomTrigger> create() {
        auto ret = std::make_unique<CustomTrigger>();
        if (ret && ret->init()) {
            return ret;
        };
        return nullptr;
    };

    virtual std::string configToTriggerStr() {
        return "";
    };
    virtual bool loadFromTriggerStr(std::string_view str) {
        return parseValues(str, isCreateBtnCall, customObjID);
    };

    void setSpriteStr(const char *sprStr, CCPoint groupLabelOffset) {
        this->macroSpriteStr = sprStr;
        this->groupLabelOffset = groupLabelOffset;
    };
    const char *getSpriteStr() {
        return macroSpriteStr;
    };
    virtual std::string getFormatedGroupLabel() {
        return "";
    }

    virtual bool init() {
        return true;
    };
};

class MacroTrigger : public CustomTrigger {
public:
    virtual std::vector<GameObject *> getAllObjs() {
        return {nullptr};
    };
};

class TouchMacroTrigger : public MacroTrigger {
public:
    static std::unique_ptr<TouchMacroTrigger> create() {
        auto ret = std::make_unique<TouchMacroTrigger>();
        if (ret && ret->init()) {
            return ret;
        };
        return nullptr;
    };

    bool init() override {
        this->setSpriteStr("touch_macro.png"_spr, {0, 4});
        this->customObjID = 1;
        return true;
    }

    CountTriggerGameObject *pressObj;
    CountTriggerGameObject *releaseObj;

    int actionIndex = 0;
    int pressGroupId = 0;
    int releaseGroupId = 0;
    bool touchTriggered = false;
    bool spawnTriggered = false;
    bool multiTriggered = false;
    bool disarmOnFirst = false;

    std::string configToTriggerStr() override {
        return formatValues(customObjID, actionIndex, pressGroupId, releaseGroupId, touchTriggered, spawnTriggered, multiTriggered, disarmOnFirst);
    };
    bool loadFromTriggerStr(std::string_view str) override {
        return parseValues(str, isCreateBtnCall, customObjID, actionIndex, pressGroupId, releaseGroupId, touchTriggered, spawnTriggered, multiTriggered, disarmOnFirst);
    }
    std::vector<GameObject *> getAllObjs() override {
        return {mainObject, pressObj, releaseObj};
    };
    std::string getFormatedGroupLabel() override {
        return formatGroup("{}/{}", pressGroupId, releaseGroupId);
    }
};

// The thing that like that ohhh saves ohhhh uhhh idk like saves the thing to implement therefor
void addCustomTrigger(std::unique_ptr<CustomTrigger> trigger, geode::Mod *mod = geode::Mod::get());

static constexpr int startCustomId = 20000; // ! Update if robrob works at 100x speed and create more objs
inline std::unordered_map<
    std::string,
    std::vector<std::unique_ptr<CustomTrigger>>>
    customTriggersPerMod;

} // namespace customTriggers
