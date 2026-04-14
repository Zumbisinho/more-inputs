#include "getJson.hpp"

using namespace geode::prelude;

matjson::Value& getConfig() {
    static matjson::Value json = [] {
        auto path = Mod::get()->getResourcesDir() / "constrains.json";
        return matjson::parse(file::readString(path).unwrapOr("{}"))
            .unwrapOr(matjson::Value());
    }();

    return json;
}