#pragma once

#include "../hooks/editorLayer/triggerUI/actionDropdown.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/loader/Log.hpp"
#include "Geode/loader/Mod.hpp"
#include "Geode/loader/ModEvent.hpp"
#include "keybindsCache.hpp"
#include "macro.hpp"
#include "smjs.object-collab/include/EditorPopupConfig.hpp"
#include "smjs.object-collab/include/ObjectAPI.hpp"
#include "smjs.object-collab/include/ObjectInfo.hpp"
#include "smjs.object-collab/include/ObjectTraits.hpp"
#include "smjs.object-collab/include/Property.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/CountTriggerGameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <fmt/format.h>
#include <smjs.object-collab/include/object_collab.hpp>
#include <string>

using namespace geode::prelude;
using namespace object_collab::prelude;
using namespace MacroTriggers;

namespace customTriggers {

class $macro(TouchMacroTrigger) {
public:
    setMacroId("touch-trigger"_spr);
    int m_actionIndex = 0;
    int m_pressGroupId = 0;
    int m_releaseGroupId = 0;
    bool m_disarmOnFirst = false;
    bool m_multiActivate = !m_disarmOnFirst;
    std::string m_formatedTriggerLabel = "0/0";
    MacroAuxiliarTrigger<CountTriggerGameObject, 1611> m_pressCount;
    MacroAuxiliarTrigger<CountTriggerGameObject, 1611> m_releaseCount;

    TouchMacroTrigger(ObjectInfo *info) : MacroTriggers::macroTrigger(info, ObjectTraits::builder().gameObjectType(GameObjectType::Modifier).ignoreEditorDuration(true).build()) {
        // add the triggers to the vec
        m_auxTriggers.push_back(&m_pressCount);
        m_auxTriggers.push_back(&m_releaseCount);

        if (!KeybindCache::initialized)
            KeybindCache::init(GJBaseGameLayer::get());
        m_pressCount.addInitProps(
            $Alter(m_zOrder, -67),
            $Alter(m_activateGroup, true),
            $Alter(m_pickupCount, 1),
            $Alter(m_scaleX, 0.25f),
            $Alter(m_scaleY, 0.25f)

        );

        m_releaseCount.addInitProps(
            $Alter(m_zOrder, -68),
            $Alter(m_activateGroup, true),
            $Alter(m_pickupCount, 0),
            $Alter(m_scaleX, 0.25f),
            $Alter(m_scaleY, 0.25f)
        );

        m_pressCount.addDynamicProps(
            $Alter(m_itemID, !KeybindCache::keySettings.empty() ? KeybindCache::keySettings[*&m_actionIndex].first : 0),
            $Alter(m_targetGroupID, *&m_pressGroupId),
            $Alter(m_multiActivate, *&m_multiActivate),
            $Alter(m_isTouchTriggered, *&m_isTouchTriggered),
            $Alter(m_isSpawnTriggered, *&m_isSpawnTriggered),
            $Alter(m_isMultiTriggered, *&m_isMultiTriggered)
        );

        m_releaseCount.addDynamicProps(
            $Alter(m_itemID, !KeybindCache::keySettings.empty() ? KeybindCache::keySettings[*&m_actionIndex].first : 0),
            $Alter(m_targetGroupID, *&m_releaseGroupId),
            $Alter(m_multiActivate, *&m_multiActivate),
            $Alter(m_isTouchTriggered, *&m_isTouchTriggered),
            $Alter(m_isSpawnTriggered, *&m_isSpawnTriggered),
            $Alter(m_isMultiTriggered, *&m_isMultiTriggered)
        );
    }

    static TouchMacroTrigger *create(ObjectInfo *info) {
        return new TouchMacroTrigger(info);
    }
    static PopupConfig getEditConfig(const Selected &selected);

    void postEditorInit() override;

    // void bulkApplyDynamicProps() override {
    //     m_pressCount.applyDynamicProps();
    //     m_releaseCount.applyDynamicProps();
    //     log::warn("Adrs: {},{}",m_pressCount.m_obj,m_releaseCount.m_obj);
    // };

    std::string format();
};

class $object(EditKeybindTrigger, EffectGameObject) {
public:
    int m_actionIndex = 0;
    bool m_disableKey = false;

    EditKeybindTrigger(ObjectInfo *info) : CustomObject(info, ObjectTraits::builder().gameObjectType(GameObjectType::Modifier).ignoreEditorDuration(true).build()) {
    }

    static EditKeybindTrigger *create(ObjectInfo *info) {
        return new EditKeybindTrigger(info);
    }

    static PopupConfig getEditConfig(const Selected &selected);

    void postEditorInit() override;
    void triggerObject(GJBaseGameLayer *layer, int uniqueID, const gd::vector<int> *remapKeys) override;
};

} // namespace customTriggers
// clang-format off
