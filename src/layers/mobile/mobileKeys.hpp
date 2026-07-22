#pragma once

#include "../../utils/keybindsCache.hpp"
#include "../../utils/pickupManager.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <string>

using namespace geode::prelude;

class MobileButton : public CCMenuItemSpriteExtra {
protected:
    CCNode *m_inner;
    int m_keyPickup;
    keybindsAPI::KeyFullSettings *m_key;
    bool m_looked;

    void selected() override;
    void unselected() override;

    bool init(keybindsAPI::KeyFullSettings *key);

public:
    static MobileButton *
    create(keybindsAPI::KeyFullSettings *key) {
        auto ret = new MobileButton();
        if (ret && ret->init(key)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
    keybindsAPI::KeyFullSettings *getKey() {
        return m_key;
    };
    void setKey(keybindsAPI::KeyFullSettings key) {

    };
    void setLocked(bool state) {
        m_looked = state;
    };
    bool getLocked(bool state) {
        return m_looked;
    };
};
