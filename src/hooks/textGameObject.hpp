#pragma once
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/modify/TextGameObject.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>

using namespace geode::prelude;

class $modify(CustomTriggerGameObject, TextGameObject) {
    struct Fields {
        CCLabelBMFont *m_groupLabel;
    };

    void setupCustomTrigger();

    void customObjectSetup(
        gd::vector<gd::string> &values, gd::vector<void *> &exists
    );
    void updateTextObject(gd::string text, bool defaultFont) ;
};