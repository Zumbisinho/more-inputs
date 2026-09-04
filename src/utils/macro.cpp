#include "macro.hpp"

#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/cocoa/CCObject.h"
#include "Geode/cocos/support/data_support/ccCArray.h"
#include "Geode/loader/Log.hpp"
#include "levelCache.hpp"
#include "smjs.object-collab/include/ObjectAPI.hpp"
#include "smjs.object-collab/include/object_collab.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/CustomizeObjectLayer.hpp>
#include <Geode/binding/EditGameObjectPopup.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/SetupTriggerPopup.hpp>
#include <Geode/modify/CustomizeObjectLayer.hpp>
#include <Geode/modify/EditTriggersPopup.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <vector>

template <typename T>
void removeValue(std::vector<T> &vec, const T &value) {
    vec.erase(
        std::remove(vec.begin(), vec.end(), value),
        vec.end()
    );
}

namespace MacroTriggers {

std::function<void(CCPoint)> macroTriggerOnCreateCallBacks = nullptr;

macroTrigger::macroTrigger(
    ObjectInfo *info,
    ObjectTraits traits
)
    : CustomObject(info, std::move(traits)) {
}
// on Delete func
class $modify(LevelEditorLayer){
    void removeSpecial(GameObject* object) {
        auto macro = typeinfo_cast<macroTrigger *>(object);
        if (macro){
            LevelCache::freeControlId(macro->m_controlID);
            LevelCache::m_macroTriggers.erase(macro->m_controlID);
        }
        if (auto EUI = EditorUI::get())
            EUI->deselectObject(object);
        LevelEditorLayer::removeSpecial(object);
    }
};


class $modify(GetEditorUI, EditorUI) {
    static void onModify(auto &self) {
        if (!self.setHookPriorityBeforePre("EditorUI::editObject", "smjs.object-collab"))
            log::warn("Fucccck");
    }
    struct Fields {
        GameObject *m_beforeSelectedGO = nullptr;
        bool ignoreSelect = false;
    };
    void selectObject(GameObject *object, bool ignoreFilter) {
        if (!object)
            return;
        EditorUI::selectObject(object, ignoreFilter);
        if (m_fields->ignoreSelect) // 69
            return;
        log::warn("To selecionado\n{}",object);
        // if its selecting the macro itself
        if (auto macro = typeinfo_cast<macroTrigger *>(object)) {
            if (macro->m_auxTriggers[0]->m_obj == nullptr) // onCreate the aux are not ready
                return;
            macro->selectAllAux();
        }
        // selecting a aux trigger
        if (auto auxTrigger = typeinfo_cast<EffectGameObject *>(object)) {
            if (auxTrigger->m_objectMaterial == LevelCache::IdentityMaterialId) {
                log::warn("{}", LevelCache::m_macroTriggers.size());
                auto macro = LevelCache::m_macroTriggers[auxTrigger->m_controlID];
                if (macro && !macro->m_auxTriggers.empty() && macro->m_auxTriggers[0]->m_obj == nullptr) // onCreate the aux are not ready
                    return;
                macro->selectAllAux();
                macro->selfSelect();
            }
        };
    }
    void selectObjects(CCArray *objects, bool ignoreFilter) {
        EditorUI::selectObjects(objects, ignoreFilter);
        if (m_fields->ignoreSelect)
            return;
        log::warn("To selecionado\n{}",objects);
        for (auto object : objects->asExt<GameObject *>()) {
            // if its selecting the macro itself
            if (auto macro = typeinfo_cast<macroTrigger *>(object)) {
                macro->selectAllAux();
                return;
            }
            // selecting a aux trigger
            if (auto auxTrigger = typeinfo_cast<EffectGameObject *>(object)) {
                if (auxTrigger->m_objectMaterial == LevelCache::IdentityMaterialId) {
                    auto macro = LevelCache::m_macroTriggers[auxTrigger->m_controlID];
                    if (macro && !macro->m_auxTriggers.empty() && macro->m_auxTriggers[0]->m_obj == nullptr)
                        return;
                    macro->selectAllAux();
                    macro->selfSelect();
                }
            };
        }
    }

    bool init(LevelEditorLayer *editorLayer) {
        if (!EditorUI::init(editorLayer))
            return false;

        ColorAction *action = LevelEditorLayer::get()->m_levelSettings->m_effectManager->getColorAction(676);
        action->m_fromOpacity = 0;

        return true;
    }
    void editObject(CCObject *sender) {
        if (m_selectedObjects->count() == 0) {
            EditorUI::editObject(sender);
            return;
        };

        auto selected = m_selectedObjects->asExt<GameObject *>().toVector();
        std::vector<macroTrigger *> macroTriggers = {};
        std::vector<GameObject *> toSelect = {}; // cocos cannot make a good function

        m_fields->ignoreSelect = true; // to not re-select the unselected objects
        for (auto obj : selected) {
            log::warn("{}", obj);
            toSelect.push_back(obj);

            auto macro = typeinfo_cast<macroTrigger *>(obj);
            if (!macro)
                continue;
            macroTriggers.push_back(macro);
        }
        // unsellects all the aux triggers outside the for loop because i cannot create a copy of the cur selected
        for (auto &macro : macroTriggers) {
            log::warn("aux count: {}", macro->m_auxTriggers.size());
            for (auto &auxTrigger : macro->m_auxTriggers) {
                auto obj = static_cast<GameObject *>(auxTrigger->m_obj);
                log::warn("Removing: {}", obj);
                removeValue(toSelect, obj);
            }
        }
        auto toSelectCCAssAy = CCArray::create();
        for (auto &GO : toSelect) {
            toSelectCCAssAy->addObject(GO);
        }
        deselectAll();
        selectObjects(toSelectCCAssAy, true);
        updateButtons();
        updateObjectInfoLabel();

        EditorUI::editObject(sender);
    }

    gd::string copyObjects(CCArray* objects, bool copyColors, bool sort) {
        // imagine if i wasted 3 hours over removing and iterating on the same vec!!
        auto copy = objects->shallowCopy();
        for (auto& obj : objects->asExt<GameObject*>()){
            auto macro = typeinfo_cast<macroTrigger *>(obj);
            if (!macro)
                continue;
            for (auto& aux: macro->m_auxTriggers){
                copy->removeObject(aux->m_obj);
            }

        };
        gd::string ret = EditorUI::copyObjects(copy, copyColors, sort);
        //selectObjects(copy,true);
        return ret;
    }

    CCArray *pasteObjects(gd::string str, bool withColor, bool noUndo) {
        ignoreOnCreateCB = true;
        CCArray *ret = EditorUI::pasteObjects(str, withColor, noUndo);
        log::warn("{}", ret);
        for (auto& GO : ret->asExt<GameObject>()){
            auto macro = typeinfo_cast<macroTrigger *>(GO);
            if (!macro)
                continue;
            macro->createTriggersAtPos(macro->getPosition());
            macro->bulkApplyDynamicProps();
            LevelCache::m_macroTriggers[macro->m_controlID] = macro;
        }
        
        return ret;
    }

    void onCreateObject(int id) {
        EditorUI::onCreateObject(id);

        if (m_selectedObject == nullptr)
            return;

        if (m_selectedObject == m_fields->m_beforeSelectedGO)
            return;

        if (id < 100000000)
            return;

        auto obj = ObjectAPI::getCustomObject(id);

        if (!obj)
            return;

        auto convertedId =
            static_cast<std::string>(obj->getID());

        if (!convertedId.contains("macro"))
            return;

        if (macroTriggerOnCreateCallBacks) {
            macroTriggerOnCreateCallBacks(
                m_selectedObject->getPosition()
            );

            geode::log::warn("Executandoooo");
        }

        m_fields->m_beforeSelectedGO = m_selectedObject;
    }
};

class $modify(idk, CustomizeObjectLayer) {
    void onClose(CCObject *sender) {
        auto EUI = EditorUI::get();
        auto selected = EUI->m_selectedObjects->shallowCopy();

        static_cast<GetEditorUI *>(EUI)->m_fields->ignoreSelect = false;
        if (EUI->m_selectedObject)
            selected->addObject(EUI->m_selectedObject);
        for (auto &obj : selected->asExt<GameObject *>()) {
            if (auto macro = typeinfo_cast<macroTrigger *>(obj)) {
                macro->selectAllAux();
            };
        }

        CustomizeObjectLayer::onClose(sender);
    }
};
class $modify(idk2,EditTriggersPopup) {
    void onClose(CCObject *sender) {
        auto EUI = EditorUI::get();
        auto selected = EUI->m_selectedObjects->shallowCopy();

        static_cast<GetEditorUI *>(EUI)->m_fields->ignoreSelect = false;
        if (EUI->m_selectedObject)
            selected->addObject(EUI->m_selectedObject);
        for (auto &obj : selected->asExt<GameObject *>()) {
            if (auto macro = typeinfo_cast<macroTrigger *>(obj)) {
                macro->selectAllAux();
            };
        }

        EditTriggersPopup::onClose(sender);
    }
};

void macroTrigger::firstSetup() {
    createTriggers();
    customInit();
    m_objectMaterial = LevelCache::IdentityMaterialId;
    LevelCache::m_macroTriggers[this->m_controlID] = this;
    log::warn("atualizadoss {}",this->m_controlID);
};

void macroTrigger::selectAllAux() {
    auto EUI = EditorUI::get();
    auto toSelect = CCArray::create();

    static_cast<GetEditorUI *>(EUI)->m_fields->ignoreSelect = false;
    if (this->m_auxTriggers.empty()) {
        return;
    }
    for (auto &aux : m_auxTriggers) { // 205
        if (!aux->m_obj)
            return;
        toSelect->addObject(aux->m_obj);
    };

    EUI->selectObjects(toSelect, true);
    EUI->updateButtons();
    EUI->updateObjectInfoLabel();
};

void macroTrigger::selfSelect() {
    auto EUI = EditorUI::get();
    auto toSelect = CCArray::create();

    static_cast<GetEditorUI *>(EUI)->m_fields->ignoreSelect = false;
    toSelect->addObject(this);

    EUI->selectObjects(toSelect, true);
    EUI->updateButtons();
    EUI->updateObjectInfoLabel();
};

void macroTrigger::changeAllControlId(int controlId){
    this->m_controlID = controlId;
    for (auto& trig : m_auxTriggers){
        if (!trig->m_obj)
            continue;
        trig->m_obj->m_controlID = controlId;
    };
};

} // namespace MacroTriggers