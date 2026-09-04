#include <Geode/Geode.hpp>

using namespace geode::prelude;



struct CCPointButBetter {
    double x;
    double y;
    CCPointButBetter(double X, double Y) : x(X), y(Y) {};
    CCPointButBetter() = default;

    CCPointButBetter operator+(const CCPointButBetter &other) const {
        return CCPointButBetter{x + other.x, y + other.y};
    }

    CCPointButBetter operator+(const CCPoint &other) const {
        return CCPointButBetter{x + other.x, y + other.y};
    }
    CCPointButBetter operator+=(const CCPointButBetter &other)  {
        this->x += other.x;
        this->y += other.y;
        return CCPointButBetter{x + other.x, y + other.y};
    }
    CCPointButBetter operator+=(const CCPoint &other)  {
        this->x += other.x;
        this->y += other.y;
        return CCPointButBetter{x + other.x, y + other.y};
    }
    CCPointButBetter operator-(const CCPointButBetter &other) const {
        return CCPointButBetter{x - other.x, y - other.y};
    }
    CCPointButBetter operator-=(const CCPointButBetter &other) {
        this->x -= other.x;
        this->y -= other.y;
        return CCPointButBetter{x - other.x, y - other.y};
    }
    CCPointButBetter operator/(const int &other) const {
        return CCPointButBetter{x / other, y / other};
    }
    
};
inline auto format_as(CCPointButBetter c) -> std::string{
    return fmt::format("{} {}",c.x, c.y);
}

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

        if (value34) {
            log::warn("Achei porra {} {}",x,y);
            return CCPointButBetter(x, y);
        }
    }

    return std::nullopt;
}

inline std::vector<CCPointButBetter> getPositions(std::string_view objectString) {
    std::vector<CCPointButBetter> result;

    for (auto object : string::split(objectString, ";")) {
        if (object.empty())
            continue;

        auto values = string::split(object, ",");

        CCPointButBetter pos{};

        for (size_t i = 0; i + 1 < values.size(); ++i) {
            auto key = values[i];

            if (key == "2") {
                pos.x = numFromString<double>(values[i + 1]).unwrapOrDefault();
            }
            else if (key == "3") {
                pos.y = numFromString<double>(values[i + 1]).unwrapOrDefault();
            }

            // pula o valor correspondente à key
            ++i;
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
            if (values[i] == "2") {
                auto toSet = std::to_string(positions[obj].x);
                log::warn("X {}",toSet);
                values[i + 1] = toSet;
            }
            else if (values[i] == "3") {
                auto toSet = std::to_string(positions[obj].y);
                values[i + 1] = toSet;
                log::warn("Y {}",toSet);
            }}

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

class objectCluster {
public:
    int uniqueId;
    CCSprite* getSprite() {
        return m_sprite;
    }
    void setSprite(CCSprite *spr){
        m_sprite = spr;
    }

public:
    static inline CCSprite* m_sprite;
    std::string m_relativeObjectStr;
    CCPointButBetter m_clusterSize;
    CCPointButBetter m_clusterCenter;

    bool init(std::string_view string) {
        auto center = getCenterCCPointButBetter(string);
        if (center) {
            log::warn("Achei o centro"); 
            m_clusterCenter = center.value();
        }
        CCPointButBetter min_point = {DBL_MAX,DBL_MAX};
        CCPointButBetter max_point = {DBL_MIN,DBL_MIN};
        auto positions = getPositions(string);
        for (auto &objPos : positions) {
            min_point = {minAndMax(min_point.x, objPos.x).min, minAndMax(min_point.y, objPos.y).min};
            max_point = {minAndMax(max_point.x, objPos.x).max, minAndMax(max_point.y, objPos.y).max};
        };
        m_clusterSize = max_point - min_point;
        if (!center) {
            m_clusterCenter = (m_clusterSize / 2) + min_point;
        }
        log::warn("Centro: {}\nMin Point {}\n Max point {}",m_clusterCenter,min_point,max_point); // should be 1125 540
        // absolute to relative
        for (auto &objPos : positions) {
            objPos -= m_clusterCenter;
        };
        m_relativeObjectStr = setPositions(string, positions);
        return true;
    };
    ~objectCluster(){
        log::warn("Morri!");
    }

public:
    static objectCluster *create(std::string_view string) {
        objectCluster *pRet = new objectCluster();
        if (pRet && pRet->init(string)) {
            return pRet;
        } else {
            delete pRet;
            pRet = 0;
            return nullptr;
        }
    }
    void placeOn(CCPointButBetter pos) {
        auto posAbs = getPositions(m_relativeObjectStr);
        log::warn("posabs {}\n",posAbs.size());
        for (auto &obj : posAbs) {
            log::warn("Before {} {}\n After {} {}",obj.x,obj.y,obj.x+pos.x, obj.y+pos.y);
            obj += pos;
        };
        std::string dummy;
        dummy = setPositions(m_relativeObjectStr, posAbs);
        auto objs = LevelEditorLayer::get()->createObjectsFromString(dummy, false, true);
        LevelEditorLayer::get()->addObjectsAtPosition({},objs,nullptr);
        EditorUI::get()->selectObjects(objs,true);
    }
};

inline std::map<int, objectCluster *> m_objectClusters; // uniqueId - obj
inline int curObjRegIdx = 258672;

void registerObjectCluster(objectCluster *obj);

}; // namespace ObjectClusterAPI