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
        int m_defaultModIdentityPickupId = 0;
        int m_defaultModIdentityValue = 0; // Custom field

        ~Fields(){ // when leaves a level
            KeybindCache::reset();
        }
    };
    bool init(GJGameLevel* level, bool noUI) {
        if (!LevelEditorLayer::init(level, noUI)) return false;
        auto& json = getConfig();

        if(!KeybindCache::initialized)
            KeybindCache::init(this);
        m_fields->m_defaultModIdentityPickupId = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
        m_fields->m_defaultModIdentityValue = json["defaultModIdentityValue"].asInt().unwrapOr(0);
        return true;
    }
    
	void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        int id = m_fields->m_defaultModIdentityPickupId;
        int value = m_fields->m_defaultModIdentityValue;

        KeybindCache::init(this);
		log::info("{} {}",id,value);
        pickupManager::changePickupId(id,value);
    }
    
};