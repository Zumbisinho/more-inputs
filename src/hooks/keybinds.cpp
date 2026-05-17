#include "../utils/keybindsCache.hpp"
#include "../utils/keycodeToString.hpp"
#include "../utils/pickupManager.hpp"
#include "Geode/DefaultInclude.hpp"
#include "Geode/loader/Event.hpp"
#include "Geode/loader/Log.hpp"
#include "Geode/utils/Keyboard.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/GJBaseGameLayer.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>


$execute {
    KeyboardInputEvent()
        .listen(+[](const geode::KeyboardInputData &event) {
            if (event.action == KeyboardInputData::Action::Repeat) {
                log::info("ta repidindo");
                return geode::ListenerResult::Propagate;
            };
            auto editorLayer = LevelEditorLayer::get();
            if (editorLayer)
                if (editorLayer->m_playbackMode != PlaybackMode::Playing)
                    return geode::ListenerResult::Propagate;
            if (!GJBaseGameLayer::get()) {
                return geode::ListenerResult::Propagate;
            };
            auto layer = !GJBaseGameLayer::get();
            int keyAsInt = static_cast<int>(event.key);
            auto actions = KeybindCache::keyToActionIds.find(keyAsInt);
            // geode::log::info("actions: {}, ta dentro?
            // {}",KeybindCache::keyToActionIds,
            // KeybindCache::keyToActionIds.contains(keyAsInt));
            if (KeybindCache::keybinds.contains(keyAsInt)) {
                for (const int actionID : actions->second) {
                    geode::log::info(
                        "Acionando na key: {}, actionId: {}, taSolto {}",
                        keyToString(keyAsInt),
                        actionID,
                        event.action != KeyboardInputData::Action::Release
                    );
                    Loader::get()->queueInMainThread([=] {
                        pickupManager::changePickupId(
                            actionID,
                            event.action != KeyboardInputData::Action::Release
                                ? KeybindCache::value
                                : 0
                        );
                    });
                }
            }
            return geode::ListenerResult::Propagate;
        },67)
        .leak();
    
}
