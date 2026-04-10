#include <Geode/Geode.hpp>
#include <Geode/modify/GJEffectManager.hpp>

// TODO Encode keysbinds via some shit with object encoding (Or just lazy json on text label)

using namespace geode::prelude;

class $modify(MyPlayLayer, GJEffectManager)
{
	bool init(){
		if (!GJEffectManager::init())
            return false;
		
		this->addEventListener(
			KeybindSettingPressedEventV3(Mod::get(),"test-key"),[this](Keybind const& keybind, bool down, bool repeat, double timestamp)
			{
				if (down && !repeat)
				{
					auto manager = this; 
					manager->addCountToItem(3, 1); 
					
				}
			}
		);
		return true;
		
	};
};