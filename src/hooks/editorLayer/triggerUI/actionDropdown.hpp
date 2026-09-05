#pragma once

#include "../../../utils/keybindsCache.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/ui/Layout.hpp"
#include "builderMacros.hpp"
#include "smjs.object-collab/include/CustomObject.hpp"
#include "smjs.object-collab/include/object_collab.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/InfoAlertButton.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <concepts>
#include <cstddef>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;
using namespace object_collab::prelude;

namespace customTriggers {
class TouchMacroTrigger;
}
template <typename Trigger>
    requires std::derived_from<Trigger, object_collab::CustomObject<EffectGameObject>>
class ActionDropdown : public CCMenu {
public:
    static ActionDropdown *create(const Selected &selected) {
        auto ret = new ActionDropdown<Trigger>;
        if (ret && ret->init(selected)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };

protected:
    GoffyBuilder::DropDownList *m_obj;
    int m_actionIndex = 0;
    std::vector<Trigger *> m_selected;
    Trigger *first = nullptr;

private:
    bool init(const Selected &selected) {
        if (!CCMenu::init())
            return false;
        for (auto obj : selected) {
            auto trig = typeinfo_cast<Trigger *>(obj);
            if (!trig)
                continue;

            m_selected.push_back(trig);

            if (!first)
                first = trig;
        }
        // need to update multi select and edit
        if (!first)
            return false;
        // loads it from the first
        m_actionIndex = first->m_actionIndex;

        if (!KeybindCache::initialized)
            KeybindCache::init(LevelEditorLayer::get());

        auto keybinds = KeybindCache::keySettings;
        std::vector<std::string> actionList;

        if (keybinds.empty()) {
            actionList.push_back("Not found any keybinds!");
        } else {
            for (auto &action : keybinds) {
                actionList.push_back(action.second.name);
            };
        }

        auto actionDropDown =
            GoffyBuilder::DropDownList::create("Action:", 0.5, actionList);
        actionDropDown->setAnchorPoint({0, 0});
        actionDropDown->ignoreAnchorPointForPosition(false);

        this->setContentSize({420, actionDropDown->getContentHeight()});

        actionDropDown->setSelectCallBack([this]() {
            updateTriggers();
        });

        m_obj = actionDropDown;
        actionDropDown->setPosition(0, 0);
        this->addChildAtPosition(
            actionDropDown, Anchor::BottomLeft
        );


        actionDropDown->setSelected(m_actionIndex);

        return true;
    };
    void updateTriggers() {
        m_actionIndex = m_obj->m_curIndex;

        for (auto trig : m_selected) {
            trig->m_actionIndex = m_actionIndex;
        }
    }
};