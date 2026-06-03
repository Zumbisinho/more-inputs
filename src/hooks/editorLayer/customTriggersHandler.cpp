#include "../../layers/editor/setupGUI.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/cocos.hpp"
#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CountTriggerGameObject.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "triggerUI/touchMacro.hpp"
#include "../../utils/macroClasses.hpp"

// TODO: Add Cache to the macro triggers on load and update when needed (Create and delete)
// TODO: Add a callback when a count trigger is not found (ReCreate it or idk crash the game)

constexpr int touchMacroID = 14671; // 15000 limit

using namespace geode::prelude;

class $modify(MyTriggerEditorUI, EditorUI) {
    CreateMenuItem *getCreateBtn(int id, int bg) {
        switch (id) {
        case touchMacroID: {
            auto btn = getCreateBtn(1, 4);

            auto array = CCArray::create();
            auto spr = spriteFromObjectString(
                "1,914,31,bW9yZV9pbnB1dHM6ZWRpdG9yIHRyaWdnZXI=",
                false,
                false,
                0,
                array,
                nullptr,
                nullptr
            ); // obj id and text in base64, this thing translates to
               // more-inputs:editor trigger
            spr->setScale(
                std::min(
                    32.f / spr->getContentHeight(),
                    32.f / spr->getContentWidth()
                )
            );
            auto defaultBtn =
                static_cast<ButtonSprite *>(btn->getNormalImage());
            if (auto toHide = defaultBtn->m_subSprite)
                toHide->setVisible(false);
            defaultBtn->addChild(spr);
            spr->setPosition({20, 21});

            btn->m_objectID = touchMacroID;
            btn->setTag(touchMacroID);

            return btn;
        }
        default:
            return EditorUI::getCreateBtn(id, bg); // Default
        }
    };
    void onCreateObject(int id) {
        if (id != touchMacroID)
            return EditorUI::onCreateObject(id);
        ;
        id = 914; // set to create a text label instead of a crash
        EditorUI::onCreateObject(id);    // creates the trigger macro
        if (m_selectedObject == nullptr) // clicked on top of a trigger
            return;

        auto touchMacro = m_selectedObject;

        CCPoint pos = touchMacro->getPosition();

        auto onPressCounter = static_cast<CountTriggerGameObject*>(this->m_editorLayer->createObject(
            1611, pos, false
        )); // creates the counters
        auto onReleaseCounter = static_cast<CountTriggerGameObject*>(this->m_editorLayer->createObject(
            1611, pos, false
        )); // creates the counters

        onPressCounter->m_zLayer = ZLayer::B5;
        onPressCounter->m_zOrder = -67;
        onPressCounter->m_scaleX = 0.25f;
        onPressCounter->m_scaleY = 0.25f;
        onPressCounter->m_activateGroup = true;
        onPressCounter->m_pickupCount = 1;
        onPressCounter->setScale(0.25f);

        onReleaseCounter->m_zLayer = ZLayer::B5;
        onReleaseCounter->m_zOrder = -68;
        onReleaseCounter->m_scaleX = 0.25f;
        onReleaseCounter->m_scaleY = 0.25f;
        onReleaseCounter->m_activateGroup = true;
        onReleaseCounter->m_pickupCount = 0;
        onReleaseCounter->setScale(0.25f);

        if (touchMacro) {
            auto textObj = static_cast<TextGameObject *>(touchMacro);
            textObj->updateTextObject(
                "more_inputs:", false
            ); // template, idk what params i gonna add later
        }
        CCArray *toSelect = CCArray::create();

        toSelect->addObject(touchMacro);
        toSelect->addObject(onPressCounter);
        toSelect->addObject(onReleaseCounter);

        this->selectObjects(toSelect, true);
    }
    void clickOnPosition(CCPoint position) {
        EditorUI::clickOnPosition(position);
        if (m_selectedMode == 2)
            if (m_selectedObjectIndex != 914)
                return;

        auto nodePos =
            m_editorLayer->m_objectLayer->convertToNodeSpace(position);

        auto macro = m_editorLayer->objectAtPosition(nodePos);
        auto textGameObject = typeinfo_cast<TextGameObject *>(macro);

        if (!textGameObject)
            return;

        if (textGameObject->m_text.starts_with("more_inputs:")) {
            CCArray *toSelect = CCArray::create();
            toSelect->addObject(macro);
            for (auto obj :
                 CCArrayExt<GameObject *>(m_editorLayer->m_objects)) {
                if (obj == m_selectedObject)
                    continue;

                if (obj->m_objectID != 1611)
                    continue;

                CCPoint objPosition = {
                    static_cast<float>(obj->m_positionX),
                    static_cast<float>(obj->m_positionY)
                };
                CCPoint macroPosition = {
                    static_cast<float>(macro->m_positionX),
                    static_cast<float>(macro->m_positionY)
                };
                if (objPosition == macroPosition) {
                    toSelect->addObject(obj);
                }
                if (toSelect->count() == 3)
                    break;
            };
            selectObjects(toSelect, true);
            if (m_selectedMode == 2) {
                m_selectedObjectIndex = touchMacroID;
                updateCreateMenu(true);
            }
        }
    }
    void editObject(CCObject *sender) {
        if (m_selectedObjects->count() != 3)
            return EditorUI::editObject(sender);
        for (auto trigger : CCArrayExt<GameObject *>(m_selectedObjects)) {
            if (trigger->m_objectID != 914)
                continue;

            if (!trigger->getChildByIDRecursive("touch-macro"_spr))
                continue;
            auto macro = new touchMacro();

            // soo it is selecting a macro
            for (auto trigger : CCArrayExt<GameObject *>(m_selectedObjects)){
                if (trigger->m_objectID == 914) 
                    macro->macroObj = static_cast<TextGameObject*>(trigger);
                if (trigger->m_objectID == 1611 && trigger->m_zOrder == -67) // press
                    macro->pressObj = static_cast<CountTriggerGameObject*>(trigger);
                if (trigger->m_objectID == 1611 && trigger->m_zOrder == -68) // release
                    macro->releaseObj = static_cast<CountTriggerGameObject*>(trigger);
            }
            TouchMacroUI::open(macro);
            return;

        }

        EditorUI::editObject(sender);
    };
};