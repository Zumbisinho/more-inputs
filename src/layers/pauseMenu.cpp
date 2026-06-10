#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "../utils/keybindsAPI.hpp"
#include "../utils/keycodeToString.hpp"
#include "../utils/keybindsCache.hpp"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "gui.hpp"



using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer)
{

    void customSetup()
    {
        PauseLayer::customSetup();
        if (KeybindCache::keybinds.empty()) // If no keybinds on the level = Not added
            return;


        auto sideMenu = this->getChildByID("left-button-menu");

        
        if (!sideMenu)
            return;

        if (sideMenu->getChildByID("key-config"_spr)) // already added
            return;

        auto spr = CircleButtonSprite::createWithSpriteFrameName("btn.png"_spr);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyPauseLayer::onClick));

        btn->setID("key-config"_spr);

        sideMenu->addChild(btn);
        sideMenu->setLayout(
            ColumnLayout::create()
                ->setGap(12.f)
                ->setAutoScale(true)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisAlignment(AxisAlignment::Center));

        sideMenu->updateLayout();

        return;
    };
    void onClick(CCObject * obj)
    {
        if (!KeybindCache::keybindsAndAction.empty())
        {
            KeyBindsLocalConfigGui::open(obj,KeybindCache::keybindsAndAction);
            return;
        }
        

        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        auto keys = keybindsAPI::getLevelKeyBinds(playLayer ? (CCLayer*)playLayer: (CCLayer*)editorLayer,true);

        KeyBindsLocalConfigGui::open(obj,keys);
        return;
    }
};
