#include "../../utils/keybindsCache.hpp"
#include "../../utils/keybindsAPI.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "mobileKeys.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(MobileKeys, PlayLayer) {

    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        KeybindCache::init(this);

        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        KeybindCache::mobileKeyNodes.clear();

        auto overlay = CCMenu::create();
        overlay->setID("MobileKey-Overlay"_spr);
        overlay->setContentSize(screenSize);
        overlay->setAnchorPoint({0, 0});
        overlay->setPosition(0, 0);

        this->addChild(overlay);
        int index = 0;
        for (auto key : KeybindCache::keySettings) {

            auto mobileKey = MobileButton::create(&key);
            CCPoint relativePos = mobileKey->getKey()->second.pos;

            mobileKey->setPosition(MobileButton::relativePosToCanva(relativePos));
            mobileKey->setAnchorPoint({0.5, 0.5});
            overlay->addChild(mobileKey);
            KeybindCache::mobileKeyNodes[key.first] = mobileKey; // actionId = pointer to the node

            if (auto toHide =KeybindCache::mobileKeysToHideOnInit;!toHide.empty())
                if (std::ranges::contains(toHide,key.first))
                    mobileKey->setVisible(false);
            
        };
        return true;
    }
};

class $modify(MobileKeysEditor, LevelEditorLayer) {
    struct Fields{
        CCMenu* m_overlay;
    };


    void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        KeybindCache::mobileKeyNodes.clear();

        auto overlay = CCMenu::create();
        overlay->setID("MobileKey-Overlay"_spr);
        overlay->setContentSize(screenSize);
        overlay->setAnchorPoint({0, 0});
        overlay->setPosition(0, 0);

        int index = 0;
        for (auto key : KeybindCache::keySettings) {
            auto mobileKey = MobileButton::create(&key);
            CCPoint relativePos = mobileKey->getKey()->second.pos;

            mobileKey->setPosition(MobileButton::relativePosToCanva(relativePos));
            mobileKey->setAnchorPoint({0.5, 0.5});
            overlay->addChild(mobileKey);
            KeybindCache::mobileKeyNodes[key.first] = mobileKey; // actionId = pointer to the node

            if (auto toHide =KeybindCache::mobileKeysToHideOnInit;!toHide.empty())
                if (std::ranges::contains(toHide,key.first))
                    mobileKey->setVisible(false);
        };

        m_fields->m_overlay = overlay;

        this->addChild(overlay);
    };

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();

        if (m_fields->m_overlay != nullptr) {
            m_fields->m_overlay->removeMeAndCleanup();
            m_fields->m_overlay = nullptr;
        }
    };


};