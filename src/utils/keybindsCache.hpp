#pragma once
#include <unordered_set>

namespace KeybindCache {
    extern bool initialized;
    extern int startId;
    extern int value;
    extern std::unordered_set<int> keybinds;

    void init();
    void reset();
}