#include "builderMacros.hpp"
#include "GUI/CCControlExtension/CCScale9Sprite.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "Geode/ui/Scrollbar.hpp"
#include "Geode/ui/SliderNode.hpp"
#include "Geode/ui/TextInput.hpp"
#include "Geode/utils/general.hpp"
#include <Geode/Result.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/FLAlertLayer.hpp>
#include <algorithm>
#include <cstdint>
#include <string>

using namespace geode::prelude;

// ? this is a goffy set of custom widgets that mimics the componets of RobTop
// triggers

namespace GoffyBuilder {


OkButton *OkButton::create(std::function<void()> callback) {
    auto ret = new OkButton();

    if (ret && ret->init(callback)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
};

void OkButton::onClick(CCObject *sender) {
    if (m_callback)
        m_callback();
};

bool OkButton::init(std::function<void()> callback) {
    if (!CCMenu::init())
        return false;
    m_callback = callback;

    auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create(
            "OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 30.0f, 1.0f
        ),
        this,
        menu_selector(OkButton::onClick)
    );
    this->setContentSize(okBtn->getScaledContentSize());
    this->setLayout(RowLayout::create());
    this->setTouchEnabled(true);
    this->addChild(okBtn);
    this->updateLayout();
    return true;
}

DropDownList *DropDownList::create(
    std::string labelName, float size, std::vector<std::string> itemList
) {
    auto ret = new DropDownList();

    if (ret && ret->init(labelName, size, itemList)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
};

CCMenu *DropDownList::dropList(float size) {
    // index: 3-text 2-selectedBg 1-bg
    auto wrapper = CCMenu::create();
    wrapper->setLayout(
        ColumnLayout::create()
            ->setAutoGrowAxis(30)
            ->setAutoScale(false)
            ->setAxisReverse(true)
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
    );

    uint8_t index = 0;
    float max_width;

    for (const std::string &item : m_itemList) {
        if (item == "_empty") { // ignore empty values
            index++;
            continue;
        }
        auto spr = CCLabelBMFont::create(item.c_str(), "bigFont.fnt");
        spr->setScale(size);
        if (m_curIndex == index) {
            spr->setFntFile("goldFont.fnt");
            spr->setScale(size + 0.15f);
        };
        max_width = std::max({max_width, spr->getScaledContentWidth()});
        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(DropDownList::onItemSelect)
        );
        btn->m_nZOrder = 3;
        spr->m_nZOrder = 3;

        btn->m_animationEnabled = false;

        btn->setTag(index++); // indentify the index of the item
        wrapper->addChild(btn);
    };

    wrapper->updateLayout();

    // auto wrapperBg = CCScale9Sprite::create("GJ_square06.png");
    // wrapperBg->setContentSize(wrapper->getScaledContentSize() * 1.3);
    // wrapperBg->setColor(ccc3(107, 56, 30));
    // wrapperBg->addChildAtPosition(wrapper, Anchor::Center);
    return wrapper;
};

CCScale9Sprite *DropDownList::DropDown(float size) {
    auto list = DropDownList::dropList(size);
    list->setAnchorPoint({0, 0.5});

    CCPoint contentSize = {
        list->getScaledContentWidth(),
        std::min({list->getScaledContentHeight(), 60.f})
    }; // magic number because i want

    auto wrapper = CCScale9Sprite::create("GJ_square06.png");
    wrapper->setContentSize(
        {contentSize.x + 32, contentSize.y + 12}
    ); // padding

    wrapper->setColor(ccc3(107, 56, 30));

    auto scrollArea = ScrollLayer::create(contentSize);
    scrollArea->m_contentLayer->setLayout(
        AxisLayout::create()
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
    );
    scrollArea->ignoreAnchorPointForPosition(false);
    scrollArea->setAnchorPoint({0, 0.5});

    auto scrollBar = Scrollbar::create(scrollArea);
    scrollBar->setContentSize({50, 300});
    scrollBar->setAnchorPoint({1.f, 0.5f});
    scrollBar->setScaleY(1.f);

    scrollArea->m_contentLayer->addChild(list);
    scrollArea->m_contentLayer->updateLayout();
    scrollArea->scrollToTop();

    wrapper->addChildAtPosition(scrollArea, Anchor::Left, {8, 0});
    wrapper->addChildAtPosition(scrollBar, Anchor::Right, {-8, 0});

    wrapper->updateLayout();
    return wrapper;
}

void DropDownList::onItemSelect(CCObject *sender) {
    int itemIndex = sender->getTag();
    if (m_listWidget) {
        m_listWidget->removeMeAndCleanup();
        m_listWidget = nullptr;
    }

    if (itemIndex < 0 || itemIndex >= m_itemList.size())
        return;

    m_curIndex = itemIndex;
    // ! Sorry if you are reading this because this is the WORST way to
    // reference, looks like roblox code // ? Not anymore
    if (m_label) {
        m_label->setString(m_itemList[itemIndex].c_str());

        auto wrapper = m_label->getParent();
        wrapper->updateLayout();

        auto bg = wrapper->getParent();
        bg->setContentSize(wrapper->getScaledContentSize() * 1.1f);
        bg->updateLayout();

        auto btn = bg->getParent();
        btn->updateLayout();

        this->updateLayout();
    }
    m_curArrow->setRotation(0);
    m_isOpen = false;
};

bool DropDownList::init(
    std::string labelName, float size, std::vector<std::string> itemList
) {
    if (!CCMenu::init())
        return false;
    m_itemList = itemList;
    this->setContentSize({200.f, 60.f});
    this->setLayout(
        ColumnLayout::create()
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(false)
            ->setGap(5)
            ->setAutoGrowAxis(1)
            ->setAxisReverse(false)
    );
    auto label = CCLabelBMFont::create(labelName.c_str(), "bigFont.fnt");
    label->setScale(size - 0.1);
    label->setZOrder(2);
    label->setAnchorPoint({0, 0});
    auto curArrow = CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
    curArrow->setScale(size);
    m_curArrow = curArrow;

    auto curLabel = CCLabelBMFont::create(itemList[0].c_str(), "bigFont.fnt");
    curLabel->setScale(size);
    curLabel->setZOrder(2);

    m_label = curLabel;
    auto curWrapper = CCMenu::create();
    curWrapper->setContentSize(
        {curLabel->getScaledContentWidth() + curArrow->getScaledContentWidth(),
         curLabel->getScaledContentHeight()}
    );
    curWrapper->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Between)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setAutoGrowAxis(100)
            ->setAutoScale(false)
            ->setAxisReverse(true)
    );

    curWrapper->addChild(curLabel);
    curWrapper->addChild(curArrow);
    curWrapper->updateLayout();
    auto curBg = CCScale9Sprite::create("square02b_small.png");
    curBg->setContentSize(curWrapper->getScaledContentSize() * 1.1);
    curBg->setColor(ccc3(105, 58, 35));
    curBg->setZOrder(1);
    curBg->addChildAtPosition(curWrapper, Anchor::Center);
    curBg->setAnchorPoint({0, 0});

    auto curBtn = CCMenuItemSpriteExtra::create(
        curBg, this, menu_selector(DropDownList::onClick)
    );
    curBtn->m_animationEnabled = false;
    curBtn->setLayout(
        RowLayout::create()->setAutoGrowAxis(100)->setAutoScale(false)
    );

    this->addChild(label);
    this->addChild(curBtn);
    this->updateLayout();
    return true;
};
void DropDownList::onClick(CCObject *sender) {
    if (m_itemList.size() <=
        1) // not even worth rendering a dropdown for 1 item
        return;

    geode::log::warn("DropDown!");
    if (!m_isOpen) {
        m_curArrow->setRotation(180);
        m_isOpen = true;
        auto list = DropDown(0.5);
        list->setAnchorPoint({0, 1});
        m_listWidget = list;
        this->addChildAtPosition(list, Anchor::BottomLeft);
    } else {
        m_curArrow->setRotation(0);
        m_isOpen = false;
        m_listWidget->removeMeAndCleanup();
    }
}
void DropDownList::setSelected(int index){
    m_curIndex = index;
    if (m_label) { // update the layout
        m_label->setString(m_itemList[index].c_str());

        auto wrapper = m_label->getParent();
        wrapper->updateLayout();

        auto bg = wrapper->getParent();
        bg->setContentSize(wrapper->getScaledContentSize() * 1.1f);
        bg->updateLayout();

        auto btn = bg->getParent();
        btn->updateLayout();

        this->updateLayout();
    }
    
};

ToggleOption *ToggleOption::create(
    std::string labelName, float size, ToggleOption *showOnCheck
) {
    auto ret = new ToggleOption();

    if (ret && ret->init(labelName, size, showOnCheck)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
};

bool ToggleOption::init(
    std::string labelName, float size, ToggleOption *showOnCheck
) {
    if (!CCMenu::init())
        return false;
    this->setContentSize({200.f, 100.f});
    this->setLayout(
        RowLayout::create()
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGap(5)
            ->setAutoGrowAxis(10.f)
    );

    auto uncheckSpr =
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
    uncheckSpr->setScale(0.75);

    if (!m_sprites.second) { // might be the WORSE way to prevent a leak
        auto checkSpr =
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        checkSpr->setScale(0.75);

        uncheckSpr->retain();
        checkSpr->retain();

        m_sprites = {uncheckSpr, checkSpr};
    }
    auto btn = CCMenuItemSpriteExtra::create(
        uncheckSpr, this, menu_selector(ToggleOption::onCheck)
    );
    m_checkBtn = btn;
    m_isChecked = false;
    m_onCheck = showOnCheck;

    auto label = CCLabelBMFont::create(labelName.c_str(), "bigFont.fnt");
    label->setScale(size);

    this->addChild(btn);
    this->addChild(label);

    this->updateLayout();

    return true;
};

void ToggleOption::onCheck(CCObject *sender) {
    if (m_isChecked) { // OnUncheck
        m_isChecked = false;
        m_checkBtn->setSprite(m_sprites.first);
        if (m_onCheck) {
            m_onCheck->setVisible(false);
            m_onCheck->setEnabled(false);
        };

    } else { // OnCheck
        m_isChecked = true;
        m_checkBtn->setSprite(m_sprites.second);
        if (m_onCheck) {
            m_onCheck->setVisible(true);
            m_onCheck->setEnabled(true);
        }
        if (m_unCheck) {
            m_unCheck->m_isChecked = false;
            m_unCheck->m_checkBtn->setSprite(m_sprites.first);
        };
    }

    return;
}

NumericInput *NumericInput::create(
    std::string labelName,
    float size,
    ExtraInputWidget ExtraInputWidget,
    bool hasGoldText = true,
    bool isVertical = true
) {

    auto ret = new NumericInput();

    if (ret &&
        ret->init(labelName, size, ExtraInputWidget, hasGoldText, isVertical)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
};

bool NumericInput::init(
    std::string labelName,
    float size,
    ExtraInputWidget ExtraInputWidget,
    bool hasGoldText = true,
    bool isVertical = true
) {
    if (!CCMenu::init())
        return false;
    this->setContentSize({200.f, 100.f});

    if (isVertical)
        this->setLayout(
            ColumnLayout::create()
                ->setAxisReverse(true)
                ->setCrossAxisLineAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
                ->setAutoGrowAxis(10.f)
        );
    else
        this->setLayout(
            RowLayout::create()
                ->setCrossAxisLineAlignment(AxisAlignment::Center)
                ->setAutoScale(false)
                ->setGap(5)
                ->setAutoGrowAxis(10.f)
        );

    auto label = CCLabelBMFont::create(
        labelName.c_str(), hasGoldText ? "goldFont.fnt" : "bigFont.fnt"
    );
    label->setScale(size);

    auto labelWrapper = CCMenu::create();
    labelWrapper->setLayout(
        RowLayout::create()
            ->setAutoScale(false)
            ->setGap(10.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAutoGrowAxis(1)
    );
    labelWrapper->setContentSize(label->getScaledContentSize());
    labelWrapper->addChild(label);

    auto inputWrapper = CCMenu::create();
    inputWrapper->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setAutoScale(false)
            ->setGap(10)
            ->setAutoGrowAxis(1)
    );
    inputWrapper->setContentWidth(140.f * size);

    auto numberInput = TextInput::create(70.f, "Num");
    m_input = numberInput;

    numberInput->setMaxCharCount(4);
    m_side = ExtraInputWidget;
    switch (ExtraInputWidget) {
        case ExtraInputWidget::Arrows: {
            log::warn("Arrow");
            numberInput->setCommonFilter(CommonFilter::Uint);
            auto leftSpr =
                CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");

            auto leftArrowBtn = CCMenuItemSpriteExtra::create(
                leftSpr, this, menu_selector(NumericInput::remove)
            );

            auto rightSpr =
                CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");

            auto rightArrowBtn = CCMenuItemSpriteExtra::create(
                rightSpr, this, menu_selector(NumericInput::add)
            );
            if (!isVertical) {
                numberInput->setScale(size + 0.2f);
                rightSpr->setScale(0.96f * size + 0.1f);
                leftSpr->setScale(0.96f * size + 0.1f);
                static_cast<RowLayout *>(inputWrapper->getLayout())
                    ->setGap(10 * size);
            }
            inputWrapper->addChild(leftArrowBtn);
            inputWrapper->addChild(numberInput);
            inputWrapper->addChild(rightArrowBtn);
            break;
        }
        case ExtraInputWidget::Slider:
        case ExtraInputWidget::U_Slider:
        case ExtraInputWidget::TimeSlider:
        case ExtraInputWidget::RangeSlider:
        case ExtraInputWidget::U_RangeSlider: {
            //  Slider = 2, -100 to 100 int
            //  U_Slider = 3, 0 to 100 int
            //  TimeSlider = 4, 0.00 to 10.00
            //  RangeSlider = 5,-1.00 to 1.00
            //  U_RangeSlider = 6, 0.00 to 1.00
            log::warn("Slider");

            auto slider = SliderNode::create(
                [this](SliderNode *idk, float value) {
                    NumericInput::onSliderChanged(idk, value);
                },
                false
            );
            slider->getBar()->setVisible(false);
            slider->setMax(1.f);
            slider->setContentSize({120.f, 8.f});
            slider->setAnchorPoint({0.f, 0.f});
            slider->setScale(size);

            m_slider = slider;
            float startPos;
            // idk how to make this better
            switch (ExtraInputWidget) {
                case ExtraInputWidget::Slider: {
                    startPos = 0.5f;
                    numberInput->setCommonFilter(CommonFilter::Int);
                    m_maxSlider = 100;
                    m_minSlider = -100;
                    break;
                }
                case ExtraInputWidget::U_Slider: {
                    startPos = 0.f;
                    numberInput->setCommonFilter(CommonFilter::Uint);
                    m_maxSlider = 100;
                    m_minSlider = 0;
                    break;
                }
                case ExtraInputWidget::TimeSlider: {
                    startPos = 0.05f;
                    numberInput->setFilter("0123456789."); // UFloat
                    m_maxSlider = 10;
                    m_minSlider = 0;
                    break;
                }
                case ExtraInputWidget::RangeSlider: {
                    startPos = 0.5f;
                    numberInput->setCommonFilter(CommonFilter::Float);
                    m_maxSlider = 1;
                    m_minSlider = -1;
                    break;
                }
                case ExtraInputWidget::U_RangeSlider: {
                    startPos = 0.f;
                    numberInput->setFilter("0123456789."); // UFloat
                    m_maxSlider = 1;
                    m_minSlider = 0;
                    break;
                }
                default:
                    break;
            };
            slider->setValue(startPos);
            numberInput->setMaxCharCount(10);

            if (isVertical) {
                labelWrapper->addChild(numberInput);
                static_cast<ColumnLayout *>(this->getLayout())->setGap(10.f);
                slider->setScale(0.8);
            } else
                inputWrapper->addChild(numberInput);

            inputWrapper->addChild(slider);
            numberInput->setScale(size + 0.2f);

            numberInput->setCallback([this](const std::string cur) {
                m_supressSlider = true;
                geode::log::warn("{}", cur);
                float pos = std::clamp(
                    numFromString<float>(cur).unwrapOr(0),
                    m_minSlider,
                    m_maxSlider
                );
                geode::log::warn("{}", cur);
                if (pos == m_maxSlider)
                    m_slider->setValue(1);
                else if (pos == m_minSlider)
                    m_slider->setValue(0);
                else {
                    float relativePos =
                        (pos - m_minSlider) / (m_maxSlider - m_minSlider);
                    m_slider->setValue(relativePos);
                };
                geode::log::warn("{}", cur);
                m_supressSlider = false;
            });
            break;
        }
        case ExtraInputWidget::None: {
            log::warn("None");
            inputWrapper->addChild(numberInput);
            // nothing happends, idk why i maded this case
            break;
        }
    };

    this->addChild(labelWrapper);
    this->addChild(inputWrapper);
    labelWrapper->updateLayout();
    inputWrapper->updateLayout();

    this->updateLayout();

    return true;
};
void NumericInput::onSliderChanged(SliderNode *sender, float value) {
    if (m_supressSlider)
        return;
    switch (m_side) {
        case GoffyBuilder::ExtraInputWidget::RangeSlider:
            m_input->setString(fmt::format("{:.2f}", -1.f + value * 2.f));
            break;

        case GoffyBuilder::ExtraInputWidget::U_RangeSlider:
            m_input->setString(fmt::format("{:.2f}", value));
            break;

        case GoffyBuilder::ExtraInputWidget::Slider:
            m_input->setString(
                numToString(static_cast<int>(-100.f + value * 200.f))
            );
            break;

        case GoffyBuilder::ExtraInputWidget::U_Slider:
            m_input->setString(numToString(static_cast<int>(value * 100.f)));
            break;

        case GoffyBuilder::ExtraInputWidget::TimeSlider:
            m_input->setString(fmt::format("{:.2f}", value * 10.f));
            break;
        default:
            break;
    }
};
void NumericInput::add(CCObject *sender) {
    if (!m_input)
        return;
    std::string cur = m_input->getString();
    if (cur == "")
        cur = "0";
    int newCur = getNumber() + 1;
    if (newCur > 9999)
        return;
    m_input->setString(std::to_string(newCur));
    return;
}
void NumericInput::remove(CCObject *sender) {
    if (!m_input)
        return;
    std::string cur = m_input->getString();
    if (cur == "")
        return;
    if (cur == "1") {
        m_input->setString((""));
        return;
    }

    int newCur = getNumber() - 1;
    m_input->setString(std::to_string(newCur));
    return;
}

}; // namespace GoffyBuilder