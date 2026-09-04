#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/PopupManager.hpp>
#include "../../utils/pickupManager.hpp"
#include "../../utils/getJson.hpp"
#include "../../utils/keybindsCache.hpp"
#include "../../utils/keybindsAPI.hpp"
#include <matjson.hpp>
// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)


// TODO : Fix the start shit
using namespace geode::prelude;


class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        int m_defaultModIdentityPickupId = 0;
        int m_defaultModIdentityValue = 0; // Custom field
        bool showPopUp = false;
    };
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects);
    void showPopup(float);
    void startGame();
	void resetLevel();
    void onQuit();
};
