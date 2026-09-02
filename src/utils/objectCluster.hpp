#include <Geode/Geode.hpp>

using namespace geode::prelude;

#define setClusterSpr(x)                                \
    static inline const CCSprite m_sprite = &CCSprite::createWithSpriteFrameName(x)         \
        CCSprite* getSprite() const override {       \
        return &m_sprite;                               \
    }

struct CCPointButBetter {
    double x;
    double y;
    CCPointButBetter(double X, double Y) : x(X), y(Y) {};
    CCPointButBetter() = default;

    CCPointButBetter operator+=(const CCPoint &other) const {
        return CCPointButBetter{x + other.x, y + other.y};
    }
    CCPointButBetter operator-(const CCPointButBetter &other) const {
        return CCPointButBetter{x - other.x, y - other.y};
    }
    CCPointButBetter operator-=(const CCPointButBetter &other) const {
        return CCPointButBetter{x - other.x, y - other.y};
    }
    CCPointButBetter operator/(const int &other) const {
        return CCPointButBetter{x / other, y / other};
    }
};

inline std::optional<CCPointButBetter> getCenterCCPointButBetter(
    std::string_view objectString
) {
    for (auto &object : string::split(objectString, ";")) {
        auto values = string::split(object, ",");

        float x = 0.f;
        float y = 0.f;
        bool value34 = false;

        for (size_t i = 0; i + 1 < values.size(); i += 2) {
            if (values[i] == "2")
                x = numFromString<double>(values[i + 1]).unwrapOrDefault();

            else if (values[i] == "3")
                y = numFromString<double>(values[i + 1]).unwrapOrDefault();

            else if (values[i] == "34")
                value34 = numFromString<int>(values[i + 1]).unwrapOrDefault() != 0;
        }

        if (value34)
            return CCPointButBetter(x, y);
    }

    return std::nullopt;
}

inline std::vector<CCPointButBetter> getPositions(
    std::string_view objectString
) {
    std::vector<CCPointButBetter> result;

    for (auto &object : string::split(objectString, ";")) {
        auto values = string::split(object, ",");

        CCPointButBetter pos{};

        for (size_t i = 0; i + 1 < values.size(); i += 2) {
            if (values[i] == "2")
                pos.x = numFromString<double>(values[i + 1]).unwrapOrDefault();

            else if (values[i] == "3")
                pos.y = numFromString<double>(values[i + 1]).unwrapOrDefault();
        }

        result.push_back(pos);
    }

    return result;
}

inline std::string setPositions(
    std::string_view objectString,
    const std::vector<CCPointButBetter> &positions
) {
    auto objects = string::split(objectString, ";");

    for (size_t obj = 0; obj < objects.size() && obj < positions.size(); ++obj) {
        auto values = string::split(objects[obj], ",");

        for (size_t i = 0; i + 1 < values.size(); i += 2) {
            if (values[i] == "2")
                values[i + 1] = std::to_string(positions[obj].x);

            else if (values[i] == "3")
                values[i + 1] = std::to_string(positions[obj].y);
        }

        objects[obj] = string::join(values, ",");
    }

    return string::join(objects, ";");
}
template <typename T>
struct minMax {
    T min;
    T max;
    minMax<T>(T &v1, T &v2) : min(v1), max(v2){};
};

template <typename T>
inline minMax<T> minAndMax(T &v1, T &v2) {
    auto max = std::max({v1, v2});
    if (max == v1) {
        return minMax{v2, v1};
    } else {
        return minMax{v1, v2};
    };
}

namespace ObjectClusterAPI {

class objectCluster : public CCObject {
public:
    int uniqueId;
    virtual CCSprite* getSprite() const {
        return &m_sprite;
    }

protected:
    static inline CCSprite m_sprite;
    std::string m_relativeObjectStr;
    CCSize m_clusterSize;
    CCPointButBetter m_clusterCenter;

    bool init(std::string_view string) {
        auto center = getCenterCCPointButBetter(string);
        if (center)
            m_clusterCenter = center.value();

        CCPointButBetter min_point;
        CCPointButBetter max_point;
        auto positions = getPositions(string);
        for (auto &objPos : positions) {
            min_point = {minAndMax(min_point.x, objPos.x).min, minAndMax(min_point.y, objPos.y).min};
            max_point = {minAndMax(max_point.x, objPos.x).max, minAndMax(max_point.y, objPos.y).max};
        };
        m_clusterSize = CCSize{static_cast<float>(max_point.x - min_point.x), static_cast<float>(max_point.y - min_point.y)};
        if (!center) {
            m_clusterCenter = {(max_point - min_point) / 2};
        }
        // absolute to relative
        for (auto &objPos : positions) {
            objPos -= m_clusterCenter;
        };
        setPositions(m_relativeObjectStr, positions);
        return true;
    };

public:
    static objectCluster *create(std::string_view string) {
        objectCluster *pRet = new objectCluster();
        if (pRet && pRet->init(string)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = 0;
            return nullptr;
        }
    }
    void placeOn(CCPoint pos) {
        auto posAbs = getPositions(m_relativeObjectStr);
        for (auto &obj : posAbs) {
            obj += pos;
        };
        std::string dummy;
        setPositions(dummy, posAbs);
        LevelEditorLayer::get()->createObjectsFromString(dummy, false, true);
    }
};

inline std::map<int, objectCluster *> m_objectClusters; // uniqueId - obj
inline int curObjRegIdx;

static void registerObjectCluster(objectCluster &obj);

}; // namespace ObjectClusterAPI