#pragma once

#include "Geode/loader/Log.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <matjson.hpp>
#include <string>

#include <unordered_set>
#include <utility>

using namespace geode::prelude;

namespace keybindsAPI {
// ? helper functions

struct MobileKey {
    bool isSpr;
    CCPoint pos;
    CCSize contentSize;
    std::string buttonLabel;

    MobileKey(
        std::string buttonLabel, bool isSpr, CCPoint pos, CCSize contentSize
    )
        : buttonLabel(buttonLabel), isSpr(isSpr), pos(pos),
          contentSize(contentSize) {};
};

struct KeybindValue {

private:
    // ? helper functions
    
    using JSONArray = std::vector<matjson::Value>;
    template <typename returnType>
    returnType getOr(matjson::Value json, returnType defaultValue) {
        return json.as<returnType>().unwrapOr(defaultValue);
    };
    template <typename returnType>
    std::vector<returnType>
    getArray(matjson::Value jsonArray, JSONArray defaultValue) {
        auto fixedArray = getOr<JSONArray>(jsonArray, JSONArray{defaultValue});
        std::vector<returnType> toReturn;

        for (auto item : fixedArray) {
            auto result = item.as<returnType>();
            if (result.isOk())
                toReturn.push_back(result.unwrap());
        };
        if (toReturn.size() == 0){
            toReturn.push_back(defaultValue[0].as<returnType>().unwrapOrDefault());
            toReturn.push_back(defaultValue[1].as<returnType>().unwrapOrDefault());
        }
        return toReturn;
    };

public:
    std::string name;
    int keyCode;
    bool isSpr;
    CCPoint pos;
    CCSize contentSize;
    std::string buttonLabel;

    KeybindValue(matjson::Value keySettings) {
        JSONArray defaultArray = {0, 0};
        name = getOr<std::string>(keySettings["name"], "name");

        keyCode = getOr(keySettings["keyCode"], -67);

        buttonLabel = getOr<std::string>(keySettings["buttonLabel"], "name");
        isSpr = getOr(keySettings["isSpr"], false);

        auto positionArray =
            getArray<float>(keySettings["position"], defaultArray);
        pos = CCPoint{positionArray[0], positionArray[1]};

        auto contentSizeArray =
            getArray<float>(keySettings["contentSize"], defaultArray);
        contentSize = CCSize{contentSizeArray[0], contentSizeArray[1]};
    };
    // ? Debug
    //~KeybindValue(){
    //    log::info("Object Killed: {}",this->name);
    //    //__debugbreak(); 
    //};
    static KeybindValue parse(std::string name, int keyCode, bool isSpr, CCPoint pos, CCSize contentSize, std::string buttonLabel) {
        auto dummy = matjson::Value::object();
        auto positionArray = matjson::Value::array();
        auto sizeArray = matjson::Value::array();

        positionArray.push(pos.x);
        positionArray.push(pos.y);

        sizeArray.push(contentSize.width);
        sizeArray.push(contentSize.height);

        dummy["name"] = name;
        dummy["keyCode"] = keyCode;
        dummy["isSpr"] = isSpr;
        dummy["position"] = positionArray;
        dummy["contentSize"] = sizeArray;
        dummy["buttonLabel"] = buttonLabel;
        return KeybindValue{dummy};
    };
};
// ? helper functions
using KeyFullSettings = std::pair<int, const KeybindValue>;
inline keybindsAPI::KeybindValue createPcValue(std::string name, int keyCode) {
    return keybindsAPI::KeybindValue::parse(name, keyCode, false, ccp(0, 0), CCSize{50, 50}, name);
};


std::unordered_set<int> getLevelKeyBindsRaw(CCLayer *layer);
std::vector<std::pair<std::string, int>> getLevelKeyBinds(CCLayer *layer, bool ignoreEmpty);
std::vector<KeyFullSettings> getLevelKeySettings(CCLayer *layer);

void addLevelKeyBind(LevelEditorLayer *layer, KeybindValue *keySettings);
void editLevelKeyBind(
    LevelEditorLayer *layer,
    int actionID,
    const KeybindValue *newKeySettings
);
void deleteLevelKeybind(LevelEditorLayer *layer, int actionID);

void deleteKeybindsFromLevel(LevelEditorLayer *layer);
} // namespace keybindsAPI

namespace matjson {

template <>
struct Serialize<keybindsAPI::KeybindValue> {
    static Result<keybindsAPI::KeybindValue> fromJson(Value const& value) {
        return Ok(keybindsAPI::KeybindValue(value));
    }

    static Value toJson(keybindsAPI::KeybindValue const& value) {
        auto json = Value::object();

        auto pos = Value::array();
        pos.push(value.pos.x);
        pos.push(value.pos.y);

        auto size = Value::array();
        size.push(value.contentSize.width);
        size.push(value.contentSize.height);

        json["name"] = value.name;
        json["keyCode"] = value.keyCode;
        json["isSpr"] = value.isSpr;
        json["pos"] = pos;
        json["contentSize"] = size;
        json["buttonLabel"] = value.buttonLabel;

        return json;
    }
};

}