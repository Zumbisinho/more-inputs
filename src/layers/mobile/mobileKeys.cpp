#include "mobileKeys.hpp"


using namespace geode::prelude;

void MobileButton::selected()  {
    log::info("My custom button was activated! {}", m_keyPickup);
    this->setColor({128, 128, 128});

    CCRGBAProtocol *rgba = typeinfo_cast<CCRGBAProtocol *>(m_inner);
    if (rgba != nullptr) {
        rgba->setColor(ccColor3B{128, 128, 128});
    };
    pickupManager::changePickupId(m_keyPickup, 1);
}
void MobileButton::unselected()  {
    log::info("My custom button was de-activated! {}", m_keyPickup);
    this->setColor({255, 255, 255});

    CCRGBAProtocol *rgba = typeinfo_cast<CCRGBAProtocol *>(m_inner);
    if (rgba != nullptr) {
        rgba->setColor(ccColor3B{255, 255, 255});
    };
    pickupManager::changePickupId(m_keyPickup, 0);
}

bool MobileButton::init(keybindsAPI::KeyFullSettings* key) {
    auto spr = CCScale9Sprite::createWithSpriteFrameName(
        "mobileButtomTemplateMIP.png"_spr
    );
    std::string buttomLabel = key->second.buttonLabel;
    bool isSpriteFrameName = key->second.isSpr;
    CCSize size = key->second.contentSize;
    int pickupId = key->first;
    m_key = key;

    m_keyPickup = pickupId;
    spr->setInsetLeft(64);
    spr->setInsetRight(64);
    spr->setInsetTop(50);
    spr->setInsetBottom(50);

    spr->setScale(0.25);

    spr->setScaledContentSize({size});
    spr->setAnchorPoint({0, 0});
    if (isSpriteFrameName) {
        auto innerSpr =
            CCSprite::createWithSpriteFrameName(buttomLabel.c_str());
        innerSpr->setOpacity(128);

        float toScale = (spr->getContentHeight() - 10) / innerSpr->getScaledContentHeight();

        innerSpr->setScale(toScale);

        m_inner = innerSpr;

        spr->addChildAtPosition(innerSpr, Anchor::Center);
    } else {
        auto innerText =
            CCLabelBMFont::create(buttomLabel.c_str(), "bigFont.fnt");
        float toScale = (spr->getScaledContentWidth() - 10) / innerText->getScaledContentWidth();
        toScale = std::min(1.25f,toScale); // max scale to 5 non-normal scale

        innerText->setScale(toScale * 4); // 1 to normal scale
        innerText->setOpacity(128);       // 0.5

        m_inner = innerText;

        spr->addChildAtPosition(innerText, Anchor::Center);
    };
    if (!CCMenuItemSpriteExtra::init(spr, spr, this, nullptr))
        return false;
    this->m_animationEnabled = false;
    this->setAnchorPoint({0, 0});
    return true;
};
