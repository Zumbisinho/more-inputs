#include <Geode/Geode.hpp>

using namespace geode::prelude;

class KeyBindsLocalConfigGui : public geode::Popup
{
public:
    static KeyBindsLocalConfigGui *create(std::map<std::string, std::string> const &keybinds)
    {
        auto ret = new KeyBindsLocalConfigGui;
        if (ret && ret->init(keybinds))
        {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    };
    static void open(CCObject *, std::map<std::string, std::string> keyBindsDict)
    {
        auto layer = create(keyBindsDict);
        layer->show();
    }

private:
    bool init(std::map<std::string, std::string> const &keyBindsDict)
    {
        if (!Popup::init(440.f, 280.f))
            return false;

        auto playLayer = PlayLayer::get();
        if (playLayer && playLayer->m_level)
        {
            std::string levelName = playLayer->m_level->m_levelName;
            this->setTitle(levelName + " Keybinds");
        }
        else
            return false; // How do you even open the menu without playing the level?

        auto listLabel = CCScale9Sprite::create("square02b_001.png");
        listLabel->setColor(ccc3(145, 80, 48));
        listLabel->setContentSize({360.f, 200.f});
        listLabel->setZOrder(2);

        auto contentSize = listLabel->getContentSize();

        auto bg = CCScale9Sprite::create("square02b_001.png");
        bg->setContentSize({362.5f, 202.5f});
        bg->setColor(ccc3(129, 72, 43));
        bg->setZOrder(1);

        auto scrollArea = ScrollLayer::create({contentSize.width -20, contentSize.height}, true, true);
        scrollArea->setAnchorPoint({0.f, 1.f});
        scrollArea->ignoreAnchorPointForPosition(false);

        auto content = CCNode::create();
        content->setLayout(ColumnLayout::create()
            ->setGap(5)
            ->setAutoGrowAxis(contentSize.height)
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAxisReverse(true)
        );
        
            
        auto scrollBar = Scrollbar::create(scrollArea);
        scrollBar->setContentSize({50, 300});
        scrollBar->setAnchorPoint({1.f,0.5f});
        
        
        scrollArea->m_contentLayer->addChild(content);
        

        m_mainLayer->addChildAtPosition(scrollBar, geode::Anchor::Right, {-27.f,0.f});
        bg->addChildAtPosition(listLabel, geode::Anchor::Center);
        listLabel->addChildAtPosition(scrollArea, geode::Anchor::TopLeft);
        m_mainLayer->addChildAtPosition(bg, geode::Anchor::Center);

        for (const auto &[key, def] : keyBindsDict)
        {
            for (int i = 0; i < 10; i++) {
                auto item = CCLayerColor::create(ccc4(0, 0, 0, 80));
                item->setContentSize({280, 40});
                

                auto label = CCLabelBMFont::create(
                    fmt::format("Item {}", i).c_str(),
                    "bigFont.fnt"
                );
                label->setPosition({10, 20});
                label->setAnchorPoint({0, 0.5});

                item->addChild(label);
                content->addChild(item);
            }



            //for (int i =0;i<10;i++)
            //{
            //    auto keyRow = CCMenu::create();
            //    keyRow->setLayout(RowLayout::create());
//
            //    auto nameLabel = CCLabelBMFont::create(key.c_str(), "bigFont.fnt");
            //    nameLabel->setAnchorPoint({0, 0.5});
//
            //    auto defLabel = CCLabelBMFont::create(def.c_str(), "goldFont.fnt");
            //    defLabel->setScale(0.6f);
//
            //    auto defBtn = CCMenuItemSpriteExtra::create(
            //        defLabel,
            //        this,
            //        nullptr);
            //    defBtn->setAnchorPoint({1, 0.5});
//
            //    keyRow->addChild(nameLabel);
            //    keyRow->addChild(defBtn);
            //    content->addChild(keyRow);
            //}
            
        };
        content->updateLayout();
        scrollArea->m_contentLayer->setContentHeight(content->getContentHeight());
        scrollArea->scrollToTop();

        return true;
    };
};