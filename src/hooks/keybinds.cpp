#include "../utils/keybindsCache.hpp"
#include "../utils/keycodeToString.hpp"
#include "../utils/pickupManager.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include "Geode/loader/Log.hpp"
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
    bool
    dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double dt) {
        static bool s_processing = false;
       
        if (!isSafeKey(key))
            return false;

        if (s_processing || repeat || KeybindCache::keybinds.empty())
            return false;

        s_processing = true;

        auto playLayer = PlayLayer::get();
        auto editorLayer = LevelEditorLayer::get();

        if (playLayer || editorLayer) {
            if (playLayer) {
                if (!playLayer->m_player1 || playLayer->m_isPaused ||
                    playLayer->m_playerDied) {
                    s_processing = false;
                    return CCKeyboardDispatcher::dispatchKeyboardMSG(
                        key, down, repeat, dt
                    );
                }
            }
            if (editorLayer) {
                if (editorLayer->m_playbackMode == PlaybackMode::Not) {
                    geode::log::info(
                        "I dont wanna on the editor, get out of my way!"
                    );
                    s_processing = false;
                    return CCKeyboardDispatcher::dispatchKeyboardMSG(
                        key, down, repeat, dt
                    );
                }
            }
            int keyInt = static_cast<int>(key);

            if (KeybindCache::keybinds.contains(keyInt)) {
                auto it = KeybindCache::keyToActionIds.find(keyInt);
                if (it != KeybindCache::keyToActionIds.end()) {
                    if (down) // self-explaning
                        for (const int actionID : it->second) {
                            Loader::get()->queueInMainThread([=] {
                                pickupManager::changePickupId(
                                    actionID, KeybindCache::value
                                );
                            });
                        };
                    if (!down) // On upkey event
                        for (const int actionID : it->second) {
                            Loader::get()->queueInMainThread([=] {
                                pickupManager::changePickupId(actionID, 0);
                            });
                        };
                }
            }
        }

        auto result =
            CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, dt);

        s_processing = false;
        return result;
    }
};