#include "editMobileKeys.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/actions/CCActionEase.h"
#include "Geode/cocos/actions/CCActionInterval.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/draw_nodes/CCDrawNode.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/cocos/support/CCPointExtension.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "ccTypes.h"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <algorithm>

using namespace geode::prelude;

// helper functions

ccColor4F toColor4F(const ccColor4B &color) {
    return {
        color.r / 255.f,
        color.g / 255.f,
        color.b / 255.f,
        color.a / 255.f
    };
}

void EditMobileKeys::drawCross(CCPoint center, ccColor4B color) {
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    const int thickness = 1;

    auto xFrom = ccp(0, center.y);
    auto xTo = ccp(screenSize.width, center.y);

    auto yFrom = ccp(center.x, 0);
    auto yTo = ccp(center.x, screenSize.height);

    // X line
    m_drawLayer->drawSegment(xFrom, xTo, thickness, toColor4F(color));

    m_drawLayer->drawSegment(yFrom, yTo, thickness, toColor4F(color));
};

void EditMobileKeys::calcSnaps(){
    auto screenSize = CCDirector::sharedDirector()->getWinSize();

    auto centerSnapLine = EditMobileKeys::CrossSnapLines{
            ccp(screenSize.width / 2.f, screenSize.height / 2.f),
            20,
            ccc4(255, 0, 0, 128)
        };
    this->addSnap(centerSnapLine);

    for (auto& node : toEdit){
        if (node.first == m_curEditing.first) // ignore the editing node
            continue;
        auto snapLine = EditMobileKeys::CrossSnapLines{
            node.first->getPosition(),
            15,
            ccc4(0, 195, 255, 128)
        };
        
        this->addSnap(snapLine);
    };
};

bool EditMobileKeys::init(bool canSave) {
    if (!CCLayerColor::initWithColor({45, 158, 176, 255}))
        return false;

    std::function<void()> okButtonCB;
    if (canSave){
        okButtonCB = [this]() {
            saveKeybinds();
            removeMeAndCleanup();
        };
    } else {
        okButtonCB = [this]() {
            removeMeAndCleanup();
            for (auto& node: toEdit){
                int actionId = node.first->getKey()->first;
                if (auto it = KeybindCache::mobileKeyNodes.find(actionId);it != KeybindCache::mobileKeyNodes.end()){
                    it->second->setPosition(node.first->getPosition());
                };
            };
        };
    };

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    auto drawLayer = CCDrawNode::create();
    m_drawLayer = drawLayer;
    drawLayer->setContentSize(screenSize);
    drawLayer->setPosition({0, 0});
    addChild(drawLayer);

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    this->setZOrder(120);
    this->setContentSize(screenSize);
    this->setPosition({0, 0});
    this->setAnchorPoint({0, 0});
    //edit this bull shittt 🤣🫱
    auto saveKey = GoffyBuilder::OkButton::create(okButtonCB);

    saveKey->setPosition(screenSize.width / 2, 24);
    saveKey->setAnchorPoint({0.5, 0.5});
    saveKey->setZOrder(1001);
    addChild(saveKey);

    // select menu

    auto selectMenu = CCScale9Sprite::create("GJ_square06.png");
    selectMenu->setContentSize({240.f, 300.f});
    selectMenu->setColor(ccc3(0, 45, 63));
    selectMenu->setAnchorPoint({0, 0});
    selectMenu->setZOrder(10);  
    m_selectMenu = selectMenu;

    std::string levelName = GJBaseGameLayer::get()->m_level->m_levelName;

    auto selectMenuTitle = CCLabelBMFont::create((levelName + " Keybinds").c_str(), "bigFont.fnt");
    selectMenuTitle->setScale(std::min(0.75f, 220.f / selectMenuTitle->getContentWidth()));
    selectMenuTitle->setAnchorPoint({0.5, 1});
    selectMenuTitle->setPosition({120, 300 - 5});

    selectMenu->addChild(selectMenuTitle);

    auto selectMenuScrollLayer = ScrollLayer::create({240, 300 - selectMenuTitle->getContentHeight() - 20}, true, true);
    selectMenuScrollLayer->setAnchorPoint({0, 0});
    selectMenuScrollLayer->setPosition({0, 10});
    m_scrollLayer = selectMenuScrollLayer;

    selectMenu->addChild(selectMenuScrollLayer);

    auto selectMenuContent = CCMenu::create();
    selectMenuContent->setContentSize({230, 100});
    selectMenuContent->setAnchorPoint({0, 0});
    selectMenuContent->setPosition({5, 0});
    selectMenuContent->setLayout(ColumnLayout::create()->setAutoScale(false)->setGap(5)->setAxisAlignment(AxisAlignment::Start)->setAxisReverse(true)->setAutoGrowAxis(80));
    

    m_contentLayer = selectMenuContent;

    selectMenuScrollLayer->m_contentLayer->addChild(selectMenuContent);
    selectMenuScrollLayer->m_contentLayer->setContentHeight(
        selectMenuContent->getContentHeight()
    );
    selectMenuScrollLayer->scrollToTop();

    auto selectMenuButtonWrapper = CCMenu::create();
    selectMenuButtonWrapper->setAnchorPoint({0, 1});
    selectMenuButtonWrapper->setPosition({238, 290});

    auto selectMenuButtonSpr = CCSprite::createWithSpriteFrameName("selectMenuTemplateMIP.png"_spr);
    selectMenuButtonSpr->setScale(0.2);

    auto selectMenuButton = CCMenuItemSpriteExtra::create(
        selectMenuButtonSpr,
        this,
        menu_selector(EditMobileKeys::onMenuOpen)
    );
    
    selectMenuButton->m_animationEnabled = false;
    selectMenuButton->setAnchorPoint({0,1});
    
    selectMenuButtonWrapper->setContentSize(selectMenuButton->getContentSize());

    auto selectMenuButtonArrow = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
    selectMenuButtonArrow->setScale(0.6);
    selectMenuButtonArrow->setOpacity(128);
    selectMenuButtonArrow->setAnchorPoint({0.5, 0.5});
    m_selectArrow = selectMenuButtonArrow;

    selectMenuButton->addChildAtPosition(selectMenuButtonArrow, Anchor::Center);
    selectMenuButtonWrapper->addChild(selectMenuButton);

    selectMenu->addChild(selectMenuButtonWrapper);

    addChild(selectMenu);

    selectMenu->setPosition({-240, 10});

    // snap toggle

    auto snapWrapper = CCMenu::create();

    auto snapBtn = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("warpLockOnBtn_001.png"),
            CCSprite::createWithSpriteFrameName("warpLockOffBtn_001.png"),
            this, menu_selector(EditMobileKeys::onSnapLock)
    );
    snapWrapper->setContentSize(snapBtn->getContentSize());
    snapBtn->setOpacity(128);
    snapBtn->setZOrder(1000);
    snapWrapper->setPosition({screenSize.width /2 - 50,24});
    snapBtn->setAnchorPoint({0.5,0.5});
    snapWrapper->setAnchorPoint({0.5,0.5});

    snapWrapper->addChild(snapBtn);
    addChild(snapWrapper);



    return true;
};

void EditMobileKeys::onMenuOpen(CCObject* sender){
    int direction = m_isSelectMenuOpen ? -1 : 1;
    m_selectMenu->runAction(CCEaseExponentialInOut::create(CCMoveBy::create(0.5,ccp(240,0) * direction)));
    m_isSelectMenuOpen = !m_isSelectMenuOpen;
    m_selectArrow->setScaleX(-0.6 * direction);
};

void EditMobileKeys::saveKeybinds() {
    for (auto &node : toEdit) {
        if (auto keybind = node.first; keybind != nullptr) {
            auto keySettings = keybind->getKey()->second;
            auto keyPickupId = keybind->getKey()->first;
            geode::log::warn("Changing key {} to pos {}{}", keybind->getKey()->second.buttonLabel, keybind->getKey()->second.pos.x, keybind->getKey()->second.pos.y);
            keybindsAPI::editLevelKeyBind(LevelEditorLayer::get(), keyPickupId, &keySettings);
        };
    }
};

void EditMobileKeys::registerWithTouchDispatcher(void) {
    CCTouchDispatcher::get()->addTargetedDelegate(this, -500, true);
}

bool EditMobileKeys::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent) {
    log::warn("Clicado");
    if (!this->isVisible())
        return false;

    for (auto &node : toDragNodes) {
        if (node && node->isVisible()) {
            auto rect = node->boundingBox();
            rect.origin = ccp(0, 0);

            if (rect.containsPoint(node->convertToNodeSpace(pTouch->getLocation()))) {
                curDragging = node;
                sp = node->getPosition();
            }
        }
    }

    return true;
}
void EditMobileKeys::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent) {
    if (!curDragging) // Not dragging anything
        return;
    curDragging->setPosition(sp + (pTouch->getLocation() - pTouch->getStartLocation()));
    auto centerPos = getCenterOfNode(curDragging);

    if (!this->boundingBox().containsPoint(centerPos)) { // if the center of the node is not on the screen
        auto curSize = curDragging->getScaledContentSize();
        auto screenSize = CCDirector::sharedDirector()->getWinSize();
        bool isXAxis = !this->boundingBox().containsPoint(centerPos * ccp(1, 0));
        bool isYAxis = !this->boundingBox().containsPoint(centerPos * ccp(0, 1));
        int correctionMargin = 1;

        // correct X Pos
        if (isXAxis) {
            if (centerPos.x > 0)
                curDragging->setPositionX(screenSize.width - correctionMargin);
            else
                curDragging->setPositionX(correctionMargin);
        };

        // correct Y pos
        if (isYAxis) {
            if (isYAxis && centerPos.y > 0)
                curDragging->setPositionY(screenSize.height - correctionMargin);
            else
                curDragging->setPositionY(correctionMargin);
        };
    }

    // snap logic
    if (!m_canSnap)
        return;
    for (auto &snapLine : toSnap) {
        auto whereSnap = snapLine.Snap(curDragging);

        if (whereSnap == NullPoint) { // Cannot snap
            geode::log::warn("Theres nothing to snap!");
            if (curDrawing) { // removes any drawn line
                curDrawing = nullptr;
                m_drawLayer->clear();
            };
            continue;
        }
        curDragging->setPosition(whereSnap);

        if (curDrawing != &snapLine) {
            curDrawing = &snapLine;
            m_drawLayer->clear();
            drawCross(snapLine.pos, snapLine.color);
        }

        break;
    }
}
void EditMobileKeys::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
    if (auto keybind = static_cast<MobileButton *>(curDragging); keybind != nullptr) { // updates internal keyValue

        keybind->getKey()->second.pos = canvaPosToRelative(keybind->getPosition());
        geode::log::warn("Setting up {} to {},{}", keybind->getKey()->second.name, keybind->getKey()->second.pos.x, keybind->getKey()->second.pos.y);
    };
    curDragging = nullptr;
    curDrawing = nullptr;
    m_drawLayer->clear();
};

void EditMobileKeys::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
    ccTouchEnded(pTouch, pEvent);
}
// onClose
void EditMobileKeys::keyBackClicked() {
    this->removeMeAndCleanup();
}

void EditMobileKeys::onSnapLock(CCObject* sender){
    auto state = static_cast<CCMenuItemToggler*>(sender)->m_toggled;

    m_canSnap = state;
};
