#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/Geode.hpp>
#include "../utils/getJson.hpp"
#include "../utils/getKeysFromCurrentLevel.hpp"
#include "../utils/pickupManager.hpp"

using namespace geode::prelude;

    //"startKeyPickupId" : int,
    //"defaultModIdentityPickupId" : int,
    //"defaultModIdentityValue" : int,
    //"keyPressedValue" : int,
    //"keyReleasedValue" : int

class $modify(MyKeyboard, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat,double dt) {
        auto& json = getConfig();
        int keyInt = static_cast<int>(key);
        log::info("{} | {} | {}",keyInt,down,repeat);
        
        int startId = json["startKeyPickupId"].asInt().unwrapOr(0);
        int value = json["keyPressedValue"].asInt().unwrapOr(0);

        auto keybinds = getLevelKeyBinds();

        if (down) {
            if (keybinds.contains(key)) {
                pickupManager::changePickupId(startId + keyInt,value);
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat,dt);
    }
};