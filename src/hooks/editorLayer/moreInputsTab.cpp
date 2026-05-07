#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <matjson.hpp>
#include "../../layers/editor/setupGUI.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        
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
        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto keys = keybindsAPI::getLevelKeyBinds(playLayer ? (CCLayer*)playLayer: (CCLayer*)editorLayer,true);
        setupKeyBindsGUI::open(scene,keys);
    };
};