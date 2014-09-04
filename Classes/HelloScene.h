#ifndef __HELLO_SCENE_H__
#define __HELLO_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UILayout.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace cocostudio;

class HelloScene : public cocos2d::Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* scene();

    void menuCloseCallback(cocos2d::Ref* pSender);

    void animationEvent(Armature *pArmature,
        MovementEventType movementType, const char *movementID);

    virtual void update(float delta);

    // implement the "static node()" method manually
    CREATE_FUNC(HelloScene);


    void updateData();
    // events
    void startGame(Ref* pSender, ui::TouchEventType type);
private:
    ui::Layout* m_pHelloLayout;
    Vec2 m_stTouchMoveStartPosition;
};

#endif // __HELLOWORLD_SCENE_H__
