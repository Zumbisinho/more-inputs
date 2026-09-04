#include "objectCluster.hpp"
#include <Geode/modify/EditorUI.hpp>

using namespace ObjectClusterAPI;


class $modify(ObjectClustersEUI, EditorUI) {
    static void onModify(auto &self) {
        if (!self.setHookPriorityBeforePre("EditorUI::onCreateObject", "hjfod.betteredit"))
            log::warn("Not found");
    }
    CreateMenuItem *getCreateBtn(int id, int bg) {
        if (auto it = m_objectClusters.find(id); it != m_objectClusters.end()) {
            auto dummy = getCreateBtn(1, 4);
            auto array = CCArray::create();
            auto spr = it->second->getSprite();
            spr->setScale(
                std::min(
                    32.f / spr->getContentHeight(),
                    32.f / spr->getContentWidth()
                )
            );
            auto defaultBtn =
                static_cast<ButtonSprite *>(dummy->getNormalImage());
            if (auto toHide = defaultBtn->m_subSprite)
                toHide->setVisible(false);
            defaultBtn->addChild(spr);
            spr->setPosition({20, 21});

            dummy->m_objectID = it->first;
            dummy->setTag(it->first);
            return dummy;
        }

        CreateMenuItem *ret = EditorUI::getCreateBtn(id, bg);
        return ret;
    }
    void onCreateObject(int id) {
        if (auto it = m_objectClusters.find(id); it != m_objectClusters.end()){
            auto before = m_selectedObject;
            EditorUI::onCreateObject(1);
            if (before == m_selectedObject)
                return;
            CCPointButBetter pos = {m_selectedObject->m_positionX,m_selectedObject->m_positionY - 90};
            deleteObject(m_selectedObject,true);
            it->second->placeOn(pos);
        };
        EditorUI::onCreateObject(id);
    }
    
};

void ObjectClusterAPI::registerObjectCluster(objectCluster *obj) {
    obj->uniqueId = curObjRegIdx;
    m_objectClusters[curObjRegIdx] = obj;

};