#include <Geode/Geode.hpp>
#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include "Geode/ui/Layout.hpp"
#include "Geode/ui/Popup.hpp"


namespace keyEdit {
class setKeyPopup : public geode::Popup {
protected:
    std::function<void(int)> m_callback;

    bool init(std::function<void(int)> cb)
    {
        if (!Popup::init(240.f, 240.f))
            return false;
        
        m_callback = cb;

        auto label = cocos2d::CCLabelBMFont::create("Waiting for key...","goldFont.fnt");
        label->setScale(0.8f);
        m_mainLayer->addChildAtPosition(label,geode::Anchor::Center);

        cocos2d::CCDirector::sharedDirector()
            ->getKeyboardDispatcher()
            ->addDelegate(this);
        return true;
    }


    void keyDown(cocos2d::enumKeyCodes key, double isRepeat) override {
        int keyCode = static_cast<int>(key);
        if (key == cocos2d::KEY_Escape) { // to cancell the action
            m_callback(-2);
            this->onClose(nullptr); 
            return;
        }
        if (m_callback)
            m_callback(keyCode);

        this->onClose(nullptr);
        delete this;
    }
    

public:
    static setKeyPopup* create(std::function<void(int)> cb) {
        auto ret = new setKeyPopup;
        if (ret && ret->init(cb)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
    static void open(CCObject *,std::function<void(int)> cb)
    {
        auto layer = create(cb);
        layer->show();
    }
};
} // namespace keyEdit