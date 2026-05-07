#include "../utils/keybindsCache.hpp"
#include "../utils/keycodeToString.hpp"
#include "../utils/pickupManager.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

class $modify(MyKeyboard, CCKeyboardDispatcher) {
    bool isSafeKey(enumKeyCodes key) {
        switch (key) {
        case KEY_LeftWindowsKey:
        case KEY_RightWindowsKey:
            return false;
        default:
            return true;
        }
    }
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat,
                             double dt) {
        static bool s_processing = false;

        if (s_processing)
            return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat,
                                                             dt);

        if (!isSafeKey(key))
            return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat,
                                                             dt);

        s_processing = true;

        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        if (playLayer || editorLayer) {
            if (playLayer) {
                if (!playLayer->m_player1 || playLayer->m_isPaused ||
                    playLayer->m_playerDied) {
                    s_processing = false;
                    return CCKeyboardDispatcher::dispatchKeyboardMSG(
                        key, down, repeat, dt);
                }
            }

            int keyInt = static_cast<int>(key);
            log::info("Before dispatch: key={} down={} repeat={} dt={}", keyInt, down, repeat, dt);
            if (down && KeybindCache::keybinds.contains(keyInt)) {
                auto it = KeybindCache::keyToActionIds.find(keyInt);
                if (it != KeybindCache::keyToActionIds.end()) {
                    for (const int actionID : it->second) {
                        pickupManager::changePickupId(actionID,
                                                      KeybindCache::value);
                    }
                }
            }
        }

        auto result =
            CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, dt);

        s_processing = false;
        return result;
    }
};