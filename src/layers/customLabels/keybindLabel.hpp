#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "../../utils/keycodeToString.hpp"

using namespace geode::prelude;

class KeyBindsSection : public CCMenu {
protected:
    std::string m_keyName;
    std::string m_defaultKey;

    bool init(
        const std::string& KeyName,
        const int DefaultKey,
        const CCSize& size
        
    );

public:
    
    static KeyBindsSection* create(
        const std::string& KeyName,
        const int DefaultKey,
        const CCSize& size
        
    );

    void onSettingsBtn(CCObject* sender);
};

