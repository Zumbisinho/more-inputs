#include "../../layers/editor/setupGUI.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/cocos.hpp"
#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextGameObject.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <matjson.hpp>
#include "../../utils/customTriggers.hpp"
#include "smjs.object-collab/include/ObjectAPI.hpp"
#include <smjs.object-collab/include/object_collab.hpp>

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
                    "addKeyMIP.png"_spr,
                    menu_selector(MyEditorUI::onSetupBtn),
                    this->m_tabsMenu,
                    0.8f,
                    4,
                    {0, 0}

                );

                auto touchMacro = ObjectAPI::getCustomObject("touch-trigger-macro"_spr)->setupCreateMenuItem();
                auto editKeybind = ObjectAPI::getCustomObject("edit-keybind-trigger"_spr)->setupCreateMenuItem();


                buttons.push_back(btn);
                buttons.push_back(touchMacro);
                buttons.push_back(editKeybind);
                return alpha::editor_tabs::createEditButtonBar(buttons);
            },
            [] { // ? Setup to Place the tabImg
                return CCSprite::createWithSpriteFrameName("btn.png"_spr);
            }
        );
        return true;
    };

    void onSetupBtn(CCObject *sender) {
        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto keys = keybindsAPI::getLevelKeySettings(
            playLayer ? (CCLayer *)playLayer : (CCLayer *)editorLayer
        );
        setupKeyBindsGUI::open(keys);
    };
};