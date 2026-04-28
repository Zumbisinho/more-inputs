#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/Geode.hpp>
#include "../utils/pickupManager.hpp"
#include "../utils/keybindsCache.hpp"
#include "../utils/keycodeToString.hpp"



using namespace geode::prelude;

    //"startKeyPickupId" : int,
    //"defaultModIdentityPickupId" : int,
    //"defaultModIdentityValue" : int,
    //"keyPressedValue" : int,
    //"keyReleasedValue" : int




class $modify(MyKeyboard, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat,double dt) {
        if (!KeybindCache::initialized) {
            KeybindCache::init();
        }   
        int keyInt = static_cast<int>(key);
        
        log::info("{} | {} | {} | {}",keyInt,down,repeat,keyToString(key));

        if (down) {
            if (KeybindCache::keybinds.contains(key)) {
                pickupManager::changePickupId(KeybindCache::startId + keyInt,KeybindCache::value);
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat,dt);
    }
};