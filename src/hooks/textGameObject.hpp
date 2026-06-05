#pragma once
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/modify/TextGameObject.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(TouchMacroGameObject, TextGameObject) {
    struct Fields {
        CCLabelBMFont *m_groupLabel;
    };

    void setupCustomTrigger();

    void customObjectSetup(
        gd::vector<gd::string> &values, gd::vector<void *> &exists
    );
    void updateTextObject(gd::string text, bool defaultFont) ;
};