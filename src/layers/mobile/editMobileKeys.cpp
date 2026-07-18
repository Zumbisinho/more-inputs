#include "editMobileKeys.hpp"
#include "Geode/cocos/CCDirector.h"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/draw_nodes/CCDrawNode.h"
#include "Geode/cocos/support/CCPointExtension.h"
#include "Geode/loader/Log.hpp"
#include "ccTypes.h"

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

bool EditMobileKeys::init() {
    if (!CCLayerColor::initWithColor({45, 158, 176, 255}))
        return false;

    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    auto drawLayer = CCDrawNode::create();
    m_drawLayer = drawLayer;
    drawLayer->setContentSize(screenSize);
    drawLayer->setPosition({0,0});
    addChild(drawLayer);

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

    if (!this->boundingBox().containsPoint(centerPos)) {  // if the center of the node is not on the screen
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
