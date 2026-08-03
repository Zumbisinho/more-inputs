#include "customTriggers.hpp"
#include "Geode/loader/Log.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

namespace customTriggers {

void addCustomTrigger(std::unique_ptr<CustomTrigger> trigger, geode::Mod *mod) {
    auto modName = static_cast<std::string>(mod->getID());
    auto &triggers = customTriggersPerMod[modName];
    for (auto const &addedTrigger : triggers) {
        if (addedTrigger->customObjID == trigger->customObjID) {
            auto desc = fmt::format(
                "A error has occured, you cannot assign the same CustomObjectId\n"
                "Error on Classes {} and {}\n"
                "To fix it just change the Id",
                typeid(*trigger).name(),
                typeid(*addedTrigger).name()
            );

            auto popup = createQuickPopup(
                "Error",
                desc,
                "EXIT AND NOT SAVE",
                "EXIT AND SAVE",
                [](auto, bool bt2) {
                    utils::game::exit(bt2);
                }
            );

            CCDirector::sharedDirector()
                ->getRunningScene()
                ->addChild(popup);

            return;
        }
    }

    triggers.push_back(std::move(trigger));
}

} // namespace customTriggers

class $modify(Idk, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        customTriggers::addCustomTrigger(std::move(customTriggers::TouchMacroTrigger::create()));

        for (const auto &[key, value] : customTriggers::customTriggersPerMod) {
            log::info("ModName: {} : {}", key, value.size());
            for (const auto &trigger : value) {
                geode::log::info("    TriggerId: {}, className: {}", trigger->customObjID, typeid(*trigger).name());
            }
        }
        return true;
    }
};