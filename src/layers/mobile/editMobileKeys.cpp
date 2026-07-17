#include "editMobileKeys.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/loader/Log.hpp"

using namespace geode::prelude;

// helper functions
CCPoint getCenterOfNode(CCNode *node) {
    auto rect = node->boundingBox();
    auto x = rect.getMidX();
    auto y = rect.getMidY();
    return ccp(x, y);
};

bool EditMobileKeys::init() {
    if (!CCLayerColor::initWithColor({45, 158, 176, 255}))
        return false;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    this->setZOrder(120);
    this->setContentSize(screenSize);
    this->setPosition({0, 0});
    this->setAnchorPoint({0, 0});

    return true;
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
    if (this->boundingBox().containsPoint(centerPos))
        return; 
    // if the center of the node is not on the screen
    auto curSize = curDragging->getScaledContentSize();
    auto screenSize = CCDirector::sharedDirector()->getWinSize();
    bool isXAxis = !this->boundingBox().containsPoint(centerPos * ccp(1, 0));
    bool isYAxis = !this->boundingBox().containsPoint(centerPos * ccp(0, 1));
    int correctionMargin = 1;

    // correct X Pos
    if (isXAxis) {
        if (centerPos.x > 0)
            curDragging->setPositionX((screenSize.width - curSize.width / 2) - correctionMargin);
        else
            curDragging->setPositionX(-(curSize.width / 2) + correctionMargin);
    };

    // correct Y pos
    if (isYAxis) {
        if (isYAxis && centerPos.y > 0)
            curDragging->setPositionY((screenSize.height - curSize.height / 2) - correctionMargin);
        else
            curDragging->setPositionY(-(curSize.height / 2) + correctionMargin);
    };
}
void EditMobileKeys::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent) {
    curDragging = nullptr;
};
void EditMobileKeys::ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent) {
    ccTouchEnded(pTouch, pEvent);
}
// onClose
void EditMobileKeys::keyBackClicked() {
    this->removeMeAndCleanup();
}
