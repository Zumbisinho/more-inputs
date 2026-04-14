#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace pickupManager {
    void changePickupId(int pickupId, int value);
    int getPickupValue(int pickupId);
}