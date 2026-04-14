#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../utils/pickupManager.hpp"
#include "../../utils/getJson.hpp"
#include <matjson.hpp>
// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)


// TODO : Fix the start shit
using namespace geode::prelude;


class $modify(MyPlayLayer, PlayLayer) {
    void startGame() {
        PlayLayer::startGame();

        auto& json = getConfig();

        int id = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
        int value = json["defaultModIdentityValue"].asInt().unwrapOr(0);

		log::info("{} {}",id,value);
        pickupManager::changePickupId(id, value);
		
    }
	void resetLevel() {
        PlayLayer::resetLevel();

        auto& json = getConfig();

        int id = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
        int value = json["defaultModIdentityValue"].asInt().unwrapOr(0);

		log::info("{} {}",id,value);
        pickupManager::changePickupId(id, value);
    }
};