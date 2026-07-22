#include "../../utils/pickupManager.hpp"
#include "../../utils/keybindsCache.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "mobileKeys.hpp"



using namespace geode::prelude;


class $modify(MobileKeys, PlayLayer) {

    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        KeybindCache::init(this);

        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

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
            mobileKey->setAnchorPoint({0.5,0.5});
            overlay->addChild(mobileKey);
        };
        return true;
    }
};