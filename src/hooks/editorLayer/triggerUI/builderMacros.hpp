#pragma once

#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"

#include "Geode/ui/SliderNode.hpp"
#include "Geode/ui/TextInput.hpp"
#include "Geode/utils/general.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/Slider.hpp>
#include <functional>

#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;

// ? this is a goffy set of custom widgets that mimics the componets of RobTop
// triggers

namespace GoffyBuilder {

enum class ExtraInputWidget {
    None = 0,
    Arrows = 1,
    Slider = 2,
    U_Slider = 3,
    TimeSlider = 4,
    RangeSlider = 5,
    U_RangeSlider = 6
};
class OkButton : public CCMenu {
public:
    static OkButton *create(std::function<void()> callback);

protected:
    std::function<void()> m_callback;

protected:
    bool init(std::function<void()> callback);
    void onClick(CCObject *sender);
};

class DropDownList : public CCMenu {
public:
    static DropDownList *create(
        std::string labelName, float size, std::vector<std::string> itemList
    );

protected:
    bool m_isOpen;
    CCLabelBMFont *m_label;
    CCSprite *m_curArrow;
    int m_curIndex;
    std::vector<std::string> m_itemList;
    CCScale9Sprite *m_listWidget;

protected:
    bool
    init(std::string labelName, float size, std::vector<std::string> itemList);
    void onClick(CCObject *sender);
    void onItemSelect(CCObject *sender);
    CCMenu *dropList(float size);
    CCScale9Sprite *DropDown(float size);
};

class ToggleOption : public CCMenu {
public:
    static ToggleOption *
    create(std::string labelName, float size, ToggleOption *showOnCheck);

protected:
    bool m_isChecked;
    CCMenuItemSpriteExtra *m_checkBtn;
    std::pair<CCSprite *, CCSprite *> m_sprites;
    ToggleOption *m_onCheck;

protected:
    bool init(std::string labelName, float size, ToggleOption *showOnCheck);
    void onCheck(CCObject *sender);
};
class NumericInput : public CCMenu {
public:
    static NumericInput *create(
        std::string labelName,
        float size,
        ExtraInputWidget ExtraInputWidget,
        bool hasGoldText,
        bool isVertical
    );
    float getNumber() {
        return numFromString<float>(m_input->getString()).unwrapOr(0);
    };

protected:
    TextInput *m_input;
    ExtraInputWidget m_side;
    SliderNode *m_slider;
    float m_maxSlider;
    float m_minSlider;
    bool m_supressSlider;

protected:
    bool init(
        std::string labelName,
        float size,
        ExtraInputWidget ExtraInputWidget,
        bool hasGoldText,
        bool isVertical
    );
    void onSliderChanged(SliderNode *sender, float value);

    void add(CCObject *sender);
    void remove(CCObject *sender);
};
}; // namespace GoffyBuilder