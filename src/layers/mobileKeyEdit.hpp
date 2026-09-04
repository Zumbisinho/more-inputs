#pragma once

#include "../hooks/editorLayer/triggerUI/builderMacros.hpp"
#include "../utils/keycodeToString.hpp"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include <Geode/Geode.hpp>
#include <cue/RadioLogic.hpp>


using namespace geode::prelude;

class setMobileKeyPopup : public Popup {

public:
    std::function<void(int)> m_callback;
    bool m_isAlive = true;
    GoffyBuilder::NumericInput* m_input;
    cue::RadioLogic<int> m_keys;
    CCMenuItemToggler *createButton(int keyCode, float scaleX) {
        CCSize size = {24, 24};

        auto keyOff = ButtonSprite::create(
            keyToString(keyCode).c_str(),
            size.width * scaleX,
            size.width,
            0.6f,
            true,
            "bigFont.fnt",
            "GJ_button_04.png"
        );
        auto keyOn = ButtonSprite::create(
            keyToString(keyCode).c_str(),
            size.width * scaleX,
            size.width,
            0.6f,
            true,
            "bigFont.fnt",
            "GJ_button_04.png"
        );
        keyOn->setColor(ccc3(127, 127, 127));

        return m_keys.createToggler(keyCode, keyOn, keyOff);
    }

    CCMenuItemToggler *addKey(CCMenu *row, int keyCode, float scaleX = 1.f) {
        auto button = createButton(keyCode, scaleX);
        row->addChild(button);
        return button;
    };

    CCMenu *makeRow() {
        auto row = CCMenu::create();
        row->setContentWidth(600);
        row->setLayout(
            RowLayout::create()
                ->setGap(1)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
        );
        return row;
    };

    bool init(std::function<void(int)> cb) {
        if (!Popup::init(440.f, 280.f))
            return false;
        setTitle("Mobile Set Key");
        m_callback = cb;
        auto keyboard = CCMenu::create();
        keyboard->setContentSize({600, 200});
        constexpr float gap = 3.f;
        constexpr float keyGap = gap;

        auto rows = CCNode::create();

        rows->setLayout(
            ColumnLayout::create()
                ->setGap(gap)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
                ->setAutoGrowAxis(1)
                ->setAxisReverse(true)
        );
        keyboard->setAnchorPoint({0.5, 0.5});
        keyboard->ignoreAnchorPointForPosition(false);

        auto row1 = makeRow();

        addKey(row1, 4096); // '
        addKey(row1, KEY_One);
        addKey(row1, KEY_Two);
        addKey(row1, KEY_Three);
        addKey(row1, KEY_Four);
        addKey(row1, KEY_Five);
        addKey(row1, KEY_Six);
        addKey(row1, KEY_Seven);
        addKey(row1, KEY_Eight);
        addKey(row1, KEY_Nine);
        addKey(row1, KEY_Zero);
        addKey(row1, KEY_OEMMinus);
        addKey(row1, KEY_OEMEqual);
        addKey(row1, KEY_Backspace, 2);
        rows->addChild(row1);

        auto row2 = makeRow();
        addKey(row2, KEY_Tab, 1.5f);
        addKey(row2, KEY_Q);
        addKey(row2, KEY_W);
        addKey(row2, KEY_E);
        addKey(row2, KEY_R);
        addKey(row2, KEY_T);
        addKey(row2, KEY_Y);
        addKey(row2, KEY_U);
        addKey(row2, KEY_I);
        addKey(row2, KEY_O);
        addKey(row2, KEY_P);
        addKey(row2, KEY_LeftBracket);
        addKey(row2, KEY_RightBracket);
        addKey(row2, KEY_Backslash);
        rows->addChild(row2);

        auto row3 = makeRow();
        addKey(row3, KEY_CapsLock, 1.75f);
        addKey(row3, KEY_A);
        addKey(row3, KEY_S);
        addKey(row3, KEY_D);
        addKey(row3, KEY_F);
        addKey(row3, KEY_G);
        addKey(row3, KEY_H);
        addKey(row3, KEY_J);
        addKey(row3, KEY_K);
        addKey(row3, KEY_L);
        addKey(row3, KEY_Semicolon);
        addKey(row3, KEY_Apostrophe);
        addKey(row3, KEY_Enter, 2.f);
        rows->addChild(row3);

        auto row4 = makeRow();
        addKey(row4, KEY_LeftShift, 2.25f);
        addKey(row4, KEY_Z);
        addKey(row4, KEY_X);
        addKey(row4, KEY_C);
        addKey(row4, KEY_V);
        addKey(row4, KEY_B);
        addKey(row4, KEY_N);
        addKey(row4, KEY_M);
        addKey(row4, KEY_OEMComma);
        addKey(row4, KEY_OEMPeriod);
        addKey(row4, KEY_Slash);
        addKey(row4, KEY_RightShift, 2.75f);
        rows->addChild(row4);

        auto row5 = makeRow();
        addKey(row5, KEY_LeftControl, 1.5f);
        addKey(row5, KEY_LeftWindowsKey, 1.5f);
        addKey(row5, 164, 1.5f);
        auto space = addKey(row5, KEY_Space, 8.f);
        addKey(row5, KEY_RightWindowsKey, 1.5f);
        addKey(row5, KEY_RightMenu, 1.5f);
        addKey(row5, KEY_RightControl, 1.5f);
        rows->addChild(row5);

        row1->updateLayout();
        row2->updateLayout();
        row3->updateLayout();
        row4->updateLayout();
        row5->updateLayout();

        auto discard = addKey(row5,-2,0);
        discard->setVisible(false);
        discard->setClickable(false);

        keyboard->addChild(rows);
        keyboard->setScale(0.6f);
        keyboard->updateLayout();
        rows->updateLayout();

        auto numericSelector = CCMenu::create();
        numericSelector->setLayout(ColumnLayout::create()->setAxisReverse(true)->setAutoScale(false)->setAutoGrowAxis(1));

        auto preview = CCLabelBMFont::create("None", "bigFont.fnt");
        preview->setOpacity(127);
        preview->setScale(0.8);

        auto CallBack = [this, preview](std::string out) {
            preview->setString(keyToString(numFromString<int>(out).unwrapOr(0)).c_str());
            m_keys.select(-2);
        };

        auto input = GoffyBuilder::NumericInput::create("KeyCode (Cocos)", 1, GoffyBuilder::ExtraInputWidget::Arrows, false, true, CallBack);
        m_input = input;
        numericSelector->addChild(input);
        numericSelector->addChild(preview);

        numericSelector->updateLayout();
        numericSelector->setScale(0.7);

        auto orText = CCLabelBMFont::create("Or","bigFont.fnt");
        orText->setScale(0.5);
        orText->setOpacity(127);

        auto rowThatAllGonnaBePlaced = CCMenu::create();
        rowThatAllGonnaBePlaced->setLayout(ColumnLayout::create()->setGap(5)->setAxisAlignment(AxisAlignment::Center)->setAutoGrowAxis(1)->setAutoScale(false)->setAxisReverse(true));

        rowThatAllGonnaBePlaced->addChild(keyboard);
        rowThatAllGonnaBePlaced->addChild(orText);
        rowThatAllGonnaBePlaced->addChild(numericSelector);

        rowThatAllGonnaBePlaced->updateLayout();
        m_mainLayer->addChildAtPosition(rowThatAllGonnaBePlaced, Anchor::Center);

        m_keys.setCallback([input](int id){
            if (id == -2)
                return ;
            input->setString("");
        });
        
        auto okBtn = GoffyBuilder::OkButton::create([this](){
            if (m_keys.getSelected() == -2){ // numeric input instead
                m_callback(static_cast<int>(m_input->getNumber()));
                onClose(nullptr);
                return ;
            }
            m_callback(m_keys.getSelected());
            onClose(nullptr);
            return ;
        });
        okBtn->setPosition({220, 0});
        okBtn->setAnchorPoint({0.5,0.5});
        m_buttonMenu->addChild(okBtn);
        return true;
    };

    void onClose(CCObject *sender) override {
        m_isAlive = false; // kbdispathcher doesnt wanna get killed for some reason
        this->removeFromParentAndCleanup(true);
    }

public:
    static setMobileKeyPopup *create(std::function<void(int)> cb) {
        auto ret = new setMobileKeyPopup;
        if (ret && ret->init(cb)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
    static void open(CCObject *, std::function<void(int)> cb) {
        auto layer = create(cb);
        layer->show();
    }
};
