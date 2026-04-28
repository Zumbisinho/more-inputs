#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <matjson.hpp>
#include "../../layers/editor/setupGUI.hpp"
#include "../../utils/keybindsAPI.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        struct Fields{
            matjson::Value *json;
        };
        alpha::editor_tabs::addTab("tab"_spr,alpha::editor_tabs::BUILD,
        [this] { // ? Setup to build the Tab
            std::vector<Ref<CCNode>> buttons;
            
                      
            auto btn = getSpriteButton(
                "addKeyMIProt.png"_spr,                    
                menu_selector(MyEditorUI::onSetupBtn),  
                this->m_tabsMenu,                     
                0.8f, 
                4, 
                {0,0}
                                                    
            );

            buttons.push_back(btn);
            return alpha::editor_tabs::createEditButtonBar(buttons);
        },
        [] { // ? Setup to Place the tabImg
            return CCSprite::createWithSpriteFrameName("btnUgly.png"_spr);
        });
        return true;
    };

    void onSetupBtn(CCObject* sender)
    {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto keys = keybindsAPI::getLevelKeyBinds();
        setupKeyBindsGUI::open(scene,keys);
    };
};