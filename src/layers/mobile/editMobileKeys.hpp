#include "../../utils/keybindsCache.hpp"
#include "Geode/Geode.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "ccTypes.h"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <vector>

// Thanks TheSillyDoggo For this function :3


using namespace geode::prelude;

class EditMobileKeys : public CCLayerColor {
protected:
    CCNode *curDragging;
    std::vector<CCNode *> toDragNodes;
    CCPoint sp;
    virtual void registerWithTouchDispatcher(void);

    virtual bool init();

    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    
    virtual void keyBackClicked();

public:
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
            node->setColor(ccc3(170,170,170));
        this->addChild(node);
        
    }



};