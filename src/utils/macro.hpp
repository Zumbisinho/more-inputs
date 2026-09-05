#pragma once

#include "Geode/binding/EffectGameObject.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "smjs.object-collab/include/CustomObject.hpp"
#include "smjs.object-collab/include/EditorPopupConfig.hpp"
#include "smjs.object-collab/include/object_collab.hpp"

#include "keybindsCache.hpp"

#include <Geode/Enums.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <concepts>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace geode::prelude;
using namespace object_collab::prelude;

namespace LevelCache {
int getNextFreeControlId();
void addLLSATCBs(std::function<void(std::vector<EffectGameObject *>)> func, int controlId);

} // namespace LevelCache

/// Macro for Alter Any EffectGO derived props, like the count and spawn triggers
/// @param x The prop for the trigger
/// @param y The value to change Ex: a class Variable
#define $Alter(x, y) \
    [this](auto &self) { self.x = y; }

#define setMacroId(x)                                                    \
    static inline const std::string macroId = std::string(x) + "-macro"; \
    std::string getMacroId() const override {                            \
        return macroId;                                                  \
    }

namespace MacroTriggers {

inline bool ignoreOnCreateCB = false;

extern std::function<void(CCPoint)> macroTriggerOnCreateCallBacks;

class AbstractMacroAuxiliarTrigger {
protected:
    std::vector<std::function<void(EffectGameObject &)>> m_alterations;
    std::vector<std::function<void(EffectGameObject &)>> m_initPropSetting;

public:
    EffectGameObject *m_obj = nullptr;
    int m_objID = 0;
    virtual void applyDynamicProps(EffectGameObject *) {};
    virtual void applyInitProps() {};
    virtual void createTrigger(CCPoint pos, int id, int controlId) {};
};

template <std::derived_from<EffectGameObject> ClassType, int triggerObjId>
struct MacroAuxiliarTrigger : public AbstractMacroAuxiliarTrigger {
protected:
    std::vector<std::function<void(ClassType &)>> m_alterations;
    std::vector<std::function<void(ClassType &)>> m_initPropSetting;

public:
    int m_objID = 0;


    template <typename... Funcs>
    void addDynamicProps(Funcs &&...funcs) {
        (m_alterations.emplace_back(std::forward<Funcs>(funcs)), ...);
    }

    void applyDynamicProps(EffectGameObject *genericObj) override {
        auto obj = static_cast<ClassType *>(genericObj);
        for (auto &alteration : m_alterations) {
            alteration(*obj);
        }
    }

    template <typename... Funcs>
    void addInitProps(Funcs &&...funcs) {
        (m_initPropSetting.emplace_back(std::forward<Funcs>(funcs)), ...);
    }

    void applyInitProps() override{

        if (!m_obj) {

            return;
        }

        for (auto &alteration : m_initPropSetting) {
            alteration(*static_cast<ClassType *>(m_obj));
        }

        m_initPropSetting.clear();
    }

    void createTrigger(CCPoint pos, int id, int controlId) override {
        auto obj = LevelEditorLayer::get()->createObject(
            triggerObjId,
            pos,
            false
        );

        if (!obj) {
            geode::log::error(
                "Failed to create object with ID {}",
                triggerObjId
            );

            return;
        }

        m_obj = static_cast<ClassType *>(obj);
        m_obj->m_controlID = controlId;
        m_obj->m_zLayer = ZLayer::B5;
        m_obj->m_baseColor->m_colorID = 676;
        m_obj->m_objectMaterial = getConfig()["keyBindsIdentityMaterialId"].asInt().unwrapOr(67);
        m_objID = triggerObjId;

    }
};

class macroTrigger : public object_collab::CustomObject<EffectGameObject> {

public:
    static inline std::string macroId;
    std::vector<AbstractMacroAuxiliarTrigger *> m_auxTriggers = {};
    virtual std::string getMacroId() const {
        return macroId;
    }

    virtual void customInit() {}

    void bulkApplyDynamicProps() {
        for (auto &trg : m_auxTriggers) {
            trg->applyDynamicProps(trg->m_obj);
        }
    }

    void firstSetup() override;

    std::function<void(CCPoint)> onCreateCB;

    macroTrigger(ObjectInfo *info, ObjectTraits traits);

    void createTriggers() {
        if (ignoreOnCreateCB){
            ignoreOnCreateCB = false;
            return;
        };
        auto nextFreeControlId = LevelCache::getNextFreeControlId();
        
        this->m_controlID = nextFreeControlId;

        macroTriggerOnCreateCallBacks = createCallback(nextFreeControlId, m_auxTriggers);
    }
    void createTriggersAtPos(const CCPoint &pos) {
        auto nextFreeControlId = LevelCache::getNextFreeControlId();

        this->m_controlID = nextFreeControlId;

        CCArray *toSelect = CCArray::create();
        for (auto &trig : m_auxTriggers) {
            trig->createTrigger(pos, trig->m_objID, nextFreeControlId);
            trig->applyInitProps();
            toSelect->addObject(trig->m_obj);
        }

        EditorUI::get()->selectObjects(toSelect, true);
        
    }

    std::function<void(CCPoint)> createCallback(int controlId, std::vector<AbstractMacroAuxiliarTrigger *> triggers) {
        return [controlId, triggers](CCPoint point) {
            CCArray *toSelect = CCArray::create();
            for (auto &trig : triggers) {
                trig->createTrigger(point, trig->m_objID, controlId);
                trig->applyInitProps();
                toSelect->addObject(trig->m_obj);
            }

            EditorUI::get()->selectObjects(toSelect, true);
        };
    }
    void createInitLinkageCallback(int controlId, std::vector<AbstractMacroAuxiliarTrigger *> &triggers) {
        auto cb = [controlId, &triggers](std::vector<EffectGameObject *> auxTriggers) {
            std::size_t i = 0;
            for (auto trigger : triggers) {
                trigger->m_obj = auxTriggers[i++];
            }
        };

        LevelCache::addLLSATCBs(cb, controlId);
    }
    void selectAllAux();

    void selfSelect();

    void changeAllControlId(int controlId);
};

#define $macro(name) \
    name:            \
public               \
    MacroTriggers::macroTrigger

} // namespace MacroTriggers