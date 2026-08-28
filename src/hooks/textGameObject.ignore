
#include "textGameObject.hpp"
#include "../utils/customTriggers.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <fmt/format.h>
#include <string>

using namespace geode::prelude;

void CustomTriggerGameObject::setupCustomTrigger() {
    auto view = std::string_view(m_text);
    std::string modName;
    std::unique_ptr<customTriggers::CustomTrigger> *found = nullptr;

    for (auto &[modNameIt, triggers] : customTriggers::customTriggersPerMod) {
        if (!view.starts_with(modNameIt))
            continue;

        for (auto &trigger : triggers) {
            if (view.starts_with(modNameIt + ": " + std::to_string(trigger->customObjID))) {
                modName = modNameIt;
                found = &trigger;
                break;
            }
        }

        if (found)
            break;
    }

    if (!found) // Normal text
        return;
    if (this->getID() == "custom-trigger"_spr) // if Not added
        return;
    // some trigger definitions
    m_isTrigger = true;
    m_detailColor = nullptr;
    m_addToNodeContainer = true;
    for (auto letter : getChildrenExt()) { // removes default label
        letter->setVisible(false);
    }
    if (!LevelEditorLayer::get()) // if editor needs the spr, else it just invisible anyways
        return;
    CCNodeRGBA *groupContainer = CCNodeRGBA::create();
    groupContainer->setAnchorPoint({0.5f, 0.5f});
    groupContainer->ignoreAnchorPointForPosition(false);
    groupContainer->setZOrder(1);
    groupContainer->setScale(0.5f);
    groupContainer->setCascadeOpacityEnabled(true);
    groupContainer->setVisible(true);

    auto pos = m_text.find(':');
    std::string strippedConfig = "";
    if (pos != std::string::npos) {
        strippedConfig = m_text.substr(pos + 1);
    }
    auto &config = (*found);
    config->loadFromTriggerStr(strippedConfig);

    std::string groupString = config->getFormatedGroupLabel();

    auto groupLabel = CCLabelBMFont::create(groupString.c_str(), "bigFont.fnt");
    groupContainer->setContentSize(groupLabel->getContentSize());

    groupLabel->setPosition(groupContainer->getContentSize() * 0.5f);

    m_fields->m_groupLabel = groupLabel;

    groupContainer->addChildAtPosition(
        groupLabel, Anchor::Center, config->groupLabelOffset
    ); // centralize with the blue ball from my custom sprite
    addChild(groupContainer);

    auto sprStr = config->getSpriteStr();
    geode::log::warn("{}", sprStr);
    auto spr = CCSprite::createWithSpriteFrameName(sprStr);
    this->setID("custom-trigger"_spr);

    log::info("spr = {}", spr);
    log::info("label = {}", groupLabel);
    float toScale =
        spr->getScaledContentWidth() / groupLabel->getScaledContentWidth();
    groupLabel->setScale(toScale * 1.5);

    setContentSize(spr->getContentSize());
    updateOrientedBox();

    spr->setPosition(getContentSize() * 0.5f);
    groupContainer->setPosition({getContentWidth() * 0.5f, 10.5f});

    addChild(spr);

    spr->setColor(getColor());
    spr->release();
};
void CustomTriggerGameObject::customObjectSetup(
    gd::vector<gd::string> &values, gd::vector<void *> &exists
) {
    TextGameObject::customObjectSetup(values, exists);
    CustomTriggerGameObject::setupCustomTrigger();
};
void CustomTriggerGameObject::updateTextObject(gd::string text, bool defaultFont) {
    TextGameObject::updateTextObject(text, defaultFont);
    CustomTriggerGameObject::setupCustomTrigger();
};
