#include <cstdio>
#include <limits>
#include <numeric>

#include "HelloScene.h"
#include "GameScene.h"
#include "VisibleRect.h"
#include "cocos-ext.h"
#include "CocosGUI.h"
#include "cocostudio/CCSSceneReader.h"
#include "cocostudio/CCSGUIReader.h"
#include "cocostudio/CCActionManagerEx.h"
#include "cocostudio/CCArmatureDataManager.h"

#include "Player.h"
#include "Judger.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;

Scene* HelloScene::scene()
{
    fc::Judger::GetInstance().Reset();

    // 'scene' is an autorelease object
    Scene *scene = Scene::create();

    // 'layer' is an autorelease object
    HelloScene *layer = HelloScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return TransitionFade::create(0.5f, scene);
}

// on "init" you need to initialize your instance
bool HelloScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    // ui animation root from json
    m_pHelloLayout = dynamic_cast<Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/hello_panel.ExportJson"));
    {
        m_pHelloLayout->addTouchEventListener([this](Ref* pSender, Widget::TouchEventType type){
            auto me = static_cast<Widget*>(pSender);

            if (Widget::TouchEventType::BEGAN == type) {    
                this->m_stTouchMoveStartPosition = me->getPosition();
                me->getActionManager()->pauseTarget(me);
            } else if (Widget::TouchEventType::MOVED == type) {
                auto move_end = me->getTouchMovePosition();
                auto move_begin = me->getTouchBeganPosition();
                auto my_pos = this->m_stTouchMoveStartPosition + move_end - move_begin;
                me->setPosition(my_pos);
            } else if (Widget::TouchEventType::ENDED == type) {
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

            } else if (Widget::TouchEventType::CANCELED == type) {
                me->getActionManager()->resumeTarget(me);
            }
        });

        auto my_size = getContentSize();
        m_pHelloLayout->setAnchorPoint(Vec2(0.5, 0.5));
        m_pHelloLayout->setPosition(Vec2(my_size.width / 2, my_size.height / 2));
    }
    addChild(m_pHelloLayout);

    // 开始按钮
    {
        Button* start_button = static_cast<Button*>(Helper::seekWidgetByName(m_pHelloLayout, "Button_StartGame"));
        start_button->addTouchEventListener([](Ref* pSender, Widget::TouchEventType type){
            if (type == Widget::TouchEventType::ENDED) {
                // 开始游戏，切换scene
                Director::getInstance()->replaceScene(GameScene::scene());
            }
        });
    }

    // 玩家状态切换按钮
    {
        auto player_status_btn = [](cocos2d::ui::Layout* m_pHelloLayout, fc::EnPlayerColor color) {
            char btn_name[32] = { 0 };
            sprintf(btn_name, "Button_Player_Color_%d", color);

            Button* player_button = static_cast<Button*>(Helper::seekWidgetByName(m_pHelloLayout, btn_name));
            //std::function<void(Ref*, Widget::TouchEventType)>
            auto evt_func = [player_button, color](Ref* pSender, Widget::TouchEventType type){
                if (type == Widget::TouchEventType::ENDED) {
                    fc::PlayerSelecter ps = fc::Player::Pool[color].GetStatus();
                    ps = static_cast<fc::PlayerSelecter>((ps + 1) % fc::PS_MAX);
                    fc::Player::Pool[color].SetStatus(ps);

                    std::string name = fc::Player::Pool[color].GetPlayerName();
                    if (fc::PS_DISABLE == ps) {
                        name += ": None";
                    }
                    else if (fc::PS_HUMEN == ps) {
                        name += ": Player";
                    }
                    else if (fc::PS_AI == ps) {
                        name += ": AI";
                    }
                    else {
                        name += ": Invalid";
                    }

                    player_button->setTitleText(name);
                }
            };

            player_button->addTouchEventListener(evt_func);
            evt_func(NULL, Widget::TouchEventType::ENDED);
        };


        for (int i = 0; i < fc::EPC_MAX; ++i) {
            player_status_btn(m_pHelloLayout, static_cast<fc::EnPlayerColor>(i));
        }
    }

    // 飞机数量选择框
    {
        auto num_scroll_view = dynamic_cast<ui::ScrollView*>(Helper::seekWidgetByName(m_pHelloLayout, "ListView_Plane_List"));
        num_scroll_view->setBounceEnabled(true);
        num_scroll_view->getInnerContainer()->setAnchorPoint(Vec2(0.0f, 1.0f));
        num_scroll_view->setUserData((void*)(int)1);

        auto callback = [](Ref* sender, ui::ScrollView::EventType type){
            switch (type) {
            case cocos2d::ui::ScrollView::EventType::SCROLLING: {
                ui::ScrollView* me = static_cast<ui::ScrollView*>(sender);
                auto my_sh = me->getCustomSize().height; // 可视区域范围
                auto c_sh = me->getInnerContainer()->getContentSize().height - my_sh; // 最大有效活动范围
                auto c_mh = me->getInnerContainer()->getPositionY() - my_sh; // 当前位置偏移

                const int ele_num = me->getChildrenCount();
                auto unit_h = c_sh / (ele_num - 1); // 每个选项UI组件占用长度

                int now_select_index = (int) me->getUserData();
                int select_index = 0;

                // Select NO. 1
                if (c_mh < unit_h * 0.5f)
                    select_index = 1;
                else if (c_mh < c_sh - unit_h * 0.5f)
                    select_index = (int) ((c_mh + unit_h * 0.5f) / unit_h) + 1;
                else // Select NO. N
                    select_index = ele_num;

                // 触发更换选取值
                if (select_index != now_select_index) {
                    me->setUserData((void*) select_index);
                    log("change plane number from %d to %d.", now_select_index, select_index);
                    fc::Judger::GetInstance().SetPlaneNumber(select_index);
                    auto old_item = me->getChildren().at(now_select_index - 1);
                    auto new_item = me->getChildren().at(select_index - 1);

                    old_item->setColor(Color3B(255, 255, 255));
                    new_item->setColor(Color3B(128, 96, 0));
                }
                break;
            }
            default:
                break;
            }
        };
        num_scroll_view->addEventListener((ui::ScrollView::ccScrollViewCallback) (callback));
        callback(num_scroll_view, cocos2d::ui::ScrollView::EventType::SCROLLING);
    }

    // 排行版
    {
        fc::EnPlayerColor player_ranking[fc::EPC_MAX];
        fc::Player::SortByRanking(player_ranking);

        auto player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_1"));
        fc::Player* player = &fc::Player::Pool[player_ranking[0]];
        auto color = player->GetPlayerColor();
        std::string text_content = " 1st: ";

        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        text_content += player->HasRanking() ? player->GetPlayerName() : "None";
        player_rk->setString(text_content);

        player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_2"));
        player = &fc::Player::Pool[player_ranking[1]];
        color = player->GetPlayerColor();
        text_content = " 2nd: ";
        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        text_content += player->HasRanking() ? player->GetPlayerName() : "None";
        player_rk->setString(text_content);

        player_rk = static_cast<Text*>(Helper::seekWidgetByName(m_pHelloLayout, "Label_Ranking_3"));
        player = &fc::Player::Pool[player_ranking[2]];
        color = player->GetPlayerColor();
        text_content = " 3rd: ";
        player_rk->setColor(Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));
        text_content += player->HasRanking() ? player->GetPlayerName() : "None";
        player_rk->setString(text_content);
    }

    scheduleUpdate();

    return true;
}

void HelloScene::menuCloseCallback(Ref* pSender)
{
    //cocostudio::ArmatureDataManager::destroyInstance();
    //cocostudio::SceneReader::destroyInstance();
    cocostudio::GUIReader::destroyInstance();
    //cocostudio::ActionManagerEx::destroyInstance();

    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloScene::animationEvent(Armature *pArmature,
    MovementEventType movementType, const char *movementID)
{
    //ComRender *render = (ComRender*) (m_pGameScene->getChildByTag(10006)->getComponent("CCArmature"));
    //Armature *pEnemy = (Armature*) (render->getNode());

    //if (movementType == COMPLETE)
    //{
    //    pEnemy->getAnimation()->play("death");
    //    pArmature->getAnimation()->setMovementEventCallFunc(NULL, NULL);
    //}

}

void HelloScene::update(float delta)
{
    //if (m_bStart)
    //{
    //    ComRender *pHeroRender = (ComRender*) (m_pGameScene->getChildByTag(10005)->getComponent("CCArmature"));
    //    Armature *pHero = (Armature*) (pHeroRender->getNode());
    //    pHero->getParent()->setPositionX(pHero->getParent()->getPositionX() + m_fSpeed);

    //    ComRender *pEnemyRender = (ComRender*) (m_pGameScene->getChildByTag(10006)->getComponent("CCArmature"));
    //    Armature *pEnemy = (Armature*) (pEnemyRender->getNode());

    //    /* for 3.0 */
    //    float distance = Point(pHero->getParent()->getPositionX(), 0).getDistance(Point(pEnemy->getParent()->getPositionX(), 0));
    //    if (distance < m_fAttackDis)
    //    {
    //        pHero->getAnimation()->play("attack");
    //        pHero->getAnimation()->setMovementEventCallFunc(this,
    //            movementEvent_selector(HelloScene::animationEvent));
    //        m_bStart = false;
    //    }
    //    // before
    //    /*
    //    if(ccpDistance(ccp(pHero->getParent()->getPositionX(), 0), ccp(pEnemy->getParent()->getPositionX(), 0)) < m_fAttackDis)
    //    {
    //    pHero->getAnimation()->play("attack");
    //    pHero->getAnimation()->setMovementEventCallFunc(this,
    //    movementEvent_selector(HelloScene::animationEvent));
    //    m_bStart = false;
    //    }
    //    */
    //    /**/
    //}

    //if (m_bDead)
    //{
    //    ComRender *pUIRender = static_cast<ComRender*>(m_pGameScene->getChildByTag(10007)->getComponent("GUIComponent"));


    //    /* for 3.0 */
    //    Node* pUILayer = static_cast<Node*>(pUIRender->getNode());
    //    Layout* root = static_cast<Layout*>(pUILayer->getChildren().at(2));
    //    LoadingBar *pHPLoadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(root, "hp02_LoadingBar"));
    //    LoadingBar *pMPLoadingBar = static_cast<LoadingBar*>(Helper::seekWidgetByName(root, "mp02_LoadingBar"));
    //    // before
    //    /*
    //    cocos2d::ui::TouchGroup *pUILayer = static_cast<cocos2d::ui::TouchGroup*>(pUIRender->getNode());
    //    cocos2d::ui::LoadingBar *pHPLoadingBar = static_cast<cocos2d::ui::LoadingBar*>(pUILayer->getWidgetByName("hp02_LoadingBar"));
    //    cocos2d::ui::LoadingBar *pMPLoadingBar = static_cast<cocos2d::ui::LoadingBar*>(pUILayer->getWidgetByName("mp02_LoadingBar"));
    //    */
    //    /**/

    //    pHPLoadingBar->setPercent(m_fPercentage);
    //    pMPLoadingBar->setPercent(m_fPercentage);

    //    m_fPercentage -= 2.0f;
    //    if (m_fPercentage < 0.0f) {
    //        unscheduleUpdate();
    //    }
    //}

}

void HelloScene::updateData() {
}

void HelloScene::startGame(Ref* pSender, TouchEventType type)
{
    if (type == TOUCH_EVENT_ENDED)
    {
        
    }
}