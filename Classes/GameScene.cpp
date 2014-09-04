#include <cstdio>

#include "GameScene.h"
#include "HelloScene.h"
#include "VisibleRect.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "cocostudio/CCSSceneReader.h"
#include "cocostudio/CCSGUIReader.h"
#include "cocostudio/CCActionManagerEx.h"
#include "cocostudio/CCArmatureDataManager.h"

#include "Player.h"
#include "Judger.h"
#include "SoundMgr.h"
#include "RollPoints.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

Scene* GameScene::scene()
{
    // 开局数据初始化（不清空配置）
    fc::Judger::GetInstance().Reset(false);

    // 'scene' is an autorelease object
    Scene *scene = Scene::create();

    // 'layer' is an autorelease object
    GameScene *layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return TransitionFade::create(1.2f, scene);
    //return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    // ui animation root from json
    m_pGameLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/game_panel_bk.ExportJson"));
    {
        m_pGameLayout->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type){
            if (Widget::TouchEventType::BEGAN == type) {
                auto me = static_cast<Widget*>(pSender);
                this->m_stTouchMoveStartPosition = me->getPosition();
            } else if (Widget::TouchEventType::MOVED == type) {
                auto me = static_cast<Widget*>(pSender);
                auto move_end = me->getTouchMovePosition();
                auto move_begin = me->getTouchBeganPosition();
                auto my_pos = this->m_stTouchMoveStartPosition + move_end - move_begin;
                me->setPosition(my_pos);
            }
        });

        auto my_size = getContentSize();
        m_pGameLayout->setAnchorPoint(Vec2(0.5, 0.5));
        m_pGameLayout->setPosition(Vec2(my_size.width / 2, my_size.height / 2));
    }
    addChild(m_pGameLayout);

    // 初始化飞行棋
    {
        auto chesses_sprite = Sprite::create();
        m_pGameLayout->addChild(chesses_sprite);

        for (int i = 0; i < fc::EPC_MAX; ++i) {
            char chess_path[64] = {0};
            sprintf(chess_path, "publish/plane_color_%d.ExportJson", i);
            for (int j = 0; j < fc::EP_MAX; ++j) {
                auto chess_layout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile(chess_path));
                assert(chess_layout);
                chess_layout->setAnchorPoint(Vec2(.5, .5));
                chess_layout->setOpacity(255);
                //chess_layout->setBackGroundColor(Color3B(255, 255, 255));
                //chess_layout->setBackGroundColorOpacity(255);

                chess_layout->addTouchEventListener([i, j](Ref* pSender, Widget::TouchEventType type){
                    if (Widget::TouchEventType::ENDED == type) {
                        fc::Judger::GetInstance().OnSelectPlane(fc::Player::Pool[i][j]);

                        auto me = static_cast<Widget*>(pSender);
                        log("plane [%d][%d] clicked", i, j);
                    }
                });

                chesses_sprite->addChild(chess_layout);
                fc::Player::Pool[i][j].SetUI(chess_layout);
            }
        }
    }

    // 初始化骰子
    {
        auto roll_sprite = Sprite::create();
        m_pGameLayout->addChild(roll_sprite);

        auto roll = fc::RollPoints::GetInstance().CreateUI();
        roll_sprite->addChild(roll);
    }

    // 启动第一个事件
    fc::Judger::GetInstance().DoNext();

    scheduleUpdate();

    // 音效
    fc::SoundMgr::GetInstance().PlayBackground();
    return true;
}

GameScene::~GameScene() {
    // 音效
    fc::SoundMgr::GetInstance().StopBackground();
}

void GameScene::update(float delta)
{
}

void GameScene::updateData() {
}

void GameScene::onGameOver() {

    // 解绑UI
    for (int i = 0; i < fc::EPC_MAX; ++i)
        for (int j = 0; j < fc::EP_MAX; ++j)
            fc::Player::Pool[i][j].SetUI(NULL);

    fc::RollPoints::GetInstance().SetUI(NULL);

    Director::getInstance()->replaceScene(HelloScene::scene());
}
