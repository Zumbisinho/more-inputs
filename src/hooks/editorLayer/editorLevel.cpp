#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "../../utils/pickupManager.hpp"
#include "../../utils/getJson.hpp"
#include "../../utils/keybindsCache.hpp"

// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)


// TODO : Fix the start shit
using namespace geode::prelude;


class $modify(MyPlayLayer, LevelEditorLayer) {
    struct Fields {
        ~Fields(){ // when leaves a level
            KeybindCache::reset();
        }
    };
    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;

        KeybindCache::reset();
        KeybindCache::init(this);

        return true;
    }
    
	void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        KeybindCache::init(this);
    }
    
};