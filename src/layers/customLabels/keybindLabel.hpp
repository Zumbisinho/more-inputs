#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <functional>
#include <string>
#include "../../utils/keycodeToString.hpp"
#include "../../utils/keybindsCache.hpp"
#include "../keyEdit.hpp"
#include "Geode/cocos/cocoa/CCObject.h"


using namespace geode::prelude;

class KeyBindsSection : public CCMenu {
protected:
    std::string m_keyName;
    std::string m_defaultKey;
    CCLabelBMFont* m_defaultKeyLabel;
    CCLabelBMFont* m_actionNameLabel;

    int m_keyCode;
    std::function<void(CCObject*,std::string,int,CCLabelBMFont *)> m_callback;
    std::function<void(CCObject*,CCMenu*)> m_deleteCB;

    bool init(
        const std::string& KeyName,
        const int DefaultKey,
        const CCSize& size,
        const std::function<void(CCObject*,std::string,int,CCLabelBMFont *)> callback
        
        
    );

public:
    
    static KeyBindsSection* create(
        const std::string& KeyName,
        const int DefaultKey,
        const CCSize& size,
        const std::function<void(CCObject*,std::string,int,CCLabelBMFont *)> callback
        
        
    );

    void onSettingsBtn(CCObject* sender);
};

