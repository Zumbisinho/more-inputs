#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "customLabels/keybindLabel.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/KeyframeObject.hpp>
#include <string>

using namespace geode::prelude;

// TODO: Add popup to edit key

class KeyBindsLocalConfigGui : public geode::Popup {
public:
    static KeyBindsLocalConfigGui *
    create(std::vector<keybindsAPI::KeyFullSettings> const keybinds) {
        auto ret = new KeyBindsLocalConfigGui;
        if (ret && ret->init(keybinds)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(CCObject *, std::vector<keybindsAPI::KeyFullSettings> keyBindsDict) {
        auto layer = create(keyBindsDict);
        layer->show();
    }

protected:
    CCNode *m_contentLayer;
    ScrollLayer *m_scrollArea;
    CCSize m_contentSize;
    std::function<void(CCObject *, const keybindsAPI::KeyFullSettings)> m_editCB = [this](CCObject *sender, const keybindsAPI::KeyFullSettings newKey) {
        keyEdit::setKeyPopup::open(
            sender, [this, newKey](int keyCode) {
                if (keyCode == -2)
                    return;

                KeybindCache::changeLocalKey(newKey, keyCode);
                resetList();
            }
        );
    };

private:
    void resetList() {
        if (!KeybindCache::initialized)
            KeybindCache::init(LevelEditorLayer::get());

        auto keybinds = KeybindCache::keySettings;
        m_contentLayer->removeAllChildrenWithCleanup(true);
        if (keybinds.size() != 0)
            geode::log::warn("Size {}\nItem {}: {} {}", keybinds.size(), keybinds[0].first, keybinds[0].second.name, keybinds[0].second.keyCode);

        for (const auto &key : keybinds) {
            auto Label = KeyBindsSection::create(
                &key, {m_contentSize.width, 20.f}, m_editCB
            );
            m_contentLayer->addChild(Label);
        };
        m_contentLayer->updateLayout();
        m_scrollArea->m_contentLayer->updateLayout();
        m_scrollArea->m_contentLayer->setContentHeight(
            m_contentLayer->getContentHeight()
        );
        m_scrollArea->scrollToTop();
    }
    bool init(const std::vector<keybindsAPI::KeyFullSettings> keyBindsDict) {
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
        m_contentSize = contentSize;

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
                ->setAxisReverse(true)
        );
        m_scrollArea = scrollArea;

        auto scrollBar = Scrollbar::create(scrollArea);
        scrollBar->setContentSize({50, 300});
        scrollBar->setAnchorPoint({1.f, 0.5f});
        scrollBar->setScaleY(1.f);

        auto content = CCNode::create();
        content->setLayout(ColumnLayout::create()->setGap(5)->setAutoGrowAxis(contentSize.height)->setAxisAlignment(AxisAlignment::End)->setAxisReverse(true));
        scrollArea->m_contentLayer->addChild(content);
        m_contentLayer = content;
        m_mainLayer->addChildAtPosition(scrollBar, geode::Anchor::Right, {-27.f, 0.f});
        bg->addChildAtPosition(listLabel, geode::Anchor::Center);
        listLabel->addChildAtPosition(scrollArea, geode::Anchor::TopLeft);
        m_mainLayer->addChildAtPosition(bg, geode::Anchor::Center);

        for (const keybindsAPI::KeyFullSettings &item : keyBindsDict) {
            auto label = KeyBindsSection::create(
                &item, {contentSize.width, 20.f}, m_editCB
            );
            content->addChild(label);
        }
        content->updateLayout();
        scrollArea->m_contentLayer->updateLayout();
        scrollArea->m_contentLayer->setContentHeight(
            content->getContentHeight()
        );
        scrollArea->scrollToTop();

        return true;
    };
};