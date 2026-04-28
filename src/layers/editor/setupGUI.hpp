#include <Geode/Geode.hpp>
#include "../customLabels/keybindLabel.hpp"
#include "Geode/cocos/base_nodes/CCNode.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <string>
#include "../../utils/keycodeToString.hpp"
#include "../keyEdit.hpp"
#include "Geode/ui/ScrollLayer.hpp"
#include "Geode/ui/Scrollbar.hpp"
using namespace geode::prelude;

// TODO: Add popup to edit key - fr bro i am whating dor that
// TODO : add the fucking popup key, and a callback for it - added, what do you want more? "a integration", **The big Zz**

class addKeyGUI : public geode::Popup
{
protected:
    CCLabelBMFont* m_keyDef;
    int m_keyCode;
    TextInput* m_textInput;
    std::function<void(std::pair<std::string, int>)> m_callback;
    CCMenu *wrapperWithLabel(CCNode *target, float size, std::string text, AxisAlignment side)
    {
        // How hard is to resize a text :(
        auto container = CCMenu::create();
        container->setLayout(ColumnLayout::create()
                                 ->setAxisAlignment(AxisAlignment::Center)
                                 ->setCrossAxisLineAlignment(side)
                                 ->setAxisReverse(true));
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

        container->setContentSize({std::max(CSize.width, LSize.width), CSize.height + LSize.height + 5.f});

        container->addChild(label);
        container->addChild(target);
        container->updateLayout();
        container->setContentSize({std::max(CSize.width, LSize.width), CSize.height + LSize.height + 5.f});

        if (size > 0.f)
            target->setScale(size);
        label->setScale(0.5f);

        return container;
    };
    bool init(std::function<void(std::pair<std::string, int>)> cb)
    {
        if (!Popup::init(360.f, 160.f))
            return false;
        this->setTitle("Add keybind");
        m_callback = cb;
        m_keyCode = -1;
        auto content = CCMenu::create();
        content->setContentSize({300.f, 140.f});
        content->setAnchorPoint({0.5f, 0.5f});

        content->setLayout(RowLayout::create()
                               ->setAxisAlignment(AxisAlignment::Between)
                               ->setGap(15)
                               ->setCrossAxisLineAlignment(AxisAlignment::Start));
        content->setLayoutOptions(AnchorLayoutOptions::create()
                                      ->setAnchor(Anchor::Center));

        auto nameInput = TextInput::create(220.f, "Action Name", "bigFont.fnt");
        nameInput->setAnchorPoint({0.f, 0.f});
        m_textInput = nameInput;

        auto keyDefaultSpr = CCLabelBMFont::create("None", "bigFont.fnt");
        keyDefaultSpr->setAnchorPoint({0.5f, 0.4f});
        m_keyDef = keyDefaultSpr;
        auto keyDefault = EditorButtonSprite::create(keyDefaultSpr, EditorBaseColor::Gray);
        keyDefault->setAnchorPoint({1.f, 0.5f});

        auto keyDefBtn = CCMenuItemSpriteExtra::create(keyDefault, this, menu_selector(addKeyGUI::onKeyDef));

        auto nameInputWrapped = wrapperWithLabel(nameInput, 1.f, "Action Name", AxisAlignment::Start);
        nameInputWrapped->setAnchorPoint({0.f, 0.5f});

        auto separator = CCLabelBMFont::create(":", "bigFont.fnt");
        separator->setScale(1.f);
        separator->setAnchorPoint({0.5f, 0.7f});


        auto okBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create(
            "OK",               
            0,                   
            false,              
            "goldFont.fnt",      
            "GJ_button_01.png",  
            30.0f,              
            1.0f                 
        ),
        this,
        menu_selector(addKeyGUI::onConfirm)
        );
        okBtn->setPosition({180,30});
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
    void onKeyDef(CCObject *sender)
    {
        keyEdit::setKeyPopup::open(sender,[this](int keyCode) {
            log::info("{}",keyCode);
            std::string toDisplay;
            if (keyCode != -2)  // skip if pressed "esc"
                toDisplay = keyToString(keyCode);
            
            m_keyDef->setString(toDisplay.c_str());
            m_keyCode = keyCode;
            m_keyDef->updateLabel();
        });
    }
    void onConfirm(CCObject *sender)
    {
        std::pair<std::string, int> toReturn;
        int kC = m_keyCode;
        std::string actionName = m_textInput->getString();
        toReturn = {actionName,kC};
        
        if (m_callback)
            m_callback(toReturn);

        this->onClose(nullptr);
        delete this; // close for good :1

    }

public:
    static addKeyGUI *create(std::function<void(std::pair<std::string, int>)> cb)
    {
        auto ret = new addKeyGUI;
        if (ret->init(cb))
        {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
    static void open(CCObject *,std::function<void(std::pair<std::string, int>)> cb)
    {
        auto layer = create(cb);
        layer->show();
    }
};














class setupKeyBindsGUI : public geode::Popup
{
private:
    CCNode* m_contentLayer;
    CCSize m_contentSize;
    ScrollLayer* m_scrollArea;
    Scrollbar* m_scrollBar;
public:
    static setupKeyBindsGUI *create(std::unordered_map<std::string, int> const &keybinds)
    {
        auto ret = new setupKeyBindsGUI;
        if (ret && ret->init(keybinds))
        {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(CCObject *, std::unordered_map<std::string, int> keyBindsDict)
    {
        auto layer = create(keyBindsDict);
        layer->show();
    }

private:

    bool init(std::unordered_map<std::string, int> const &keyBindsDict)
    {
        if (!Popup::init(440.f, 280.f))
            return false;

        this->setTitle("Setup level Keybinds");
        // How do you even open the menu without playing the level?

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
        scrollArea->m_contentLayer->setLayout(AxisLayout::create()
                                                  ->setAxisAlignment(AxisAlignment::Center));
        m_scrollArea = scrollArea;

        auto scrollBar = Scrollbar::create(scrollArea);
        scrollBar->setContentSize({50, 300});
        scrollBar->setAnchorPoint({1.f, 0.5f});
        scrollBar->setScaleY(-1.f);
        m_scrollBar = scrollBar;

        auto content = CCNode::create();
        content->setLayout(ColumnLayout::create()
                               ->setGap(5)
                               ->setAutoGrowAxis(contentSize.height)
                               ->setAxisAlignment(AxisAlignment::Center)
                               ->setAxisReverse(true));
        m_contentLayer = content;
        scrollArea->m_contentLayer->addChild(content);

        m_mainLayer->addChildAtPosition(scrollBar, geode::Anchor::Right, {-27.f, 0.f});
        bg->addChildAtPosition(listLabel, geode::Anchor::Center);
        listLabel->addChildAtPosition(scrollArea, geode::Anchor::TopLeft);
        m_mainLayer->addChildAtPosition(bg, geode::Anchor::Center);

        for (const auto &[key, def] : keyBindsDict)
        {
            auto Label = KeyBindsSection::create(key, def, {contentSize.width, 20.f});
            content->addChild(Label);
        };
        content->updateLayout();
        scrollArea->m_contentLayer->updateLayout();
        scrollArea->m_contentLayer->setContentHeight(content->getContentHeight());
        scrollArea->scrollToTop();

        auto windowSize = m_buttonMenu->getContentSize();

        auto addBtnSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        addBtnSpr->setScale(1.1f);
        auto addBtn = CCMenuItemSpriteExtra::create(
            addBtnSpr,
            this,
            menu_selector(setupKeyBindsGUI::onAdd));
        addBtn->setAnchorPoint({0.5, 0.5});
        addBtn->setPosition(
            {windowSize.width - addBtn->getContentWidth() / 4,
             addBtn->getContentHeight() / 4});

        auto deleteBtnSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        deleteBtnSpr->setScale(1.25f);
        auto deleteBtn = CCMenuItemSpriteExtra::create(
            deleteBtnSpr,
            this,
            menu_selector(setupKeyBindsGUI::onDelete));

        deleteBtn->setAnchorPoint({0.5, 0.5});
        auto deleteSize = deleteBtn->getScaledContentSize();
        deleteBtn->setPosition(
            {deleteSize.width / 4,
             deleteSize.height / 4});

        m_buttonMenu->addChild(addBtn);
        m_buttonMenu->addChild(deleteBtn);

        return true;
    };

    void onAdd(CCObject *sender)
    {
        addKeyGUI::open(sender,[this](std::pair<std::string, int> actionKeyCode) {
            auto actionName = actionKeyCode.first;
            auto actionKey = actionKeyCode.second;
            if (actionName.empty() || actionKey == -2)
                return ;
            auto actionLabel = KeyBindsSection::create(actionName,actionKey,{m_contentSize.width,20.f});
            m_contentLayer->addChild(actionLabel);
            m_contentLayer->updateLayout();
            m_scrollArea->m_contentLayer->updateLayout();
            m_scrollArea->m_contentLayer->setContentHeight(m_contentLayer->getContentHeight());
            m_scrollArea->scrollToTop();
            m_scrollBar->setScale(-1.f);
        });
    }
    void onDelete(CCObject *sender)
    {
        log::info("Saborrr deleted");
    }
};