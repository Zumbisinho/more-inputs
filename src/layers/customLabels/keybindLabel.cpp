#include "keybindLabel.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/loader/Log.hpp"
#include <string>

KeyBindsSection *KeyBindsSection::create(
    const keybindsAPI::KeyFullSettings* key,
    const CCSize &size = {60.f, 67.f},
    const std::function<void(CCObject *, const keybindsAPI::KeyFullSettings)> callback = nullptr
) {
    geode::log::warn("\nItem {}: {} {}",key->first,key->second.name,key->second.keyCode);
    auto ret = new KeyBindsSection(key);

    if (ret && ret->init(key, size, callback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool KeyBindsSection::init(
    const keybindsAPI::KeyFullSettings *key,
    const CCSize &size = {60.f, 67.f},
    const std::function<void(CCObject *, const keybindsAPI::KeyFullSettings)> callback = nullptr
) {
    if (!CCMenu::init())
        return false;
    if (!key)
        return false;
    
    log::warn("actionId = {}", m_keySetting.first);
    log::warn("keycode = {}", m_keySetting.second.keyCode);
    // ! Ponteiro invalido, ta copiando o nada e allocando um gazetals de memoria, arrumar isso
    auto keyString = m_keySetting.second.name;
    m_callback = callback;

    m_defaultKey = keyToString(m_keySetting.second.keyCode);
    ccColor3B bgColor;

    this->setLayout(
        RowLayout::create()->setAxisAlignment(AxisAlignment::Between)
    );
    this->setContentSize(
        {size.width - 20, size.height}
    ); // gap for the list borders

    auto keyName = CCLabelBMFont::create(keyString.c_str(), "bigFont.fnt");
    keyName->setAnchorPoint({0.f, 0.5f});
    keyName->setContentWidth(size.width / 2);
    if (keyString.size() > 20) {
        auto abreviated = keyString.substr(0, 20) + "...";
        keyName->setString(abreviated.c_str());
    };

    m_actionNameLabel = keyName;

    auto keyBindsBtns = CCMenu::create();
    keyBindsBtns->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End)->setGap(10.f));

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
        SettingsSpr, this, menu_selector(KeyBindsSection::onSettingsBtn)
    );
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
        m_callback(sender, m_keySetting);
};
