#include "keybindsCache.hpp"
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

void insertOrUpdate(std::vector<std::pair<std::string, int>> &keybindsAndAction,
                    const std::string &actionName, int newValue) {
    auto it = std::ranges::find(keybindsAndAction, actionName,
                                &std::pair<std::string, int>::first);

    if (it != keybindsAndAction.end()) {

        it->second = newValue;
    } else {

        keybindsAndAction.emplace_back(actionName, newValue);
    }
}

void init(CCLayer *layer) {
    auto &json = getConfig();
    // Reset all Variables for safe integration with keybindsAPI
    keyToActionIds.clear();
    actionNameToID.clear();
    keybinds.clear();
    keybindsAndAction.clear();

    keybinds = keybindsAPI::getLevelKeyBindsRaw(layer);
    keybinds.erase(-67);
    keybindsAndAction = keybindsAPI::getLevelKeyBinds(layer,false);

    startId = json["startKeyPickupId"].asInt().unwrapOr(0);
    value = json["keyPressedValue"].asInt().unwrapOr(0);
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

};
void changeLocalKey(std::string actionName, int newKeyCode) {
    
    int oldKeyCode = std::ranges::find(keybindsAndAction, actionName,
                                       &std::pair<std::string, int>::first)
                         ->second;
    int actionID = actionNameToID[actionName];
    if (keyToActionIds[oldKeyCode].size() ==
        1) // if more that 1 action, we need just to remove the actionId, else,
           // remove from keybinds and delete the pair
    {
        keybinds.erase(oldKeyCode);
        keyToActionIds.erase(oldKeyCode);
    } else {

        std::erase(keyToActionIds[oldKeyCode], actionID);
    }
    keybinds.insert(newKeyCode);
    keyToActionIds[newKeyCode].push_back(actionID);
    insertOrUpdate(keybindsAndAction,actionName,newKeyCode);
   
};

} // namespace KeybindCache
