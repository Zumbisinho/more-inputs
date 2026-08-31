#include "customTriggers.hpp"
#include "Geode/cocos/sprite_nodes/CCSprite.h"
#include "Geode/loader/Log.hpp"
#include "Geode/ui/Layout.hpp"
#include "keybindsCache.hpp"
#include "smjs.object-collab/include/EditorPopupConfig.hpp"
#include "smjs.object-collab/include/ObjectInfo.hpp"
#include <fmt/format.h>

std::string customTriggers::TouchMacroTrigger::format(){
    return fmt::format("{}/{}",m_pressGroupId,m_releaseGroupId);
};

void customTriggers::TouchMacroTrigger::postEditorInit() {
    this->setTriggerTextProperty(105,{0,-4},0.75);
    createInitLinkageCallback(m_controlID,m_auxTriggers);

    
};

PopupConfig customTriggers::TouchMacroTrigger::getEditConfig(const Selected &selected) {
    auto disarmOnFirst = ToggleMenu::builder()
        .title("Disarm On\nFirst Key")
        .id("touch-macro-disarm-on-first"_spr)
        .onValue([](const bool value, const Selected &selected, Popup *popup) {
            applyValueToSelected(selected, &TouchMacroTrigger::m_disarmOnFirst, value);
            applyValueToSelected(selected, &TouchMacroTrigger::m_multiActivate, !value);
        })
        .currentValue([](const Selected &selected, Popup *popup) {
            return getCommonValueOrDefault<bool>(selected, &TouchMacroTrigger::m_disarmOnFirst);
        })
        .inverse(true)
    .build();

    return PopupConfig::builder()
        .triggerToggles(true)
        .height(280.f)
        .width(440.f)
        .title("Edit Touch Macro")
        .onClose([](cocos2d::CCObject* sender, const Selected& selected, geode::Popup* popup){
            for (auto& obj: selected){
                auto convert = static_cast<TouchMacroTrigger*>(obj);
                convert->bulkApplyDynamicProps();
                convert->selectAllAux();
            };
        })
        .leftToggle(std::move(disarmOnFirst))
        .info(InfoPopup::builder().title("Help")
            .description("Listens for a <cy>player action</c> and activates groups when "
            "that action is detected. Use the <cj>Action</c> selector to "
            "choose which input event this trigger will monitor."

            "\n<cs>Press ID:</c> the group that will be activated "
            "when the selected action key is <cg>pressed</c>."

            "\n<cs>Release ID:</c> the group that will be activated "
            "when the selected action key is <cr>released</c>."

            "\n<cj>Action:</c> determines which player input this trigger will "
            "listen for."

            "\n<co>Disarm On First Key</c> causes the trigger to deactivate "
            "itself after the first valid input is detected.")
        .build())
        .menu(CustomValueMenu::builder()
            .id("touch-macro-dropdown"_spr)
            .factory([](const Selected &selected, Popup *popup) -> CCMenu * {
                return ActionDropdown<TouchMacroTrigger>::create(selected);
                })
                .build()
        )
        .menu(AxisLayoutMenu::builder()
            .axis(Axis::Row)
            .gap(20)
            .padding({20,60,20,0})
            .menu(NumericMenu::builder()
                .id("touch-macro-press-groupId"_spr)
                .title("Press Id")
                .min(0)
                .max(9999)
                .precision(0)
                .inputType(NumericMenu::InputType::Arrows)
                .onValue([](const int value, const Selected &selected, Popup *popup) {
                    applyValueToSelected(selected,&TouchMacroTrigger::m_pressGroupId,value);
                    for (auto& obj : selected){
                        auto trig = typeinfo_cast<customTriggers::TouchMacroTrigger *>(obj);
                        if (!trig)
                            continue;
                
                        trig->updateProperty(105, trig->format());
                    }
                })
                .currentValue([](const Selected &selected, Popup *popup) {
                    return getCommonValueOrDefault(selected, &TouchMacroTrigger::m_pressGroupId);
                })
            .build())
            .menu(NumericMenu::builder()
                .id("touch-macro-release-groupId"_spr)
                .title("Release Id")
                .min(0)
                .max(9999)
                .precision(0)
                .inputType(NumericMenu::InputType::Arrows)
                .onValue([](const int value, const Selected &selected, Popup *popup) {
                    applyValueToSelected(selected, &TouchMacroTrigger::m_releaseGroupId, value);
                    for (auto& obj : selected){
                        auto trig = typeinfo_cast<customTriggers::TouchMacroTrigger *>(obj);
                        if (!trig)
                            continue;
                
                        trig->updateProperty(105, trig->format());
                    }
            }
                )
                .currentValue([](const Selected &selected, Popup *popup) {
                    return getCommonValueOrDefault(selected, &TouchMacroTrigger::m_releaseGroupId);
                })
            .build())
        .build())
        .build()
        ;
};

$on_mod(Loaded) {
    ObjectAPI::registerObject(ObjectInfo::builder()
    .id(customTriggers::TouchMacroTrigger::macroId)
    .sprite("touchMacro.png"_spr)
    .editorTab(EditorTab::None)
    .editObject(customTriggers::TouchMacroTrigger::getEditConfig)
    .editorButtonColor(EditorButtonColor::LightGray)
    .construction(ComplexObject::builder()
        .factory(customTriggers::TouchMacroTrigger::create)
        .customProperties({
            PropertyInterface::from(101, &customTriggers::TouchMacroTrigger::m_actionIndex, 0), 
            PropertyInterface::from(102, &customTriggers::TouchMacroTrigger::m_pressGroupId, 0), 
            PropertyInterface::from(103, &customTriggers::TouchMacroTrigger::m_releaseGroupId, 0), 
            PropertyInterface::from(104, &customTriggers::TouchMacroTrigger::m_disarmOnFirst, false),
            PropertyInterface::from(204, &customTriggers::TouchMacroTrigger::m_multiActivate, true),  
            PropertyInterface::from(105, &customTriggers::TouchMacroTrigger::m_formatedTriggerLabel, "0/0")
        }).build()
    )
    .build());
}
void customTriggers::EditKeybindTrigger::postEditorInit(){
    if (!this->m_disableKey){
        this->init("editKeybindDisabled.png"_spr);
    };
    
}
void customTriggers::EditKeybindTrigger::triggerObject(GJBaseGameLayer *layer, int uniqueID, const gd::vector<int> *remapKeys){
    if (!KeybindCache::initialized){
        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        KeybindCache::init(playLayer ? (CCLayer*)playLayer: (CCLayer*)editorLayer);
    }

    auto actionId = KeybindCache::keySettings[m_actionIndex].first;
    if (actionId == 0)
        return;

    if (KeybindCache::mobileKeyNodes.empty() && !m_disableKey){
        KeybindCache::mobileKeysToHideOnInit.push_back(actionId);
    }

    auto it = KeybindCache::mobileKeyNodes.find(actionId);

    if (it != KeybindCache::mobileKeyNodes.end()) {

        auto node = it->second;
        if (node->retainCount() == 0)
            return;
        node->setVisible(this->m_disableKey);
    }
};

PopupConfig customTriggers::EditKeybindTrigger::getEditConfig(const Selected &selected) {
    return PopupConfig::builder()
        .triggerToggles(true)
        .height(280.f)
        .width(440.f)
        .title("Edit Keybind Trigger")
        .info(InfoPopup::builder().title("Help")
            .description("Enables/Disables the selected key, "
            "when disable, both mobile button and the press are disabled."

            "\n<cs>Activate Keybind:</c> enables/disables the selected key."
            )
        .build())
        .menu(CustomValueMenu::builder()
            .id("edit-keybind-macro-dropdown"_spr)
            .factory([](const Selected &selected, Popup *popup) -> CCMenu * {
                return ActionDropdown<EditKeybindTrigger>::create(selected);
                })
        .build()
        )
        .menu(AxisLayoutMenu::builder()
            .axis(Axis::Row)
            .gap(20)
            .padding({20,60,20,0})
            .menu(ToggleMenu::builder()
                .title("Activate Keybind")
                .id("edit-keybind-activate"_spr)
                .onValue([](const bool value, const Selected &selected, Popup *popup) {
                    applyValueToSelected(selected, &EditKeybindTrigger::m_disableKey, value);
                    for (auto& obj : selected){
                        if (!value)
                            obj->init("editKeybindDisabled.png"_spr);
                        else
                            obj->init("editKeybind.png"_spr);
                    }
                })
                .currentValue([](const Selected &selected, Popup *popup) {
                    return getCommonValueOrDefault<bool>(selected, &EditKeybindTrigger::m_disableKey);
                })
                .inverse(false)
            .build())
        .build()
        )
        
    .build();
};
$on_mod(Loaded) {
    ObjectAPI::registerObject(ObjectInfo::builder()
    .id("edit-keybind-trigger"_spr)
    .sprite("editKeybind.png"_spr)
    .editorTab(EditorTab::None)
    .editObject(customTriggers::EditKeybindTrigger::getEditConfig)
    .editorButtonColor(EditorButtonColor::LightGray)
    .construction(ComplexObject::builder()
        .factory(customTriggers::EditKeybindTrigger::create)
        .customProperties({
            PropertyInterface::from(101, &customTriggers::EditKeybindTrigger::m_actionIndex, 0), 
            PropertyInterface::from(102, &customTriggers::EditKeybindTrigger::m_disableKey, false)
        }).build()
    )
    .build());
}