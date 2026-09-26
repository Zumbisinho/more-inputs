#include "../utils/keybindsAPI.hpp"
#include "../utils/keybindsCache.hpp"
#include "../utils/keycodeToString.hpp"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "gui.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {

    void customSetup() {
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
            spr, this, menu_selector(MyPauseLayer::onClick)
        );

        btn->setID("key-config"_spr);

        sideMenu->addChild(btn);
        sideMenu->setLayout(
            ColumnLayout::create()
                ->setGap(12.f)
                ->setAutoScale(true)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisAlignment(AxisAlignment::Center)
        );

        sideMenu->updateLayout();

        return;
    };
    void onMobileEdit() {
        if (!KeybindCache::initialized)
            KeybindCache::init(LevelEditorLayer::get());
        auto layer = EditMobileKeys::create(false);
        size_t index = 0;
        for (auto &keybind : KeybindCache::keySettings) {
            CCPoint relativePos = keybind.second.pos;
            CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

            auto btn = MobileButton::create(&keybind);
            btn->setAnchorPoint({0.5, 0.5});
            btn->setPosition(relativePos.x * screenSize.width, relativePos.y * screenSize.height);
            layer->addNode(btn, index++ != 0, true);
            layer->calcSnaps();
        };

        CCScene::get()->addChild(layer);
    }

    void onClick(CCObject *obj) {
        #ifndef GEODE_IS_DESKTOP
            onMobileEdit();
            return;
        #endif

        if (!KeybindCache::keybindsAndAction.empty()) {
            KeyBindsLocalConfigGui::open(obj, KeybindCache::keySettings);
            return;
        }

        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        auto keys = keybindsAPI::getLevelKeySettings(playLayer ? (CCLayer *)playLayer : (CCLayer *)editorLayer);

        

        KeyBindsLocalConfigGui::open(obj, keys);
        return;
    }
};
