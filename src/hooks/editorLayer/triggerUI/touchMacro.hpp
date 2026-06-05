#pragma once
#include "../../../utils/keybindsCache.hpp"
#include "../../../utils/macroClasses.hpp"
#include "../../textGameObject.hpp"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include "builderMacros.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/InfoAlertButton.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <vector>


using namespace geode::prelude;

class TouchMacroUI : public Popup {
public:
    static TouchMacroUI *create(macroTriggers::touchMacro *touchMacroCluster) {
        auto ret = new TouchMacroUI;
        if (ret && ret->init(touchMacroCluster)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(macroTriggers::touchMacro *touchMacroCluster) {
        auto layer = create(touchMacroCluster);
        layer->m_noElasticity = true;
        layer->show();
    }

protected:
    macroTriggers::touchMacro *m_macroCluster;
    std::vector<CCObject *> m_widgets;

private:
    bool init(macroTriggers::touchMacro *touchMacroCluster) {
        geode::log::warn("{}", touchMacroCluster->macroObj->m_text);
        if (!Popup::init(440.f, 280.f))
            return false;
        this->setTitle("Edit Touch Macro");
        m_closeBtn->setEnabled(
            false
        ); // remove the close btn to replace with info one
        m_closeBtn->setVisible(false);
        m_macroCluster = touchMacroCluster;

        auto infoBtn = InfoAlertButton::create(
            "Help",

            "Listens for a <cy>player action</c> and activates groups when "
            "that action is detected. Use the <cj>Action</c> selector to "
            "choose which input event this trigger will monitor."

            "\n<cs>Press ID:</c> the group that will be activated "
            "when the selected action key is <cg>pressed</c>."

            "\n<cs>Release ID:</c> the group that will be activated "
            "when the selected action key is <cr>released</c>."

            "\n<cj>Action:</c> determines which player input this trigger will "
            "listen for."

            "\n<co>Disarm On First Key<:/c> causes the trigger to deactivate "
            "itself after the first valid input is detected.",
            1
        );
        infoBtn->setAnchorPoint({0, 1});

        auto groupRow = CCMenu::create();
        groupRow->setLayout(
            RowLayout::create()
                ->setAutoGrowAxis(1)
                ->setGap(15)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
        );
        groupRow->setAnchorPoint({0.5, 0.5});

        auto pressNI = GoffyBuilder::NumericInput::create(
            "Press ID", 0.7, GoffyBuilder::ExtraInputWidget::Arrows, true, true
        );

        auto releaseNI = GoffyBuilder::NumericInput::create(
            "Release ID",
            0.7,
            GoffyBuilder::ExtraInputWidget::Arrows,
            true,
            true
        );

        groupRow->addChild(pressNI);
        groupRow->addChild(releaseNI);
        groupRow->updateLayout();

        auto leftBottomGroup = CCMenu::create();
        leftBottomGroup->setLayout(
            RowLayout::create()
                ->setAutoGrowAxis(1)
                ->setGap(10)
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
        );
        auto FCleftBottomGroup = CCMenu::create(); // First Column
        FCleftBottomGroup->setLayout(
            ColumnLayout::create()
                ->setAutoGrowAxis(1)
                ->setGap(10)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false)
                ->setAxisReverse(false)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
        );
        auto SCleftBottomGroup = CCMenu::create(); // Second Column
        SCleftBottomGroup->setLayout(
            ColumnLayout::create()
                ->setAutoGrowAxis(1)
                ->setGap(10)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setAutoScale(false)
                ->setAxisReverse(false)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
        );

        leftBottomGroup->addChild(FCleftBottomGroup);
        leftBottomGroup->addChild(SCleftBottomGroup);
        leftBottomGroup->setAnchorPoint({0, 0});

        auto multiTriggered = GoffyBuilder::ToggleOption::create(
            "Multi\nTriggered", 0.35, nullptr
        );
        auto touchTriggered = GoffyBuilder::ToggleOption::create(
            "Touch\nTrigger", 0.35, multiTriggered
        );
        auto spawnTriggered = GoffyBuilder::ToggleOption::create(
            "Spawn\nTrigger", 0.35, multiTriggered
        );
        auto disarmOnFirst = GoffyBuilder::ToggleOption::create(
            "Disarm on\nfirst input", 0.35, nullptr
        );

        touchTriggered->m_unCheck = spawnTriggered;
        spawnTriggered->m_unCheck = touchTriggered;

        FCleftBottomGroup->addChild(touchTriggered);
        FCleftBottomGroup->addChild(spawnTriggered);
        SCleftBottomGroup->addChild(multiTriggered);
        SCleftBottomGroup->addChild(disarmOnFirst);

        FCleftBottomGroup->updateLayout();
        SCleftBottomGroup->updateLayout();
        leftBottomGroup->updateLayout();

        multiTriggered->setVisible(false);
        if (!KeybindCache::initialized)
            KeybindCache::init(LevelEditorLayer::get());

        auto keybinds = KeybindCache::keybindsAndAction;
        std::vector<std::string> actionList;

        if (keybinds.empty()) {
            actionList.push_back("Not found any keybinds!");
        } else {
            for (std::pair<std::string, int> action : keybinds) {
                actionList.push_back(action.first);
            };
        }

        auto actionDropDown =
            GoffyBuilder::DropDownList::create("Action:", 0.5, actionList);
        actionDropDown->setAnchorPoint({0, 1});
        actionDropDown->ignoreAnchorPointForPosition(false);

        auto okBtn = GoffyBuilder::OkButton::create([this]() {
            // get all inputs and update both counters & custom trigger //! nah
            // just hook the onclose
            this->onClose(nullptr);
        });
        okBtn->setPosition({220, 24});

        m_mainLayer->addChildAtPosition(groupRow, Anchor::Center);
        m_mainLayer->addChildAtPosition(
            leftBottomGroup, Anchor::BottomLeft, {12, 12}
        );
        m_mainLayer->addChildAtPosition(
            actionDropDown, Anchor::TopLeft, {18, -30}
        );

        m_buttonMenu->addChild(okBtn);
        m_buttonMenu->addChildAtPosition(infoBtn, Anchor::TopLeft, {6, -6});

        m_widgets.insert(
            m_widgets.end(),
            {actionDropDown,
             pressNI,
             releaseNI,
             touchTriggered,
             spawnTriggered,
             multiTriggered,
             disarmOnFirst}
        );

        // ? Loading params from string
        auto config = macroTriggers::triggerStrToConfig(
            touchMacroCluster->macroObj->m_text
        );
        if (!config.empty()) {
            actionDropDown->setSelected(config[0]);
            pressNI->setString(std::to_string(config[1]));
            releaseNI->setString(std::to_string(config[2]));

            touchTriggered->setCheck(config[3]);
            spawnTriggered->setCheck(config[4]);
            multiTriggered->setCheck(config[5]);
            if (config[3] == 1 || config[4] == 1) {
                multiTriggered->setVisible(
                    true
                ); // show if any of the things blah blah blah
            }
            disarmOnFirst->setCheck(config[6]);
        }

        return true;
    };
    void onClose(CCObject *sender) override {
        Popup::onClose(sender);
        auto DropDown = static_cast<GoffyBuilder::DropDownList *>(m_widgets[0]);
        auto action = DropDown->m_curIndex;

        int pressGroupId =
            static_cast<GoffyBuilder::NumericInput *>(m_widgets[1])
                ->getNumber();
        int releaseGroupId =
            static_cast<GoffyBuilder::NumericInput *>(m_widgets[2])
                ->getNumber();

        auto touch = static_cast<GoffyBuilder::ToggleOption *>(m_widgets[3])
                         ->m_isChecked;
        auto spawn = static_cast<GoffyBuilder::ToggleOption *>(m_widgets[4])
                         ->m_isChecked;
        auto multi = static_cast<GoffyBuilder::ToggleOption *>(m_widgets[5])
                         ->m_isChecked;
        auto disarmOnFirst =
            static_cast<GoffyBuilder::ToggleOption *>(m_widgets[6])
                ->m_isChecked;

        m_macroCluster->macroObj->m_text = macroTriggers::configToTriggerStr(
            action,
            pressGroupId,
            releaseGroupId,
            touch,
            spawn,
            multi,
            disarmOnFirst
        );
        auto actionName =
            static_cast<GoffyBuilder::DropDownList *>(m_widgets[0])
                ->m_itemList[action];
        // change the triggers
        int actionId = KeybindCache::actionNameToID[actionName];

        auto press = m_macroCluster->pressObj;
        auto release = m_macroCluster->releaseObj;
        // ? Action
        press->m_itemID = actionId;
        release->m_itemID = actionId;
        // ? group
        press->m_targetGroupID = pressGroupId;
        release->m_targetGroupID = releaseGroupId;

        // ! all thoses variations
        press->m_isTouchTriggered = touch;
        press->m_isSpawnTriggered = spawn;
        press->m_isMultiTriggered = multi;

        release->m_isTouchTriggered = touch;
        release->m_isSpawnTriggered = spawn;
        release->m_isMultiTriggered = multi;

        // ? Disarm
        press->m_multiActivate = !disarmOnFirst;
        release->m_multiActivate = !disarmOnFirst;

        // ? Update groupLabel
        auto groupWidget = static_cast<TouchMacroGameObject *>(m_macroCluster->macroObj)->m_fields->m_groupLabel;
        std::string groupLabel = fmt::format(
            "{}/{}",
            m_macroCluster->getPressGroup(),
            m_macroCluster->getReleaseGroup()
        ); // 6/7
        float oldWidth = groupWidget->getScaledContentWidth();
        groupWidget->setString(groupLabel.c_str());
        float newWidth = groupWidget->getScaledContentWidth(); 

        float fixedScale = (oldWidth / newWidth) * groupWidget->getScale();

        groupWidget->setScale(fixedScale);
    }
};