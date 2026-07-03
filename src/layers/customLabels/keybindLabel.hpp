#pragma once
#include "../../utils/keybindsCache.hpp"
#include "../../utils/keycodeToString.hpp"
#include "../keyEdit.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <functional>
#include <string>

using namespace geode::prelude;

class KeyBindsSection : public CCMenu {
protected:
    keybindsAPI::KeyFullSettings m_keySetting;
    std::string m_defaultKey;
    CCLabelBMFont *m_defaultKeyLabel;
    CCLabelBMFont *m_actionNameLabel;

    std::function<void(CCObject *, const keybindsAPI::KeyFullSettings )> m_callback;
    std::function<void(CCObject *, CCMenu *)> m_deleteCB;

    KeyBindsSection(const keybindsAPI::KeyFullSettings* initKey) : m_keySetting(*initKey){};

    bool init(
        const keybindsAPI::KeyFullSettings *key,
        const CCSize &size,
        const std::function<void(CCObject *, const keybindsAPI::KeyFullSettings )> callback
    );

public:
    static KeyBindsSection *create(
        const keybindsAPI::KeyFullSettings *key,
        const CCSize &size,
        const std::function<void(CCObject *, const keybindsAPI::KeyFullSettings )> callback
    );

    void onSettingsBtn(CCObject *sender);
};
