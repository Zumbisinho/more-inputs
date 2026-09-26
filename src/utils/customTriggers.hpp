#pragma once

#include "../hooks/editorLayer/triggerUI/actionDropdown.hpp"



#include "smjs.object-collab/include/EditorPopupConfig.hpp"
#include "smjs.object-collab/include/ObjectInfo.hpp"
#include "smjs.object-collab/include/ObjectTraits.hpp"

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

namespace customTriggers {

class $object(TouchPPTrigger,EffectGameObject) {
public:
    int m_actionIndex = 0;
    int m_pressGroupId = 0;
    int m_releaseGroupId = 0;
    bool m_disarmOnFirst = false;
    std::string m_formatedTriggerLabel = "0/0";

    TouchPPTrigger(ObjectInfo *info) : CustomObject(info, ObjectTraits::builder().gameObjectType(GameObjectType::Modifier).ignoreEditorDuration(true).build()) {
    }

    static TouchPPTrigger *create(ObjectInfo *info) {
        return new TouchPPTrigger(info);
    }
    static PopupConfig getEditConfig(const Selected &selected);
    void triggerObject(GJBaseGameLayer *layer, int uniqueID, const gd::vector<int> *remapKeys) override;
    void postEditorInit() override;


    std::string format();
};

class $object(EditKeybindTrigger, EffectGameObject) {
public:
    int m_actionIndex = 0;
    bool m_enabledKey = false;

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
