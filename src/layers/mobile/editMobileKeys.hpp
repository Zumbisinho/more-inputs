#include "../../hooks/editorLayer/triggerUI/builderMacros.hpp"
#include "../../utils/keybindsCache.hpp"
#include "../../utils/keycodeToString.hpp"
#include "Geode/Geode.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/draw_nodes/CCDrawNode.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/platform/CCPlatformMacros.h"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "ccTypes.h"
#include "mobileKeys.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <functional>
#include <string>
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
    static CCPoint canvaPosToRelative(CCPoint canvaPos) {
        auto screenSize = CCDirector::sharedDirector()->getWinSize();
        return ccp(canvaPos.x / screenSize.width, canvaPos.y / screenSize.height);
    };
    struct CrossSnapLines {

        CCPoint pos;
        float thickness;
        ccColor4B color;

        CCPoint Snap(CCNode *node) {
            auto screenSize = CCDirector::sharedDirector()->getWinSize();
            auto xRect = CCRectMake(0, pos.y - thickness / 2, screenSize.width, thickness);

            auto yRect = CCRectMake(pos.x - thickness / 2, 0, thickness, screenSize.height);

            auto centerSquare = CCRectMake(pos.x - thickness / 2, pos.y - thickness / 2, thickness, thickness);

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

    virtual bool init(bool canSave);

    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);

    virtual void keyBackClicked();

    CCNode *curDragging;
    std::vector<CCNode *> toDragNodes;
    std::vector<CrossSnapLines> toSnap;
    std::vector<std::pair<MobileButton *, CCMenuItemSpriteExtra *>> toEdit;
    CCPoint sp;
    CCDrawNode *m_drawLayer;
    CrossSnapLines *curDrawing;
    CCNode *m_selectMenu;
    CCNode *m_selectArrow;
    CCMenu *m_contentLayer;
    ScrollLayer *m_scrollLayer;
    bool m_isSelectMenuOpen = false;
    std::pair<MobileButton *, CCMenuItemSpriteExtra *> m_curEditing;
    bool m_canSnap = true;

public:
    static constexpr CCPoint NullPoint = ccp(-167, -167);

    static EditMobileKeys *
    create(bool canSave) {
        auto ret = new EditMobileKeys();
        if (ret && ret->init(canSave)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    void addNode(CCMenuItemSpriteExtra *node, bool isLocked, bool isEditable) {
        auto nodeConverted = static_cast<MobileButton *>(node);
        if (isEditable) {
            if (!nodeConverted)
                return;
            auto keySettings = nodeConverted->getKey()->second;
            // generates the edit Node
            auto editNode = CCMenu::create();
            editNode->setContentSize({230, 40});

            auto keyName = CCLabelBMFont::create(keySettings.name.c_str(), "bigFont.fnt");
            keyName->setScale(0.8);
            keyName->setAnchorPoint({0, 1});
            keyName->setPosition({0, 40});
            auto keyCode = CCLabelBMFont::create(("Binded to [" + keyToString(keySettings.keyCode) + "] key").c_str(), "bigFont.fnt");
            keyCode->setScale(0.5);
            keyCode->setAnchorPoint({0, 0});
            keyCode->setPosition({0, 0});

            auto selectArrow = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
                this,
                menu_selector(EditMobileKeys::onSelect)
            );
            selectArrow->setAnchorPoint({0.5, 0.5});
            selectArrow->setPosition({210, 20});

            editNode->addChild(keyName);
            editNode->addChild(keyCode);
            editNode->addChild(selectArrow);

            toEdit.push_back({nodeConverted, selectArrow});

            if (toEdit.size() == 1) // if its the first to edit, it should not be selected
            {
                selectArrow->setVisible(false);
                selectArrow->setEnabled(false);
                m_curEditing = {nodeConverted, selectArrow};
            };

            selectArrow->setTag(toEdit.size() - 1); //? size to index // get the index of the new item

            m_contentLayer->addChild(editNode);
            m_contentLayer->updateLayout();
            m_scrollLayer->m_contentLayer->setContentHeight(m_contentLayer->getContentHeight());
            m_scrollLayer->scrollToTop();

    
        };
        if (!isLocked) {
            toDragNodes.push_back(node);

            if (nodeConverted) {
                nodeConverted->setLocked(false);
            }
        } else {
            node->setColor(ccc3(170, 170, 170));
            if (nodeConverted)
                nodeConverted->setLocked(true);
        }
        addChild(node);
    }

    void onSelect(CCObject *sender) {
        auto oldMobileButton = m_curEditing.first;
        auto oldSelectButton = m_curEditing.second;

        auto newMobileButton = toEdit[sender->getTag()].first;
        auto newSelectButton = toEdit[sender->getTag()].second;

        oldSelectButton->setVisible(true);
        oldSelectButton->setEnabled(true);

        newSelectButton->setVisible(false);
        newSelectButton->setEnabled(false);

        std::erase(toDragNodes, oldMobileButton);
        toDragNodes.push_back(newMobileButton);

        oldMobileButton->setColor(ccc3(170, 170, 170));
        newMobileButton->setColor(ccc3(255, 255, 255));

        m_curEditing = {newMobileButton,newSelectButton};

        resetSnaps();
        calcSnaps();
    };

    void addSnap(CrossSnapLines line) {
        toSnap.push_back(line);
    };

    void resetSnaps(){
        toSnap.clear();
    };

    void saveKeybinds();

    void updateKeys();

    void onMenuOpen(CCObject *sender);

    void calcSnaps();

    void onSnapLock(CCObject* sender);
};