#pragma once

#include "Geode/binding/EffectGameObject.hpp"
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/cocos/cocoa/CCGeometry.h"
#include "Geode/loader/Log.hpp"
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

#define setMacroId(x) \
    static inline const std::string macroId = std::string(x) + "-macro"; \
    std::string getMacroId() const override { \
        return macroId; \
    }

namespace MacroTriggers {

extern std::function<void(CCPoint)> macroTriggerOnCreateCallBacks;



class AbstractMacroAuxiliarTrigger {
protected:
    std::vector<std::function<void(EffectGameObject &)>> m_alterations;
    std::vector<std::function<void(EffectGameObject &)>> m_initPropSetting;

public:
    EffectGameObject *m_obj = nullptr;
    virtual void applyDynamicProps(EffectGameObject *) {};
};

template <std::derived_from<EffectGameObject> ClassType, int triggerObjId>
struct MacroAuxiliarTrigger : public AbstractMacroAuxiliarTrigger {
protected:
    std::vector<std::function<void(ClassType &)>> m_alterations;
    std::vector<std::function<void(ClassType &)>> m_initPropSetting;

public:
    int m_objID = 0;

    ~MacroAuxiliarTrigger<ClassType, triggerObjId>() {
        geode::log::info("Morri!");
    };

    template <typename... Funcs>
    void addDynamicProps(Funcs &&...funcs) {
        (m_alterations.emplace_back(std::forward<Funcs>(funcs)), ...);
    }

    void applyDynamicProps(EffectGameObject *genericObj) override {
        log::warn("antes do SC {}", genericObj);
        auto obj = static_cast<ClassType *>(genericObj);
        log::warn("size das alteracoes {}", m_alterations.size());
        for (auto &alteration : m_alterations) {
            log::warn("bulk thing {}", obj);
            alteration(*obj);
        }
    }

    template <typename... Funcs>
    void addInitProps(Funcs &&...funcs) {
        (m_initPropSetting.emplace_back(std::forward<Funcs>(funcs)), ...);
    }

    void applyInitProps() {
        geode::log::warn("m_obj = {}", fmt::ptr(m_obj));

        if (!m_obj) {
            geode::log::error("m_obj IS NULL");
            return;
        }

        for (auto &alteration : m_initPropSetting) {
            alteration(*static_cast<ClassType*>(m_obj));
        }

        m_initPropSetting.clear();
    }

    void createTrigger(CCPoint pos, int id, int controlId) {
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
        log::warn(
            "SETTING PRESS: this={} pressCount={}",
            fmt::ptr(this),
            fmt::ptr(&m_obj)
        );
    }

};

class macroTrigger : public object_collab::CustomObject<EffectGameObject> {

    

public:
    static inline std::string macroId;
    std::vector<AbstractMacroAuxiliarTrigger *> m_auxTriggers;
    virtual std::string getMacroId() const {
        return macroId;
    }

    virtual void customInit() {}

    void bulkApplyDynamicProps() {
        log::warn("size do m_auxTrigges{}", m_auxTriggers.size());
        for (auto &trg : m_auxTriggers) {
            log::warn("ponteiro no bulk aply{}", fmt::ptr(trg->m_obj));
            trg->applyDynamicProps(trg->m_obj);
        }
        log::warn("Di apply {}", m_auxTriggers.size());
    }

    void firstSetup() override;

    std::function<void(CCPoint)> onCreateCB;

    macroTrigger(ObjectInfo *info, ObjectTraits traits);

    template <typename... Triggers>
    void createTriggers(Triggers &...triggers) {
        auto nextFreeControlId = LevelCache::getNextFreeControlId();

        this->m_controlID = nextFreeControlId;

        macroTriggerOnCreateCallBacks =
            createCallback(nextFreeControlId, triggers...);
    }

    template <typename... Triggers>
    auto createCallback(int controlId, Triggers &...triggers) {
        return [controlId, &triggers...](CCPoint point) {
            CCArray* toSelect = CCArray::create(); 
            (
                (
                    triggers.createTrigger(point, triggers.m_objID, controlId),
                    triggers.applyInitProps(),
                    toSelect->addObject(triggers.m_obj)
                ),
                ...);
            EditorUI::get()->selectObjects(toSelect,true);
        };
    }
    void createInitLinkageCallback(int controlId, std::vector<AbstractMacroAuxiliarTrigger *> &triggers) {
        auto cb = [controlId, &triggers](std::vector<EffectGameObject *> auxTriggers) {
            std::size_t i = 0;
            for (auto trigger : triggers) {
                trigger->m_obj = auxTriggers[i++];
                log::warn("Ai dentctrus mudei o {}", trigger->m_obj);
            }
        };

        LevelCache::addLLSATCBs(cb, controlId);
    }
    void selectAllAux();

    void selfSelect();
    
};

#define $macro(name) \
    name:            \
public               \
    MacroTriggers::macroTrigger 

} // namespace MacroTriggers