#pragma once


#include <Geode/Geode.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <matjson.hpp>
#include <string>

#include <unordered_set>
#include <utility>

using namespace geode::prelude;

namespace keybindsAPI {
// ? helper functions

inline int curButtonIdx = 0;
VersionInfo getLevelVersion(CCLayer *layer);

inline void resetCurButtonIdx() {
    curButtonIdx = 0;
};

struct MatJsonUtils {
protected:
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
        if (toReturn.size() == 0) {
            for (auto& value: defaultValue){
                toReturn.push_back(value.as<returnType>().unwrapOrDefault());
            }
        }
        return toReturn;
    };
    JSONArray defaultArray = {0, 0};

};

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

struct MobileKeySprProps : MatJsonUtils{
public:
    ccColor3B color;
    float rotation;
    CCPoint offset;
    CCSize scale;
    bool invertedX;
    bool invertedY;

    MobileKeySprProps() = default;

    MobileKeySprProps(matjson::Value props){
        auto colorArray = getArray<GLubyte>(props["color"],{ 255, 255, 255 });
        color = ccc3(colorArray[0],colorArray[1],colorArray[2]);

        rotation = getOr<float>(props["rotation"], 0);

        auto positionArray = getArray<float>(props["offset"], defaultArray);
        offset = CCPoint{positionArray[0], positionArray[1]};

        auto contentSizeArray = getArray<float>(props["scale"], {1,1});
        scale = CCSize{contentSizeArray[0], contentSizeArray[1]};

        invertedX = getOr<bool>(props["invertedX"], false);

        invertedY = getOr<bool>(props["invertedY"], false);
    };
    static MobileKeySprProps parse(ccColor3B color,float rotation,CCPoint offset,CCSize scale,bool invertedX,bool invertedY) {
        auto dummy = matjson::Value::object();
        auto colorArray = matjson::Value::array();
        auto positionArray = matjson::Value::array();
        auto sizeArray = matjson::Value::array();

        colorArray.push(color.r);
        colorArray.push(color.g);
        colorArray.push(color.b);

        positionArray.push(offset.x);
        positionArray.push(offset.y);

        sizeArray.push(scale.width);
        sizeArray.push(scale.height);

        dummy["color"] = colorArray;
        dummy["rotation"] = rotation;
        dummy["offset"] = positionArray;
        dummy["scale"] = sizeArray;
        dummy["invertedX"] = invertedX;
        dummy["invertedY"] = invertedY;
        return MobileKeySprProps{dummy};
    };
    static MobileKeySprProps getDefault(){
        return parse({255,255,255},0,{0,0},{1,1},false,false);
    }
};

struct KeybindValue : MatJsonUtils {
public:
    std::string name;
    int keyCode;
    bool isSpr;
    CCPoint pos;
    CCSize contentSize;
    std::string buttonLabel;
    MobileKeySprProps mobileKeySprProps;

    KeybindValue() = default;

    KeybindValue(matjson::Value keySettings) {
        name = getOr<std::string>(keySettings["name"], "name");

        keyCode = getOr(keySettings["keyCode"], -67);

        buttonLabel = getOr<std::string>(keySettings["buttonLabel"], "name");
        isSpr = getOr(keySettings["isSpr"], false);

        auto positionArray = getArray<float>(keySettings["pos"], defaultArray);
        pos = CCPoint{positionArray[0], positionArray[1]};

        auto contentSizeArray = getArray<float>(keySettings["contentSize"], defaultArray);
        contentSize = CCSize{contentSizeArray[0], contentSizeArray[1]};
 
        mobileKeySprProps = getOr<MobileKeySprProps>(keySettings["mobileKeySprProps"],MobileKeySprProps::getDefault());
    };
    static KeybindValue parse(std::string name, int keyCode, bool isSpr, CCPoint pos, CCSize contentSize, std::string buttonLabel,MobileKeySprProps mksp);
};


// ? helper functions
using KeyFullSettings = std::pair<int, KeybindValue>;
// just convert from numeric Position to a ratio from the screen size
inline CCPoint absToRel(float x, float y) {
    auto screenSize = CCDirector::sharedDirector()->getWinSize();
    return ccp(x / screenSize.width, y / screenSize.height);
};

keybindsAPI::KeybindValue createPcValue(std::string name, int keyCode,int& index);

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

void convertLevelKeybinds(LevelEditorLayer *layer);
} // namespace keybindsAPI



namespace matjson {


template <>
struct Serialize<keybindsAPI::MobileKeySprProps> {
    static Result<keybindsAPI::MobileKeySprProps> fromJson(Value const &value) {
        return Ok(keybindsAPI::MobileKeySprProps(value));
    }

    static Value toJson(keybindsAPI::MobileKeySprProps const &value) {
        auto json = Value::object();

        auto color = Value::array();
        color.push(value.color.r);
        color.push(value.color.g);
        color.push(value.color.b);

        auto offset = Value::array();
        offset.push(value.offset.x);
        offset.push(value.offset.y);

        auto scale = Value::array();
        scale.push(value.scale.width);
        scale.push(value.scale.height);

        json["color"] = color;
        json["rotation"] = value.rotation;
        json["offset"] = offset;
        json["scale"] = scale;
        json["invertedX"] = value.invertedX;
        json["invertedY"] = value.invertedY;

        return json;
    }
};


template <>
struct Serialize<keybindsAPI::KeybindValue> {
    static Result<keybindsAPI::KeybindValue> fromJson(Value const &value) {
        return Ok(keybindsAPI::KeybindValue(value));
    }

    static Value toJson(keybindsAPI::KeybindValue const &value) {
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
        json["mobileKeySprProps"] = value.mobileKeySprProps;

        return json;
    }
};

} // namespace matjson