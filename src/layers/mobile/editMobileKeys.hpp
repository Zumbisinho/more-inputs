#include "../../utils/keybindsCache.hpp"
#include "Geode/Geode.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/draw_nodes/CCDrawNode.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/loader/Log.hpp"
#include "ccTypes.h"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <vector>



using namespace geode::prelude;

class EditMobileKeys : public CCLayerColor {
public:
    static CCPoint getCenterOfNode(CCNode *node) {
        auto rect = node->boundingBox();
        auto x = rect.getMidX();
        auto y = rect.getMidY();
        return ccp(x, y);
    };
    struct CrossSnapLines {

        CCPoint pos;
        float thickness;
        ccColor4B color;

        CCPoint Snap(CCNode *node) {
            auto screenSize = CCDirector::sharedDirector()->getWinSize();
            auto xRect = CCRectMake(0, pos.y - thickness/2, screenSize.width, thickness);

            auto yRect = CCRectMake(pos.x - thickness/2, 0, thickness, screenSize.height);

            auto centerSquare = CCRectMake(pos.x - thickness/2, pos.y - thickness/2, thickness, thickness);

            auto nodeCenter = getCenterOfNode(node);

            // geode::log::warn("\nx {} \n y {}", nodeCenter.x, nodeCenter.y);
            // geode::log::warn(" \n --X Rect--\n Position min: {} {}    max: {} {}", xRect.getMinX(), xRect.getMinY(), xRect.getMaxX(), xRect.getMaxY());
            // geode::log::warn("\n Size x: {} y: {}", xRect.size.width, xRect.size.height);
            // geode::log::warn(" \n --Y Rect--\n Position min: {} {}    max: {} {}", yRect.getMinX(), yRect.getMinY(), yRect.getMaxX(), yRect.getMaxY());
            // geode::log::warn("\n Size x: {} y: {}", yRect.size.width, yRect.size.height);

            if (centerSquare.containsPoint(nodeCenter)) {
                return pos;
            };

            if (xRect.containsPoint(nodeCenter)) {
                return ccp(nodeCenter.x, pos.y);
            } else if (yRect.containsPoint(nodeCenter)) {
                return ccp(pos.x, nodeCenter.y);
            } else {
                return NullPoint;
            };
        };
    };

protected:
    void drawCross(CCPoint center, ccColor4B color);

    virtual void registerWithTouchDispatcher(void);

    virtual bool init();

    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

    virtual void keyBackClicked();

    CCNode *curDragging;
    std::vector<CCNode *> toDragNodes;
    std::vector<CrossSnapLines> toSnap;
    CCPoint sp;
    CCDrawNode *m_drawLayer;
    CrossSnapLines *curDrawing;

public:
    static constexpr CCPoint NullPoint = ccp(-167, -167);

    static EditMobileKeys *
    create() {
        auto ret = new EditMobileKeys();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    void addNode(CCMenuItemSpriteExtra *node, bool isLocked) {
        if (!isLocked)
            toDragNodes.push_back(node);
        else
            node->setColor(ccc3(170, 170, 170));
        addChild(node);
    }

    void addSnap(CrossSnapLines line) {
        toSnap.push_back(line);
    };
};