#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/modify/TextGameObject.hpp"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include <Geode/binding/LevelEditorLayer.hpp>
using namespace geode::prelude;

class $modify(MyTextGameObject, TextGameObject) {
    void setupCustomTrigger() {
        if (!m_text.starts_with("more_inputs:"))
            return;
        // some trigger definitions
        m_isTrigger = true;
        m_detailColor = nullptr;
        m_addToNodeContainer = true;
        for (auto letter : getChildrenExt()) { // removes default label
            letter->setVisible(false);
        }

        if (!LevelEditorLayer::get()) // if editor needs the spr, else it just
                                      // invisible anyways
            return;
        CCNodeRGBA *triggerContainer = CCNodeRGBA::create();
        triggerContainer->setAnchorPoint({0.5f, 0.5f});
        triggerContainer->ignoreAnchorPointForPosition(false);
        triggerContainer->setZOrder(1);
        triggerContainer->setScale(0.5f);
        triggerContainer->setCascadeOpacityEnabled(true);

        auto groupLabel = CCLabelBMFont::create("0", "bigFont.fnt");
        triggerContainer->setContentSize(groupLabel->getContentSize());
        groupLabel->setPosition(triggerContainer->getContentSize() * 0.5f);

        addChild(triggerContainer);

        auto spr = CCSprite::createWithSpriteFrameName("touch_macro.png"_spr);
        spr->setID("touch-macro"_spr);

        setContentSize(spr->getContentSize());
        m_width = getContentWidth();
        m_height = getContentHeight();
        updateOrientedBox();

        spr->setPosition(getContentSize() * 0.5f);
        triggerContainer->setPosition({getContentWidth() * 0.5f, 10.5f});

        addChild(spr);

        spr->setColor(getColor());

    };
    void customObjectSetup(gd::vector<gd::string>& values, gd::vector<void*>& exists) { 
        TextGameObject::customObjectSetup(values, exists);
        setupCustomTrigger();
    };
    void updateTextObject(gd::string text, bool defaultFont) {
        TextGameObject::updateTextObject(text, defaultFont);
        setupCustomTrigger();
    };
};