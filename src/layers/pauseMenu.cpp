#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include "../utils/keybindsAPI.hpp"
#include "../utils/keycodeToString.hpp"
#include "gui.hpp"
#include <unordered_map>

using namespace geode::prelude;

class $modify(MyLayer, PauseLayer)
{
    static void onModify(auto &self)
    {
        Result<> plCustomSetup = self.setHookPriority("PauseLayer::customSetup", INT_MIN);
    }
    void customSetup()
    {

        PauseLayer::customSetup();

        auto sideMenu = this->getChildByID("left-button-menu");

        if (!sideMenu)
            return;

        if (sideMenu->getChildByID("mi-key-config"))
            return;

        auto spr = CircleButtonSprite::createWithSpriteFrameName("btnUgly.png"_spr);

        auto btn = CCMenuItemSpriteExtra::create(
            spr, this, menu_selector(MyLayer::onClick));

        btn->setID("mi-key-config");

        sideMenu->addChild(btn);
        sideMenu->setLayout(
            ColumnLayout::create()
                ->setGap(12.f)
                ->setAutoScale(true)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisAlignment(AxisAlignment::Center));

        sideMenu->updateLayout();

        return;
    };
    void onClick(CCObject * obj)
    {
        auto keys = keybindsAPI::getLevelKeyBinds();

        KeyBindsLocalConfigGui::open(obj,keys);
    }
};
