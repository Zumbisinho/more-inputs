#include "../../utils/keybindsAPI.hpp"
#include "../../utils/keybindsCache.hpp"
#include "../../utils/keycodeToString.hpp"
#include "../customLabels/keybindLabel.hpp"
#include "../keyEdit.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/cocos/menu_nodes/CCMenu.h"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "Geode/ui/Scrollbar.hpp"
#include "ccTypes.h"
#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/TextArea.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/TextInput.hpp>

#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;

// TODO: Add popup to edit key - fr bro i am whating dor that
// TODO : add the fucking popup key, and a callback for it - added, what do you
// want more? "a integration", **The big Zz**
namespace wrapperLabel {
inline CCMenu *
create(CCNode *target, float size, std::string text, AxisAlignment side) {
    // How hard is to resize a text :(
    auto container = CCMenu::create();
    container->setLayout(
        ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(side)
            ->setAxisReverse(true)
    );
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    float sidePoint = 0.f;
    if (side == AxisAlignment::End)
        sidePoint = 1.f;
    label->setAnchorPoint({sidePoint, 1.f});

    label->setScale(0.5f);

    label->setOpacity(180);
    if (size > 0.f)
        target->setScale(size);

    auto CSize = target->getScaledContentSize();
    auto LSize = label->getScaledContentSize();

    container->setContentSize(
        {std::max(CSize.width, LSize.width), CSize.height + LSize.height + 5.f}
    );

    container->addChild(label);
    container->addChild(target);
    container->updateLayout();
    container->setContentSize(
        {std::max(CSize.width, LSize.width), CSize.height + LSize.height + 5.f}
    );

    if (size > 0.f)
        target->setScale(size);
    label->setScale(0.5f);

    return container;
}
}; // namespace wrapperLabel
// TODO: make the UI works, the keybinds and addKey, after that, work on delete
// shit and edit localKBs

// TODO: make a callback for the editor, to edit name/key and delete

class addKeyGUI : public geode::Popup {
protected:
    CCLabelBMFont *m_keyDef;
    int m_keyCode;
    TextInput *m_textInput;
    std::function<void(std::pair<std::string, int>)> m_callback;

    bool init(std::function<void(std::pair<std::string, int>)> cb) {
        if (!Popup::init(360.f, 160.f))
            return false;
        this->setTitle("Add keybind");
        m_callback = cb;
        m_keyCode = -1;
        auto content = CCMenu::create();
        content->setContentSize({300.f, 140.f});
        content->setAnchorPoint({0.5f, 0.5f});

        content->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Between)
                ->setGap(15)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
        );
        content->setLayoutOptions(
            AnchorLayoutOptions::create()->setAnchor(Anchor::Center)
        );

        auto nameInput = TextInput::create(220.f, "Action Name", "bigFont.fnt");
        nameInput->setAnchorPoint({0.f, 0.f});
        m_textInput = nameInput;

        auto keyDefaultSpr = CCLabelBMFont::create("None", "bigFont.fnt");
        keyDefaultSpr->setAnchorPoint({0.5f, 0.4f});
        m_keyDef = keyDefaultSpr;
        auto keyDefault =
            EditorButtonSprite::create(keyDefaultSpr, EditorBaseColor::Gray);
        keyDefault->setAnchorPoint({0.5f, 0.5f});

        auto keyDefBtn = CCMenuItemSpriteExtra::create(
            keyDefault, this, menu_selector(addKeyGUI::onKeyDef)
        );

        auto nameInputWrapped = wrapperLabel::create(
            nameInput, 1.f, "Action Name", AxisAlignment::Start
        );
        nameInputWrapped->setAnchorPoint({0.f, 0.5f});

        auto separator = CCLabelBMFont::create(":", "bigFont.fnt");
        separator->setScale(1.f);
        separator->setAnchorPoint({0.5f, 0.7f});

        auto okBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 30.0f, 1.0f
            ),
            this,
            menu_selector(addKeyGUI::onConfirm)
        );
        okBtn->setPosition({180, 30});
        m_buttonMenu->addChild(okBtn);

        m_mainLayer->addChildAtPosition(content, Anchor::Center);
        content->addChild(nameInputWrapped);
        content->addChild(separator);
        content->addChild(keyDefBtn);

        content->updateLayout();
        m_mainLayer->updateLayout();

        separator->setScale(1.f);
        keyDefault->setScale(1.25f);

        return true;
    }
    void onKeyDef(CCObject *sender) {
        keyEdit::setKeyPopup::open(sender, [this](int keyCode) {
            std::string toDisplay;
            if (keyCode != -2) // skip if pressed "esc"
                toDisplay = keyToString(keyCode);

            m_keyDef->setString(toDisplay.c_str());
            m_keyCode = keyCode;
            m_keyDef->updateLabel();
        });
    }
    void onConfirm(CCObject *sender) {
        std::pair<std::string, int> toReturn;
        int kC = m_keyCode;
        std::string actionName = m_textInput->getString();
        toReturn = {actionName, kC};

        if (m_callback)
            m_callback(toReturn);

        this->onClose(nullptr);
    }

public:
    static addKeyGUI *
    create(std::function<void(std::pair<std::string, int>)> cb) {
        auto ret = new addKeyGUI;
        if (ret->init(cb)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
    static void
    open(CCObject *, std::function<void(std::pair<std::string, int>)> cb) {
        auto layer = create(cb);
        layer->show();
    }
};

class editKeyGUI : public geode::Popup {
private:
    std::function<void(std::string, int)> m_callback;
    std::function<void()> m_afterActionCB;
    CCLabelBMFont *m_keyDef;
    int m_keyCode;
    TextInput *m_textInput;
    std::string m_oldActionName;

public:
    static editKeyGUI *create(
        std::function<void(std::string, int)> cb,
        std::string oldActionName,
        int oldKeyCode,
        std::function<void()> afterActionCB
    ) {
        auto ret = new editKeyGUI;
        if (ret && ret->init(cb, oldActionName, oldKeyCode, afterActionCB)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(
        CCObject *,
        std::function<void(std::string, int)> cb,
        std::string oldActionName,
        int oldKeyCode,
        std::function<void()> afterActionCB
    ) {
        auto layer = create(cb, oldActionName, oldKeyCode, afterActionCB);
        layer->show();
    }

private:
    bool init(
        std::function<void(std::string, int)> cb,
        std::string oldActionName,
        int oldKeyCode,
        std::function<void()> afterActionCB
    ) {
        if (!Popup::init(360.f, 160.f))
            return false;
        m_keyCode = oldKeyCode;
        m_oldActionName = oldActionName;
        m_callback = cb;
        m_afterActionCB = afterActionCB;
        this->setTitle("Edit " + oldActionName + " Action");
        auto content = CCMenu::create();
        content->setContentSize({300.f, 140.f});
        content->setAnchorPoint({0.5f, 0.5f});

        content->setLayout(
            RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Between)
                ->setGap(15)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
        );
        content->setLayoutOptions(
            AnchorLayoutOptions::create()->setAnchor(Anchor::Center)
        );

        auto nameInput = TextInput::create(220.f, "Action Name", "bigFont.fnt");
        nameInput->setAnchorPoint({0.f, 0.f});
        nameInput->setString(oldActionName);
        m_textInput = nameInput;

        auto keyDefaultSpr = CCLabelBMFont::create(
            keyToString(oldKeyCode).c_str(), "bigFont.fnt"
        );
        keyDefaultSpr->setAnchorPoint({0.5f, 0.4f});
        m_keyDef = keyDefaultSpr;
        auto keyDefault =
            EditorButtonSprite::create(keyDefaultSpr, EditorBaseColor::Gray);
        keyDefault->setAnchorPoint({0.5f, 0.5f});

        auto keyDefBtn = CCMenuItemSpriteExtra::create(
            keyDefault, this, menu_selector(editKeyGUI::onKeyDef)
        );

        auto nameInputWrapped = wrapperLabel::create(
            nameInput, 1.f, "Action Name", AxisAlignment::Start
        );
        nameInputWrapped->setAnchorPoint({0.f, 0.5f});

        auto separator = CCLabelBMFont::create(":", "bigFont.fnt");
        separator->setScale(1.f);
        separator->setAnchorPoint({0.5f, 0.7f});

        auto okBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create(
                "OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 30.0f, 1.0f
            ),
            this,
            menu_selector(editKeyGUI::onConfirm)
        );
        okBtn->setPosition({180, 30});
        m_buttonMenu->addChild(okBtn);

        m_mainLayer->addChildAtPosition(content, Anchor::Center);
        content->addChild(nameInputWrapped);
        content->addChild(separator);
        content->addChild(keyDefBtn);

        content->updateLayout();
        m_mainLayer->updateLayout();

        separator->setScale(1.f);
        keyDefault->setScale(1.25f);

        auto deleteBtnSpr =
            CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        deleteBtnSpr->setScale(1.25f);
        auto deleteBtn = CCMenuItemSpriteExtra::create(
            deleteBtnSpr, this, menu_selector(editKeyGUI::onDelete)
        );

        deleteBtn->setAnchorPoint({0.5, 0.5});
        auto deleteSize = deleteBtn->getScaledContentSize();
        deleteBtn->setPosition({deleteSize.width / 4, deleteSize.height / 4});
        m_buttonMenu->addChild(deleteBtn);

        return true;
    }
    void onKeyDef(CCObject *sender) {
        keyEdit::setKeyPopup::open(sender, [this](int keyCode) {
            std::string toDisplay;
            if (keyCode != -2) // skip if pressed "esc"
                toDisplay = keyToString(keyCode);

            m_keyDef->setString(toDisplay.c_str());
            m_keyCode = keyCode;
            m_keyDef->updateLabel();
        });
    }
    void onConfirm(CCObject *sender) {
        if (m_keyCode == -2) // ESC
            return;

        std::string actionName = m_textInput->getString();

        if (actionName.empty())
            return;

        if (m_callback)
            m_callback(actionName, m_keyCode);
        if (m_afterActionCB)
            m_afterActionCB();
        this->onClose(nullptr);
    }
    void onDelete(CCObject *sender) {
        std::string message = fmt::format(
            "You wanna delete action {}({})?",
            m_oldActionName,
            keyToString(m_keyCode)
        );
        createQuickPopup(
            "Are you sure?",
            message,
            "No",
            "Yes",
            [this](auto popup, bool btn2) {
                if (btn2) {
                    keybindsAPI::editLevelKeyBind(
                        LevelEditorLayer::get(),
                        m_oldActionName,
                        {m_oldActionName, -67},
                        false
                    ); // worst way to do it but its how i do it
                    KeybindCache::reset();
                    if (m_afterActionCB)
                        m_afterActionCB();
                };
                this->onClose(nullptr);
            }
        );
    }
};

class setupKeyBindsGUI : public geode::Popup {
private:
    CCNode *m_contentLayer;
    CCSize m_contentSize;
    ScrollLayer *m_scrollArea;
    Scrollbar *m_scrollBar;
    // Create a popup that is like the add popup, but can edit // name/key, with
    // the same actionID, and delete - delete just set // the key and actionName
    // to _empty = -67 //! Very bad aprouch but i am lazy to rewrite all
    std::function<void(CCObject *, std::string, int, CCLabelBMFont *)> m_editCB;
    TextArea *m_noKeysTip;

public:
    static setupKeyBindsGUI *
    create(std::vector<std::pair<std::string, int>> const &keybinds) {
        auto ret = new setupKeyBindsGUI;
        if (ret && ret->init(keybinds)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void
    open(CCObject *, std::vector<std::pair<std::string, int>> keyBindsDict) {
        auto layer = create(keyBindsDict);
        layer->show();
    }

private:
    void resetList() {
        if (!KeybindCache::initialized)
            KeybindCache::init(LevelEditorLayer::get());
        auto keybinds = KeybindCache::keybindsAndAction;
        m_contentLayer->removeAllChildrenWithCleanup(true);
        bool keyTipDeleted = false;
        m_noKeysTip->setVisible(true);
        for (const auto &[key, def] : keybinds) {
            if (def == -67) // empty value
                continue;
            if (!keyTipDeleted) {
                m_noKeysTip->setVisible(false);
                keyTipDeleted = true;
            };
            auto Label = KeyBindsSection::create(
                key, def, {m_contentSize.width, 20.f}, m_editCB
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
    bool init(std::vector<std::pair<std::string, int>> const &keyBindsDict) {
        if (!Popup::init(440.f, 280.f))
            return false;

        this->setTitle("Setup level Keybinds");
        const std::function<void(CCObject *, std::string, int, CCLabelBMFont *)>
            Editcallback = [this, keyBindsDict](
                               CCObject *sender,
                               std::string actionName,
                               int oldKeyCode,
                               CCLabelBMFont *keyBtn
                           ) {
                const std::function<void(std::string, int)> onEdit =
                    [this,
                     actionName,
                     keyBindsDict](std::string newActionName, int newKeyCode) {
                        // Needs logic here
                        std::pair<std::string, int> newPair = {
                            newActionName, newKeyCode
                        };
                        keybindsAPI::editLevelKeyBind(
                            LevelEditorLayer::get(), actionName, newPair, false
                        );
                        KeybindCache::reset();
                    };

                editKeyGUI::open(
                    sender,
                    onEdit,
                    actionName,
                    oldKeyCode,
                    [this, keyBindsDict]() { this->resetList(); }
                );
            };
        m_editCB = Editcallback;

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
        m_scrollBar = scrollBar;

        auto content = CCNode::create();
        content->setLayout(
            ColumnLayout::create()
                ->setGap(5)
                ->setAutoGrowAxis(contentSize.height)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAxisReverse(true)
        );
        m_contentLayer = content;
        scrollArea->m_contentLayer->addChild(content);

        m_mainLayer->addChildAtPosition(
            scrollBar, geode::Anchor::Right, {-27.f, 0.f}
        );
        bg->addChildAtPosition(listLabel, geode::Anchor::Center);
        listLabel->addChildAtPosition(scrollArea, geode::Anchor::TopLeft);
        m_mainLayer->addChildAtPosition(bg, geode::Anchor::Center);

        auto noKeysTip = TextArea::create(
            "You dont have any <cy>Keybinds</c> yet!\n To add click on "
            "the<cg> plus button</c> to add one!",
            "bigFont.fnt",
            1.f,
            360.f,
            {0.5f, 0.5f},
            28.f,
            false
        );
        noKeysTip->setOpacity(180);
        noKeysTip->setScale(0.7f);
        m_noKeysTip = noKeysTip;
        scrollArea->addChildAtPosition(noKeysTip, Anchor::Center);

        if (!keyBindsDict.empty()) {
            m_noKeysTip->setVisible(false);
            for (const auto &[key, def] : keyBindsDict) {
                auto Label = KeyBindsSection::create(
                    key, def, {contentSize.width, 20.f}, m_editCB
                );
                content->addChild(Label);
            }
        }
        content->updateLayout();
        scrollArea->m_contentLayer->updateLayout();
        scrollArea->m_contentLayer->setContentHeight(
            content->getContentHeight()
        );
        scrollArea->scrollToTop();

        auto windowSize = m_buttonMenu->getContentSize();

        auto addBtnSpr =
            CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        addBtnSpr->setScale(1.1f);
        auto addBtn = CCMenuItemSpriteExtra::create(
            addBtnSpr, this, menu_selector(setupKeyBindsGUI::onAdd)
        );
        addBtn->setAnchorPoint({0.5, 0.5});
        addBtn->setPosition(
            {windowSize.width - addBtn->getContentWidth() / 4,
             addBtn->getContentHeight() / 4}
        );

        auto deleteBtnSpr =
            CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        deleteBtnSpr->setScale(1.25f);
        auto deleteBtn = CCMenuItemSpriteExtra::create(
            deleteBtnSpr, this, menu_selector(setupKeyBindsGUI::onDelete)
        );

        deleteBtn->setAnchorPoint({0.5, 0.5});
        auto deleteSize = deleteBtn->getScaledContentSize();
        deleteBtn->setPosition({deleteSize.width / 4, deleteSize.height / 4});

        m_buttonMenu->addChild(addBtn);
        m_buttonMenu->addChild(deleteBtn);

        return true;
    };

    void onAdd(CCObject *sender) {
        addKeyGUI::open(
            sender, [this](std::pair<std::string, int> actionKeyCode) {
                auto actionName = actionKeyCode.first;
                auto actionKey = actionKeyCode.second;
                if (actionName.empty() || actionKey == -2)
                    return;

                keybindsAPI::addLevelKeyBind(
                    LevelEditorLayer::get(), actionName, actionKey
                );
                KeybindCache::reset();
                this->resetList();
            }
        );
    }

    void onDelete(CCObject *sender) {
        createQuickPopup(
            "Are you sure?",
            "You are not able to roll-back this action. This will <cr>Delete "
            "all your Keybinds</c> on this level.",
            "No",
            "Yes",
            [this](auto popup, bool btn2) {
                if (btn2) {
                    keybindsAPI::deleteKeybindsFromLevel(
                        LevelEditorLayer::get()
                    );
                    KeybindCache::reset();
                    resetList();
                };
                this->onClose(nullptr);
            }
        );
    }
};