#include "getJson.hpp"
#include "keybindsAPI.hpp"
#include "keybindsCache.hpp"

namespace KeybindCache {
    bool initialized = false;
    int startId = 0;
    int value = 0;
    std::unordered_set<int> keybinds;

    void init() {
        auto& json = getConfig();

        keybinds = keybindsAPI::getLevelKeyBindsRaw();
        startId = json["startKeyPickupId"].asInt().unwrapOr(0);
        value = json["keyPressedValue"].asInt().unwrapOr(0);

        initialized = true;
    }

    void reset() {
        initialized = false;
    }
}