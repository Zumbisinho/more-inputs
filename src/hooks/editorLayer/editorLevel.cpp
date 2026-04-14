#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "../../utils/pickupManager.hpp"
#include "../../utils/getJson.hpp"

// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)


// TODO : Fix the start shit
using namespace geode::prelude;


class $modify(MyPlayLayer, LevelEditorLayer) {
    //void startGame() {
    //    LevelEditorLayer::startGame();
//
    //    auto& json = getConfig();
//
    //    int id = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
    //    int value = json["defaultModIdentityValue"].asInt().unwrapOr(0);
//
	//	log::info("{} {}",id,value);
    //    pickupManager::changePickupId(id, value);
	//	
    //}
	void onPlaytest() {
        LevelEditorLayer::onPlaytest();

        auto& json = getConfig();

        int id = json["defaultModIdentityPickupId"].asInt().unwrapOr(0);
        int value = json["defaultModIdentityValue"].asInt().unwrapOr(0);

		log::info("{} {}",id,value);
        pickupManager::changePickupId(id, value);
    }
};