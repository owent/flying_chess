#include <cstdlib>
#include <cstdio>

#include "Config.h"
#include "Judger.h"
#include "Player.h"
#include "RollPoints.h"

#include "cocostudio/CCArmatureDataManager.h"
#include "cocostudio/CCArmature.h"
#include <2d/CCActionInterval.h>
#include <2d/CCActionInstant.h>

#include "cocostudio/CCSGUIReader.h"

#define NTAG_ARMATURE 20001
namespace fc {


    RollPoints::RollPoints(){
        using cocostudio::ArmatureDataManager;
        ArmatureDataManager::getInstance()->addArmatureFileInfo(
            "publish/roll_points0.png",
            "publish/roll_points0.plist",
            "publish/roll_points.ExportJson"
        );
    }

    void RollPoints::Reset() {
        m_pRollUI = NULL;
    }

    void RollPoints::SetUI(cocos2d::ui::Layout* l) {
        m_pRollUI = l;

        if (NULL == l)
            return;

        // 设置缩放
        OnUpdate();
    }

    void RollPoints::OnUpdate() {
        if (NULL == m_pRollUI)
            return;

        EnPlayerColor cur_player = Judger::GetInstance().GetTopActionPlayer();
        float x = Config::GetInstance().GridCfg.MarginLeft + (Config::GetInstance().RollCfg.Rolls[cur_player].x - 0.5f) * Config::GetInstance().GridCfg.Width;
        float y = Config::GetInstance().GridCfg.MarginTop + (16 - Config::GetInstance().RollCfg.Rolls[cur_player].y - 0.5f) * Config::GetInstance().GridCfg.Height;

        m_pRollUI->setPosition(cocos2d::Vec2(x, y));
        m_pRollUI->setBackGroundColorOpacity(192);
        auto color = Player::Pool[cur_player].GetPlayerColor();
        m_pRollUI->setBackGroundColor(cocos2d::Color3B(std::get<0>(color), std::get<1>(color), std::get<2>(color)));

        cocos2d::log("roll points update to player %d (%f, %f)", cur_player, x, y);
    }

    void RollPoints::RollPoint() {
        using namespace cocos2d;

        int point = rand() % 6 + 1;

        if (NULL == m_pRollUI) {
            Judger::GetInstance().OnRollPointEnd(point);
            return;
        }


        auto arm = dynamic_cast<cocostudio::Armature*>(m_pRollUI->getChildByTag(NTAG_ARMATURE));
        arm->getAnimation()->playWithIndex(0); // 滚动骰子

        auto seq = Sequence::create(DelayTime::create(Config::GetInstance().RollCfg.Duration), CallFunc::create([arm, point](){
            cocos2d::log("[UI] end roll points %fs and res: %d", Config::GetInstance().RollCfg.Duration, point);

            arm->getAnimation()->playWithIndex(point);
            arm->getAnimation()->pause();
            Judger::GetInstance().OnRollPointEnd(point);
        }), NULL);
        
        cocos2d::log("[UI] begin roll points %fs and res: %d", Config::GetInstance().RollCfg.Duration, point);
        arm->runAction(seq);
    }

    cocos2d::ui::Layout* RollPoints::CreateUI() {
        auto arm = cocostudio::Armature::create("roll_points");
        arm->setTag(NTAG_ARMATURE);
        arm->setAnchorPoint(cocos2d::Vec2(.5, .5));

        auto ret = dynamic_cast<cocos2d::ui::Layout*>(cocostudio::GUIReader::getInstance()->widgetFromJsonFile("publish/roll_points_panel.ExportJson"));
        ret->addChild(arm);
        ret->setAnchorPoint(cocos2d::Vec2(.5, .5));
        SetUI(ret);
        arm->getAnimation()->playWithIndex(1); // 初始显示1
        arm->getAnimation()->pause();

        ret->addTouchEventListener([](cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type){
            if (cocos2d::ui::Widget::TouchEventType::ENDED == type) {
                cocos2d::log("roll points clicked");
                Judger::GetInstance().OnRollPointBegin();
            }
        });

        auto roll_size = arm->getContentSize();
        float width = Config::GetInstance().RollCfg.Width;
        float height = Config::GetInstance().RollCfg.Height;
        arm->setScale(width / roll_size.width / 1.1f, height / roll_size.height / 1.1f);
        arm->setPosition(width / 2.f, height / 2.0f);
        ret->setContentSize(cocos2d::Size(Config::GetInstance().RollCfg.Width, Config::GetInstance().RollCfg.Height));
        cocos2d::log("roll points ui created, size(%f, %f) roll size(%f, %f)", 
            ret->getContentSize().width, 
            ret->getContentSize().height,
            roll_size.width,
            roll_size.height
        );

        return ret;
    }
}