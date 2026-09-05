#include "keybindsCache.hpp"
#include "keybindsAPI.hpp"
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

// TODO : Mudar de keyCode para ActionID, e guardam com um orderedmap<int,
// std::vector<int>>, onde cada keyCode tem seus respectivos ActionId, tipo,
// pular e voar são espaço, ent ao clicar espaço, é pego dessa variavel dois
// canais, 3402 e 3403, que são alterados

namespace KeybindCache {

bool initialized = false;
int startId = 0;
int value = 0;
template<typename T, typename Predicate>
void insertOrUpdate(
    std::vector<T>& vect,
    Predicate predicate,
    const T& value
) {
    auto it = std::ranges::find_if(vect, predicate);

    if (it != vect.end())
        *it = value;
    else
        vect.push_back(value);
}

void init(CCLayer *layer) {

    auto &json = getConfig();
    // Reset all Variables for safe integration with keybindsAPI
    keyToActionIds.clear();
    actionNameToID.clear();
    keybinds.clear();
    keybindsAndAction.clear();
    keySettings.clear();

    startId = json["startKeyPickupId"].asInt().unwrapOr(0);
    value = json["keyPressedValue"].asInt().unwrapOr(0);
    
    keybinds = keybindsAPI::getLevelKeyBindsRaw(layer);
    keybinds.erase(-67);
    keybindsAndAction = keybindsAPI::getLevelKeyBinds(layer,false);
    keySettings = keybindsAPI::getLevelKeySettings(layer);
    
    int index = 0;
    for (const auto &[key, value] : keybindsAndAction) {
        index++;
        if (value == -67)
            continue;
        keyToActionIds[value].push_back(startId + index);
        actionNameToID[key] = startId + index;
        
    };

    initialized = true;
    
};

void reset() {
    initialized = false;
    keyToActionIds.clear();
    actionNameToID.clear();
    keybinds.clear();
    keybindsAndAction.clear();
    keySettings.clear();
    mobileKeyNodes.clear();
    mobileKeysToHideOnInit.clear();

};
void changeLocalKey(const keybindsAPI::KeyFullSettings& key, int keyCode) {
    auto actionId = key.first;

    int oldKeyCode = -67;
    

    for (auto& keySetting : keySettings) {

        if (keySetting.first == actionId) {
            oldKeyCode = keySetting.second.keyCode;
            break;
        }
    }


    
    if (oldKeyCode == -67)
        return;
    if (keyToActionIds[oldKeyCode].size() == 1) {
        keybinds.erase(oldKeyCode);
        keyToActionIds.erase(oldKeyCode);
    } else {
        std::erase(keyToActionIds[oldKeyCode], actionId);
    }

    keybinds.insert(keyCode);
    keyToActionIds[keyCode].push_back(actionId);

    auto newKey = key;
    newKey.second.keyCode = keyCode;

    insertOrUpdate(
    keySettings,
    [&](const auto& keySetting) {
        return keySetting.first == actionId;
    },
    newKey
    );
}

} // namespace KeybindCache
