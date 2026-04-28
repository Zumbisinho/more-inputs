#include "keybindLabel.hpp"


KeyBindsSection* KeyBindsSection::create(
    const std::string& KeyName,
    const int DefaultKey,
    const CCSize& size
) {
    auto ret = new KeyBindsSection();

    if (ret && ret->init(KeyName,DefaultKey,size)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool KeyBindsSection::init(
    const std::string& KeyName = "",
    const int DefaultKey = 0,
    const CCSize& size = {60.f,67.f}
) 
{
    if (!CCMenu::init()) return false;

    
    m_keyName = KeyName;
    m_defaultKey = keyToString(DefaultKey);
    ccColor3B bgColor;

    this->setLayout(RowLayout::create()
      ->setAxisAlignment(AxisAlignment::Between)
);
    this->setContentSize({size.width-20,size.height}); // gap for the list borders

    auto keyName = CCLabelBMFont::create(m_keyName.c_str(),"bigFont.fnt");
    keyName->setAnchorPoint({0.f,0.5f});
    keyName->setContentWidth(size.width/2);

    auto keyBindsBtns = CCMenu::create();
    keyBindsBtns->setLayout(RowLayout::create()
        ->setAxisAlignment(AxisAlignment::End)
        ->setGap(10.f)
    );

    auto keyDefaultSpr = CCLabelBMFont::create(m_defaultKey.c_str(),"bigFont.fnt");
    keyDefaultSpr->setAnchorPoint({0.5f,0.4f});
    auto keyDefault = EditorButtonSprite::create(keyDefaultSpr,EditorBaseColor::Gray);
    keyDefault->setAnchorPoint({1.f,0.5f});

    auto SettingsSpr = CCSprite::createWithSpriteFrameName("accountBtn_settings_001.png");
    SettingsSpr->setScale(0.5f);
    auto SettingsBtn = CCMenuItemSpriteExtra::create(SettingsSpr, this, menu_selector(KeyBindsSection::onSettingsBtn));
    SettingsBtn->setAnchorPoint({1.f,0.5f});
    
    keyBindsBtns->addChild(SettingsBtn);
    keyBindsBtns->addChild(keyDefault);

   
    keyBindsBtns->setContentWidth(size.width/2);
    keyBindsBtns->updateLayout();

    this->addChild(keyName);
    this->addChild(keyBindsBtns);
    


    

    


    this->updateLayout();
    
    return true;


};
void KeyBindsSection::onSettingsBtn(CCObject*sender)
{
    log::info("Clicado o botão");
};
