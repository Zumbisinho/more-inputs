#include "keybindLabel.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include <string>

KeyBindsSection *KeyBindsSection::create(
    const std::string &KeyName, const int DefaultKey, const CCSize &size,
    const std::function<void(CCObject*,std::string,int,CCLabelBMFont *)> callback
    ) {
    
    auto ret = new KeyBindsSection();

    if (ret && ret->init(KeyName, DefaultKey, size, callback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool KeyBindsSection::init(
    const std::string &KeyName = "", const int DefaultKey = 0,
    const CCSize &size = {60.f, 67.f},
    const std::function<void(CCObject*,std::string,int,CCLabelBMFont *)> callback = nullptr)
    {
    if (!CCMenu::init())
        return false;

    m_keyName = KeyName;
    m_callback = callback;
    
    m_defaultKey = keyToString(DefaultKey);
    m_keyCode = DefaultKey;
    ccColor3B bgColor;

    this->setLayout(
        RowLayout::create()->setAxisAlignment(AxisAlignment::Between));
    this->setContentSize(
        {size.width - 20, size.height}); // gap for the list borders

    auto keyName = CCLabelBMFont::create(m_keyName.c_str(), "bigFont.fnt");
    keyName->setAnchorPoint({0.f, 0.5f});
    keyName->setContentWidth(size.width / 2);
    if (m_keyName.size() > 20)
    {
        auto abreviated = m_keyName.substr(0,20) + "...";
        keyName->setString(abreviated.c_str());
    };


    m_actionNameLabel = keyName;

    auto keyBindsBtns = CCMenu::create();
    keyBindsBtns->setLayout(RowLayout::create()
                                ->setAxisAlignment(AxisAlignment::End)
                                ->setGap(10.f));

    auto keyDefaultSpr =
        CCLabelBMFont::create(m_defaultKey.c_str(), "bigFont.fnt");
    keyDefaultSpr->setAnchorPoint({0.5f, 0.4f});
    m_defaultKeyLabel = keyDefaultSpr;
    auto keyDefault =
        EditorButtonSprite::create(keyDefaultSpr, EditorBaseColor::Gray);
    keyDefault->setAnchorPoint({1.f, 0.5f});

    auto SettingsSpr =
        CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png");
    SettingsSpr->setScale(0.5f);
    auto SettingsBtn = CCMenuItemSpriteExtra::create(
        SettingsSpr, this, menu_selector(KeyBindsSection::onSettingsBtn));
    SettingsBtn->setAnchorPoint({0.5f, 0.5f});

    keyBindsBtns->addChild(SettingsBtn);
    keyBindsBtns->addChild(keyDefault);

    keyBindsBtns->setContentWidth(65.f);
    keyBindsBtns->updateLayout();

    this->addChild(keyName);
    this->addChild(keyBindsBtns);

    this->updateLayout();

    return true;
};
void KeyBindsSection::onSettingsBtn(CCObject *sender) {
    if (m_callback)
        m_callback(sender, m_keyName,m_keyCode,m_defaultKeyLabel);
    
};
