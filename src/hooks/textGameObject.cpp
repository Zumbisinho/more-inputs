#include "Geode/modify/TextGameObject.hpp"
#include "../utils/macroClasses.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <fmt/format.h>

using namespace geode::prelude;

class $modify(TouchMacroGameObject, TextGameObject) {
    struct Fields {
        CCLabelBMFont *m_groupLabel;
    };

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
        auto config = macroTriggers::triggerStrToConfig(m_text);
        std::string groupString;
        if (!config.empty()) {
            if (config[0] == -1) // getCreateBtn call
                groupString = "";
            else {
                int pressGroupId = config[1];
                int releaseGroupId = config[2];

                groupString = fmt::format(
                    "{}/{}",
                    macroTriggers::triggerStrToConfig(
                        m_text
                    )[1], // pressGroup Id
                    macroTriggers::triggerStrToConfig(
                        m_text
                    )[2] // releaseGroup Id
                );
            };
        } else
            groupString = "0/0";

        geode::log::warn("{} str {}", groupString, config);
        auto groupLabel =
            CCLabelBMFont::create(groupString.c_str(), "bigFont.fnt");

        groupLabel->setPosition(triggerContainer->getContentSize() * 0.5f);
        m_fields->m_groupLabel = groupLabel;

        addChild(triggerContainer);

        auto spr = CCSprite::createWithSpriteFrameName("touch_macro.png"_spr);
        spr->setID("touch-macro"_spr);

        float toScale =
            spr->getScaledContentWidth() / groupLabel->getScaledContentWidth();
        groupLabel->setScale(toScale * 0.8);

        setContentSize(spr->getContentSize());
        m_width = getContentWidth();
        m_height = getContentHeight();
        updateOrientedBox();

        spr->setPosition(getContentSize() * 0.5f);
        triggerContainer->setPosition({getContentWidth() * 0.5f, 10.5f});

        addChild(spr);
        addChildAtPosition(
            groupLabel, Anchor::Center, {0, -4}
        ); // centralize with the blue ball from my custom sprite

        spr->setColor(getColor());
    };
    void customObjectSetup(
        gd::vector<gd::string> &values, gd::vector<void *> &exists
    ) {
        TextGameObject::customObjectSetup(values, exists);
        setupCustomTrigger();
    };
    void updateTextObject(gd::string text, bool defaultFont) {
        TextGameObject::updateTextObject(text, defaultFont);
        setupCustomTrigger();
    };
};