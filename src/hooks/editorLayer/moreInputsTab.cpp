#include "../../layers/editor/setupGUI.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/loader/Log.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <matjson.hpp>

constexpr int touchMacroID = 14671; // 15000 limit

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer *editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        alpha::editor_tabs::addTab(
            "tab"_spr,
            alpha::editor_tabs::BUILD,
            [this] { // ? Setup to build the Tab
                std::vector<Ref<CCNode>> buttons;

                auto btn = getSpriteButton(
                    "addKeyMIProt.png"_spr,
                    menu_selector(MyEditorUI::onSetupBtn),
                    this->m_tabsMenu,
                    0.8f,
                    4,
                    {0, 0}

                );

                auto touchMacro = getCreateBtn(touchMacroID, 1);

                buttons.push_back(btn);
                buttons.push_back(touchMacro);
                return alpha::editor_tabs::createEditButtonBar(buttons);
            },
            [] { // ? Setup to Place the tabImg
                return CCSprite::createWithSpriteFrameName("btnUgly.png"_spr);
            }
        );
        return true;
    };

    void onSetupBtn(CCObject *sender) {
        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto keys = keybindsAPI::getLevelKeyBinds(
            playLayer ? (CCLayer *)playLayer : (CCLayer *)editorLayer, true
        );
        setupKeyBindsGUI::open(scene, keys);
    };

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
    }
    void onCreateObject(int id) {
        if (id != touchMacroID)
            return EditorUI::onCreateObject(id);
        ;
        id = 914;
        EditorUI::onCreateObject(id);
        if (m_selectedObject) {
            auto textObj = static_cast<TextGameObject *>(m_selectedObject);
            textObj->updateTextObject(
                "more_inputs:", false
            ); // template, idk what params i gonna add later
        }
    }
    void clickOnPosition(CCPoint position) {

        EditorUI::clickOnPosition(position);
        if (m_selectedObjectIndex != 914)
            return;
        if (m_selectedMode != 2)
            return;

        auto nodePos =
            m_editorLayer->m_objectLayer->convertToNodeSpace(position);

        auto obj = m_editorLayer->objectAtPosition(nodePos);
        auto textGameObject = typeinfo_cast<TextGameObject *>(obj);
        if (!textGameObject)
            return;

        if (textGameObject->m_text.starts_with("more_inputs:")) {
            m_selectedObjectIndex = touchMacroID;
            updateCreateMenu(true);
        }
    }
    void editObject(CCObject *sender) {
        if (m_selectedObject && m_selectedObject->m_objectID == 914)
            if (bool isTouch = m_selectedObject->getChildByIDRecursive("touch-macro"_spr)) { // detect if textlabel as a sprite
                geode::log::warn("Cliquei no trigger");
            }

        EditorUI::editObject(sender);
    }
};