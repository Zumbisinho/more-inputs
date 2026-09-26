#include "pickupManager.hpp"

using namespace geode::prelude;

namespace pickupManager {


    GJEffectManager* getEffectManager() {
        if (auto pl = PlayLayer::get())
            return pl->m_effectManager;

        if (auto editor = LevelEditorLayer::get())
            return editor->m_effectManager;

        return nullptr;
    }

    void changePickupId(int pickupId, int value) {
        if (auto em = getEffectManager()) {
            // reset
            em->addCountToItem(pickupId, -em->countForItem(pickupId));

            // set value
            em->addCountToItem(pickupId, value);
        }
    }

    int getPickupValue(int pickupId) {
        if (auto em = getEffectManager()) {
            return em->countForItem(pickupId);
        }

        return 0; 
    }

}