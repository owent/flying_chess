#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <ctime>
#include <cstdlib>

#include "cocos2d.h"
#include "cocos-ext.h"
#include "cocostudio/CocoStudio.h"
#include "ui/UILayout.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace cocostudio;

class GameScene : public cocos2d::Layer
{
public:
    virtual ~GameScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* scene();

    virtual void update(float delta);

    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);


    void updateData();

    void onGameOver();


    enum EDD {
        EDD_UP2DOWN = 0,
        EDD_DOWN2UP,
        EDD_LEFT2RIGHT,
        EDD_RIGHT2LEFT,
    };

    void openExitDialog(EDD d);
private:
    ui::Layout* m_pGameLayout;
    Vec2 m_stTouchMoveStartPosition;
    ui::Layout* m_pGameOverDlgLayout;

    clock_t m_pTouchBeginClock;
};

#endif // __HELLOWORLD_SCENE_H__
