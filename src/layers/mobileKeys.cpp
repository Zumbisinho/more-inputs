#include "CCProtocols.h"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/ui/Layout.hpp"
#include "ccTypes.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <string>
#include "../utils/pickupManager.hpp"

using namespace geode::prelude;

class MobileButton : public CCMenuItemSpriteExtra {
protected:
    CCNode *m_inner;
    int m_keyPickup;
    void selected() override {
        log::info("My custom button was activated!");
        this->setColor({128, 128, 128});

        CCRGBAProtocol *rgba = typeinfo_cast<CCRGBAProtocol *>(m_inner);
        if (rgba != nullptr) {
            rgba->setColor(ccColor3B{128, 128, 128});
        };
        pickupManager::changePickupId(m_keyPickup,1);
    }
    void unselected() override {
        log::info("My custom button was de-activated!");
        this->setColor({255, 255, 255});

        CCRGBAProtocol *rgba = typeinfo_cast<CCRGBAProtocol *>(m_inner);
        if (rgba != nullptr) {
            rgba->setColor(ccColor3B{255, 255, 255});
        };
        pickupManager::changePickupId(m_keyPickup,0);
    }

    bool init(std::string buttomLabel, bool isSpriteFrameName, CCSize size,int pickupId) {
        auto spr = CCScale9Sprite::createWithSpriteFrameName(
            "mobileButtomTemplateMIP.png"_spr
        );
        m_keyPickup = pickupId;
        spr->setInsetLeft(64);
        spr->setInsetRight(64);
        spr->setInsetTop(50);
        spr->setInsetBottom(50);

        spr->setScale(0.25);

        spr->setScaledContentSize({size});
        spr->setAnchorPoint({0, 0});
        if (isSpriteFrameName) {
            auto innerSpr =
                CCSprite::createWithSpriteFrameName(buttomLabel.c_str());
            innerSpr->setOpacity(128);
            
            float toScale = (spr->getContentHeight()-10) / innerSpr->getScaledContentHeight(); 

            innerSpr->setScale(toScale);


            m_inner = innerSpr;

            spr->addChildAtPosition(innerSpr, Anchor::Center);
        } else {
            auto innerText =
                CCLabelBMFont::create(buttomLabel.c_str(), "bigFont.fnt");
            innerText->setScale(4);     // 1 to normal scale
            innerText->setOpacity(128); // 0.5

            m_inner = innerText;

            spr->addChildAtPosition(innerText, Anchor::Center);
        };
        if (!CCMenuItemSpriteExtra::init(spr, spr, this, nullptr))
            return false;
        this->m_animationEnabled = false;
        this->setAnchorPoint({0, 0});
        return true;
    };

public:
    static MobileButton *
    create(std::string buttomLabel, bool isSpriteFrameName, CCSize size, int pickupId) {
        auto ret = new MobileButton();
        if (ret && ret->init(buttomLabel, isSpriteFrameName, size,pickupId)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

class $modify(MobileKeys, PlayLayer) {

    bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects))
            return false;
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

        auto overlay = CCMenu::create();
        overlay->setContentSize(screenSize);

        this->addChild(overlay);

        return true;
    }
};