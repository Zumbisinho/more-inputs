#include "mainLevel.hpp"
// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label Exact what i'm gonna do!)

// TODO : Fix the start shit
using namespace geode::prelude;

bool MyPlayLayer::init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
    if (!PlayLayer::init(level, useReplay, dontCreateObjects))
        return false;
    auto &json = getConfig();

    m_fields->m_defaultModIdentityPickupId = json["defaultModIdentityPickupId"].asInt().unwrapOr(-1);
    m_fields->m_defaultModIdentityValue = json["defaultModIdentityValue"].asInt().unwrapOr(-1);

    int id = m_fields->m_defaultModIdentityPickupId;
    int value = m_fields->m_defaultModIdentityValue;
    pickupManager::changePickupId(id, value);

    auto curVersion = Mod::get()->getVersion();
    auto version = keybindsAPI::getLevelVersion(this);
    if (curVersion == version) {
        m_fields->showPopUp = true;
    };

    return true;
}
void MyPlayLayer::showPopup(float) {
    UILayer::get()->onPause(nullptr);
    PopupManager::get().quickPopup(
        "Outdated ass Client",
        "This level was built on a newer version, it <cr>Could cause a crash or unwanted behavior</c>\n\n"
        "Do you realy <cg>want to play this level</c> without <cc>updating More Inputs?</c>\n"
        "<cr>WARNING: Clicking No makes you quit the level!</c>",
        "No",
        "Yes",
        [this](auto* popup, bool btn2) {
            if (!btn2) {
                onQuit();
            }
        }
    ).showQueue();
};
void MyPlayLayer::startGame() {
    PlayLayer::startGame();
    if (m_fields->m_defaultModIdentityPickupId == -1 || m_fields->m_defaultModIdentityValue == -1)
        return;
    int id = m_fields->m_defaultModIdentityPickupId;
    int value = m_fields->m_defaultModIdentityValue;
    pickupManager::changePickupId(id, value);
    if (m_fields->showPopUp) {
        scheduleOnce(schedule_selector(MyPlayLayer::showPopup), 0.01);

        m_fields->showPopUp = false;
    }
}
void MyPlayLayer::resetLevel() {
    PlayLayer::resetLevel();

    int id = m_fields->m_defaultModIdentityPickupId;
    int value = m_fields->m_defaultModIdentityValue;

    pickupManager::changePickupId(id, value);
};
void MyPlayLayer::onQuit() {
    PlayLayer::onQuit();
    KeybindCache::reset();
}

;