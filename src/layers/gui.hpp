#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "customLabels/keybindLabel.hpp"
#include <Geode/Geode.hpp>
#include <string>

using namespace geode::prelude;

// TODO: Add popup to edit key

class KeyBindsLocalConfigGui : public geode::Popup {
public:
    static KeyBindsLocalConfigGui *
    create(std::vector<std::pair<std::string, int>> const keybinds) {
        auto ret = new KeyBindsLocalConfigGui;
        if (ret && ret->init(keybinds)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(CCObject *,
                     std::vector<std::pair<std::string, int>> keyBindsDict) {
        auto layer = create(keyBindsDict);
        layer->show();
    }

private:
    bool init(std::vector<std::pair<std::string, int>> const keyBindsDict) {
        if (!Popup::init(440.f, 280.f))
            return false;

        auto playLayer = PlayLayer::get();
        if (playLayer && playLayer->m_level) {
            std::string levelName = playLayer->m_level->m_levelName;
            this->setTitle(levelName + " Keybinds");
        } else
            return false; // How do you even open the menu without playing the
                          // level?

        auto listLabel = CCScale9Sprite::create("square02b_001.png");
        listLabel->setColor(ccc3(145, 80, 48));
        listLabel->setContentSize({360.f, 200.f});
        listLabel->setZOrder(2);

        auto contentSize = listLabel->getContentSize();

        auto bg = CCScale9Sprite::create("square02b_001.png");
        bg->setContentSize({362.5f, 202.5f});
        bg->setColor(ccc3(129, 72, 43));
        bg->setZOrder(1);

        auto scrollArea = ScrollLayer::create(contentSize, true, true);
        scrollArea->setAnchorPoint({0.f, 1.f});
        scrollArea->ignoreAnchorPointForPosition(false);
        scrollArea->m_contentLayer->setLayout(
            AxisLayout::create()
                ->setAxisAlignment(AxisAlignment::Center)
                ->setAxisReverse(true));

        auto scrollBar = Scrollbar::create(scrollArea);
        scrollBar->setContentSize({50, 300});
        scrollBar->setAnchorPoint({1.f, 0.5f});
        scrollBar->setScaleY(1.f);

        auto content = CCNode::create();
        content->setLayout(ColumnLayout::create()
                               ->setGap(5)
                               ->setAutoGrowAxis(contentSize.height)
                               ->setAxisAlignment(AxisAlignment::End)
                               ->setAxisReverse(true));
        scrollArea->m_contentLayer->addChild(content);

        m_mainLayer->addChildAtPosition(scrollBar, geode::Anchor::Right,
                                        {-27.f, 0.f});
        bg->addChildAtPosition(listLabel, geode::Anchor::Center);
        listLabel->addChildAtPosition(scrollArea, geode::Anchor::TopLeft);
        m_mainLayer->addChildAtPosition(bg, geode::Anchor::Center);

        for (const auto &[key, def] : keyBindsDict) {
            if (def == -67) // empty spik
                continue;
            auto label = KeyBindsSection::create(
                key, def, {contentSize.width, 20.f},
                [this](CCObject *sender, std::string keyName, int keyCode,CCLabelBMFont * defKeyLabel) {
                    keyEdit::setKeyPopup::open(
                        sender, [this, keyName, defKeyLabel](int keyCode) {
                            if (keyCode == -2)
                                return;

                            KeybindCache::changeLocalKey(keyName, keyCode);

                            if (defKeyLabel) {
                                defKeyLabel->setString(
                                    keyToString(keyCode).c_str());
                            }
                        });
                });
            content->addChild(label);
        }
        content->updateLayout();
        scrollArea->m_contentLayer->updateLayout();
        scrollArea->m_contentLayer->setContentHeight(
            content->getContentHeight());
        scrollArea->scrollToTop();

        return true;
    };
};