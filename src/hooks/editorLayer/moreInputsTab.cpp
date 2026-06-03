#include "../../layers/editor/setupGUI.hpp"
#include "Geode/cocos/cocoa/CCGeometry.h"
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

    
};