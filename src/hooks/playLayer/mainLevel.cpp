#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../utils/pickupManager.hpp"
#include "../../utils/getJson.hpp"
#include "../../utils/keybindsCache.hpp"
#include <matjson.hpp>
// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)


// TODO : Fix the start shit
using namespace geode::prelude;


class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        int m_defaultModIdentityPickupId = 0;
        int m_defaultModIdentityValue = 0; // Custom field
    };
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        auto& json = getConfig();

        m_fields->m_defaultModIdentityPickupId = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
        m_fields->m_defaultModIdentityValue = json["defaultModIdentityValue"].asInt().unwrapOr(0);

        return true;
    }
    void startGame() {
        PlayLayer::startGame();

        int id = m_fields->m_defaultModIdentityPickupId;
        int value = m_fields->m_defaultModIdentityValue;
        KeybindCache::init(this);
		log::info("{} {}",id,value);
        pickupManager::changePickupId(id,value);
		
    }
	void resetLevel() {
        PlayLayer::resetLevel();

		int id = m_fields->m_defaultModIdentityPickupId;
        int value = m_fields->m_defaultModIdentityValue;

        
		log::info("{} {}",id,value);
        pickupManager::changePickupId(id,value);
    }
    
};