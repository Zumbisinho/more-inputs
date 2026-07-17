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
    

    void selected() override;
    void unselected() override;

    bool init(std::string buttomLabel, bool isSpriteFrameName, CCSize size, int pickupId);

public:
    static MobileButton *
    create(std::string buttomLabel, bool isSpriteFrameName, CCSize size, int pickupId) {
        auto ret = new MobileButton();
        if (ret && ret->init(buttomLabel, isSpriteFrameName, size, pickupId)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
