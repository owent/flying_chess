#include <cmath>
#include <algorithm>

#include "GameScene.h"
#include "HelloScene.h"
#include "VisibleRect.h"
#include "ui/CocosGUI.h"
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
    return TransitionFade::create(0.5f, scene);
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
    m_pGameOverDlgLayout = NULL;
    m_pTouchBeginClock = 0;

    // ui animation root from json
    m_pGameLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/game_panel_bk.ExportJson"));
    m_pGameLayout->setContentSize(Size(600.0f, 600.0f));
    {
        // 初始自适应大小
        {
            using std::min;
            auto father_size = getContentSize();
            float scale_final = min(father_size.width / m_pGameLayout->getContentSize().width, father_size.height / m_pGameLayout->getContentSize().height);
            m_pGameLayout->setScale(scale_final);
        }
        
        m_pGameLayout->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type){
            auto me = static_cast<Widget*>(pSender);
            const clock_t action_clock = CLOCKS_PER_SEC / 4; // 250ms
            float action_distance = 200.0; // 200 logic px

            if (Widget::TouchEventType::BEGAN == type) {
                this->m_stTouchMoveStartPosition = me->getPosition();
                this->m_pTouchBeginClock = clock();

                me->getActionManager()->pauseTarget(me);
            } else if (Widget::TouchEventType::MOVED == type) {
                auto move_end = me->getTouchMovePosition();
                auto move_begin = me->getTouchBeganPosition();

                // 移动层
                if (clock() - this->m_pTouchBeginClock > action_clock) {
                    auto my_pos = this->m_stTouchMoveStartPosition + move_end - move_begin;
                    me->setPosition(my_pos);
                }
            } else if (Widget::TouchEventType::ENDED == type) {
                auto move_begin = me->getTouchBeganPosition();
                auto move_end = me->getTouchEndPosition();

                // 移动层
                {
                    bool need_move = false;
                    me->getActionManager()->removeAllActionsFromTarget(me);

                    auto cur_pos = me->getPosition();
                    auto father_size = me->getParent()->getContentSize();
                    auto my_size = me->getContentSize();
                    auto fix_pos = Vec2(
                        scene_fix_coordinate(cur_pos.x, father_size.width / 2, father_size.width, my_size.width),
                        scene_fix_coordinate(cur_pos.y, father_size.height / 2, father_size.height, my_size.height)
                        );

                    if (std::numeric_limits<float>::epsilon() <= fabs(fix_pos.x - cur_pos.x) || std::numeric_limits<float>::epsilon() <= fabs(fix_pos.y - cur_pos.y)) {
                        me->runAction(MoveTo::create(.5f, fix_pos));
                    }
                } 

                if (move_end.distance(move_begin) >= action_distance && clock() - this->m_pTouchBeginClock < action_clock) { // 手势
                    EDD d = EDD_UP2DOWN;
                    if (fabs(move_end.y - move_begin.y) > fabs(move_end.x - move_begin.x)) {
                        if (move_end.y > move_begin.y)
                            d = EDD_DOWN2UP;
                    } else {
                        if (move_end.x > move_begin.x)
                            d = EDD_LEFT2RIGHT;
                        else
                            d = EDD_RIGHT2LEFT;
                    }
                    this->openExitDialog(d);
                }

            } else if (Widget::TouchEventType::CANCELED == type) {
                me->getActionManager()->resumeTarget(me);
            }
        });

        auto my_size = getContentSize();
        m_pGameLayout->setAnchorPoint(Vec2(0.5, 0.5));
        m_pGameLayout->setPosition(Vec2(my_size.width / 2, my_size.height / 2));
    }
    addChild(m_pGameLayout);
    log("background layer size (%f, %f) position (%f, %f)",
        m_pGameLayout->getContentSize().width,
        m_pGameLayout->getContentSize().height,
        m_pGameLayout->getPositionX(),
        m_pGameLayout->getPositionY()
    );

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
    fc::Judger::GetInstance().OnReadyToStart();
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

    if (NULL == m_pGameOverDlgLayout) {
        m_pGameOverDlgLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/gameover_dialog.ExportJson"));
        auto sprite_layer = Sprite::create();

        m_pGameLayout->addChild(sprite_layer);
        sprite_layer->addChild(m_pGameOverDlgLayout);

        m_pGameOverDlgLayout->setAnchorPoint(Vec2(0.5, 0.5));
        m_pGameOverDlgLayout->setPosition(m_pGameLayout->getContentSize() / 2.0f);

        Button* ok_button = static_cast<Button*>(Helper::seekWidgetByName(m_pGameOverDlgLayout, "Button_GameOver"));

        ok_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type){
            if (type == Widget::TouchEventType::ENDED) {
                Director::getInstance()->replaceScene(HelloScene::scene());
            }
        });

        m_pGameOverDlgLayout->setScale(0.01f);
        m_pGameOverDlgLayout->runAction(ScaleTo::create(0.5f, 1.0));
    }
}

void GameScene::openExitDialog(EDD d) {

    auto exit_dlg = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/exit_dialog.ExportJson"));
    auto sprite_layer = Sprite::create();

    m_pGameLayout->addChild(sprite_layer);
    sprite_layer->addChild(exit_dlg);

    exit_dlg->setAnchorPoint(Vec2(0.5, 0.5));
    auto place_pos = Vec2(m_pGameLayout->getContentSize() / 2.0f);

    Vec2 begin_pos, end_pos;
    switch (d){
    case EDD_UP2DOWN:
        begin_pos = place_pos + Vec2(0.0f, m_pGameLayout->getContentSize().height);
        end_pos = place_pos - Vec2(0.0f, m_pGameLayout->getContentSize().height);
        break;
    case EDD_DOWN2UP:
        begin_pos = place_pos - Vec2(0.0f, m_pGameLayout->getContentSize().height);
        end_pos = place_pos + Vec2(0.0f, m_pGameLayout->getContentSize().height);
        break;
    case EDD_LEFT2RIGHT:
        begin_pos = place_pos - Vec2(m_pGameLayout->getContentSize().width, 0.0f);
        end_pos = place_pos + Vec2(m_pGameLayout->getContentSize().width, 0.0f);
        break;
    case EDD_RIGHT2LEFT:
        begin_pos = place_pos + Vec2(m_pGameLayout->getContentSize().width, 0.0f);
        end_pos = place_pos - Vec2(m_pGameLayout->getContentSize().width, 0.0f);
        break;
    }

    Button* yes_button = dynamic_cast<Button*>(Helper::seekWidgetByName(exit_dlg, "Button_YES"));
    Button* no_button = dynamic_cast<Button*>(Helper::seekWidgetByName(exit_dlg, "Button_NO"));

    yes_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type){
        if (type == Widget::TouchEventType::ENDED) {
            Director::getInstance()->replaceScene(HelloScene::scene());
        }
    });

    no_button->addTouchEventListener([end_pos, exit_dlg](Ref* pSender, Widget::TouchEventType type){
        if (type == Widget::TouchEventType::ENDED) {
            exit_dlg->runAction(Sequence::create(
                Spawn::create(
                    MoveTo::create(0.3f, end_pos), 
                    FadeTo::create(0.3f, 0),
                    NULL
                ),
                CallFunc::create([pSender, exit_dlg](){
                    exit_dlg->getParent()->removeChild(exit_dlg, true);
                }),
                NULL
            ));
        }
    });

    exit_dlg->setPosition(begin_pos);
    exit_dlg->setOpacity(0);
    exit_dlg->runAction(Spawn::create(MoveTo::create(0.3f, place_pos), FadeTo::create(0.3f, 255), NULL));
}
