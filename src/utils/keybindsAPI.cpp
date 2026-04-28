#include "keybindsAPI.hpp"

namespace keybindsAPI
{
    std::unordered_set<int> getLevelKeyBindsRaw()
    {
        std::unordered_set<int> testSet = {81, 82, 69};
        return testSet;
    }

    std::unordered_map<std::string, int> getLevelKeyBinds()
    {
        std::unordered_map<std::string, int> keys;
        keys["jump"] = KEY_Space;
        keys["attack"] = KEY_E;
        keys["fruck"] = KEY_F;
        keys["farias"] = KEY_Z;
        keys["frida"] = KEY_C;
        keys["cauHD"] = KEY_H;
        return keys;
    }


    //std::unordered_map<std::string, int> getLevelKeyBinds()
    //{
//
    //}

    // @param json: must be in {
    //  "name" : keycode,
    //  ...
    //}
    void setLevelKeyBinds(LevelEditorLayer *layer, matjson::Value *json)
    {
        alpha::level_storage::setSavedValue(layer, "keybinds", *json);
    }
};
