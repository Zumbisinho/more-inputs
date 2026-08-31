#include "levelCache.hpp"

#include "Geode/loader/Log.hpp"
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/EffectGameObject.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <string>


void LevelCache::freeControlId(int controlId){
    if (controlId > maxFreeControlId)
        return;
    if (controlId < StartControlId)
        return;
    freeControlIds.insert(controlId);
    geode::log::warn("freed {} {}",controlId,freeControlIds);
};

int LevelCache::getNextFreeControlId() {
    if (!freeControlIds.empty()) { // if not reach the max
        auto toReturn = *freeControlIds.begin();
        freeControlIds.erase(toReturn);
        return toReturn;
    } else {
        return ++maxFreeControlId;
    };
};

void LevelCache::addLLSATCBs(std::function<void(std::vector<EffectGameObject *>)> func, int controlId) {
    onLevelLoadSetupAuxTriggersCallbacks[controlId] = std::move(func);
};

std::string printTheLongAssVar(std::unordered_map<int, std::function<void(std::vector<EffectGameObject *>)>> map) {
    std::string toReturn = "{\n";
    for (auto &[controlId, func] : map) {
        toReturn.append(fmt::format("'{}': Funcao,\n", controlId));
    };
    toReturn.pop_back(); // removes the last ,
    toReturn.append("\n}");

    return toReturn;
};

std::string printTheAuxThing(std::unordered_map<int, std::vector<EffectGameObject *>> map) {
    std::string toReturn = "{\n";
    for (auto &[key, value] : map) {
        std::string vectorStringfy = "[";
        for (auto &ptr : value) {
            vectorStringfy.append(fmt::format("{},", ptr->m_uniqueID));
        };
        vectorStringfy.pop_back(); // removes the last ,
        vectorStringfy.push_back(']');
        toReturn.append(fmt::format("'{}':{};\n", key, vectorStringfy));
    };
    toReturn.append("}");
    return toReturn;
};

void LevelCache::init(LevelEditorLayer *layer) {
    auto objs = CCArrayExt<GameObject *>(layer->m_objects);
    log::warn("Iniciando o level cache, size dos objs: {}", objs.size());
    if (objs.empty()) // empty level
        return;

    int minFreeControlId = StartControlId;

    std::unordered_map<
        int,                            // controlId
        std::vector<EffectGameObject *> // Pointer ig ik
        >
        auxiliarMacroTriggers;

    auto manageMinMaxControlId = [&](int controlId) {
        if (controlId > 32767 || controlId < -32767) { // if controlId is gargabe data, chance that it doesnt work: 1 in 281 trilion.  // ? Discovered because of tung rung sahurcles
            return;
        }
        if (controlId <= minFreeControlId)
            return;

        if (controlId == minFreeControlId + 1) {
            if (controlId == maxFreeControlId + 1) {
                maxFreeControlId++;
            };
            minFreeControlId++;
            return;
        };
        if (controlId > minFreeControlId) { // greater that min + 1
            if (controlId < maxFreeControlId) {
               freeControlIds.erase(controlId);
                return;
            };
            if (controlId > maxFreeControlId) {
                for (int i = maxFreeControlId + 1; i < controlId; i++) {
                    log::warn("To no loop o i agr é: {} eo max e o control id {} {}", i, maxFreeControlId, controlId);
                    freeControlIds.insert(i);
                };
                maxFreeControlId = controlId;
                return;
            }
        };
    };

    for (const auto &obj : objs) {
        if (auto commomTrigger = reinterpret_cast<EffectGameObject *>(obj); commomTrigger && commomTrigger->isTrigger()) {
            if (commomTrigger->m_controlID > 32767 || commomTrigger->m_controlID < -32767)
                continue;
            manageMinMaxControlId(commomTrigger->m_controlID);

            if (auto macro = typeinfo_cast<MacroTriggers::macroTrigger *>(obj)) {
                m_macroTriggers[macro->m_controlID] = macro;
                continue;
            };

            if (obj->m_objectMaterial != IdentityMaterialId) // Not a macroAuxTrigger
                continue;

            auxiliarMacroTriggers[commomTrigger->m_controlID].push_back(commomTrigger);

            
        }
    };
    log::warn("Depois de tudo Max: {} Min: {}, Lista: {}\nTbm aqui a lista de trigger pelo id: {}", maxFreeControlId, minFreeControlId, freeControlIds, printTheAuxThing(auxiliarMacroTriggers));
    for (auto& [key,value] : m_macroTriggers){
        log::warn("    {}: {},",key,value);
    }
    // linking aux triggers to the respective macro
    log::warn("var muito extensa: {}", printTheLongAssVar(LevelCache::onLevelLoadSetupAuxTriggersCallbacks));
    for (auto &[controlId, vec] : auxiliarMacroTriggers) {
        auto cb = onLevelLoadSetupAuxTriggersCallbacks.find(controlId);
        if (cb == onLevelLoadSetupAuxTriggersCallbacks.end())
            continue;
        log::warn("Deve ta executado acho ig");

        cb->second(vec);
    }
    onLevelLoadSetupAuxTriggersCallbacks.clear();
};
