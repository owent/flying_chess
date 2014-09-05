#include <assert.h>
#include <limits>
#include <numeric>
#include <cmath>

#include "Config.h"
#include "Plane.h"
#include "Grid.h"
#include "Judger.h"
#include "SoundMgr.h"

#include <2d/CCActionInterval.h>
#include <2d/CCActionInstant.h>

namespace fc {
    void Plane::Init(int home_grid_, EnPlayerColor color_, Player* player_) {
        home_grid = home_grid_;
        color = color_;
        player = player_;
        m_pUILayout = NULL;

        assert(GridPool::Find(at_grid));
        assert(player);
    }

    void Plane::Reset(bool available) {
        left_fly = left_jump = 1;
        speed = Config::GetInstance().Plane.MoveSpeed;

        is_win = !available;

        at_grid = home_grid;

        // UI - Actions
        m_pUILayout = NULL;
        m_stUIActions.clear();
        SetActionTime(0.0f);
    }


    bool Plane::Move(int point) {
        left_fly = left_jump = 1;
        speed = Config::GetInstance().Plane.MoveSpeed;

        if (is_win)
            return false;

        if (!IsStarted()) {
            // 启动状态转化为启动出发
            point = 1;
        }

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;

        int reason = LGR_MOVE_START;

        using std::abs;
        while (point) {
            grid->OnLeave(*this, reason);
            auto next = grid->OnPass(*this, point, reason);

            reason = LGR_PASS;
            if (!next)
                break;

            grid = next;
        }

        return MoveTo(grid->ID(), LGR_ARRIVE);
    }

    bool Plane::MoveTo(int grid_id, int reason) {
        GridPool::grid_ptr grid = GridPool::Find(grid_id);
        if (!grid)
            return false;

        grid->OnArrive(*this, reason);
        return true;
    }

    bool Plane::FlyTo(int grid_id) {
        if (!CanFly())
            return false;

        --left_fly;
        speed = Config::GetInstance().Plane.FlySpeed;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;
        grid->OnLeave(*this, LGR_FLY | LGR_ARRIVE);

        return MoveTo(grid_id, LGR_FLY | LGR_ARRIVE);
    }

    bool Plane::JumpTo(int grid_id) {
        if (!CanJump())
            return false;

        --left_jump;
        speed = Config::GetInstance().Plane.JumpSpeed;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;
        grid->OnLeave(*this, LGR_JUMP | LGR_ARRIVE);

        return MoveTo(grid_id, LGR_JUMP | LGR_ARRIVE);
    }

    bool Plane::GoHome(int reason, float delay) {
        if (GetLocateGridID() == home_grid)
            return false;

        speed = Config::GetInstance().Plane.GoHomeSpeed;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;
        grid->OnLeave(*this, reason);

        bool ret = MoveTo(home_grid, reason);

        // 胜利则追加disable动画
        if (LGR_WIN == reason) {
            AddDisableAction(0.3f, 0, reason);
            is_win = true;
        }
        return ret;
    }


    void Plane::SetUI(cocos2d::ui::Layout* layout) {
        m_pUILayout = layout;

        // 设置位置
        if (NULL == m_pUILayout)
            return;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        assert(grid);
        m_pUILayout->setPosition(cocos2d::Vec2(grid->GetPositionX(), grid->GetPositionY()));
        cocos2d::log("set player %d'splane to (%f, %f)", Color(), grid->GetPositionX(), grid->GetPositionY());

        if (!IsAvailable() || IsWin()) {
            m_pUILayout->setOpacity(128);
        }
    }


    void Plane::ActiveAction() {
        next_action();
    }

    void Plane::AddAnimationAction(Grid& g, float duration, float delay, int reason) {
        UIAction act;
        act.type = UIAT_MOVE;
        act.delay = delay;
        act.duration = duration;
        act.reason = reason;
        act.param[0] = g.GetPositionX();
        act.param[1] = g.GetPositionY();
        m_stUIActions.push_back(act);

        AddActionTime(duration + delay);

        at_grid = g.ID();
        next_action();
    }

    void Plane::AddDisableAction(float duration, float delay, int reason) {
        UIAction act;
        act.type = UIAT_DISABLE;
        act.delay = delay;
        act.duration = duration;
        act.reason = reason;
        m_stUIActions.push_back(act);

        AddActionTime(duration + delay);

        next_action();
    }

    void Plane::AddEnableAction(float duration, float delay, int reason) {
        UIAction act;
        act.type = UIAT_ENABLE;
        act.delay = delay;
        act.duration = duration;
        act.reason = reason;
        m_stUIActions.push_back(act);

        AddActionTime(duration + delay);

        next_action();
    }

    void Plane::OnActionStart(UIAction* act) {
        if (NULL == act)
            return;

        if (act->reason & LGR_MOVE_START) {
            // 播放启动音效
            SoundMgr::GetInstance().PlayMove();
        }

        if (act->reason & LGR_KICKOFF) {
            // 播放被击毁音效
            SoundMgr::GetInstance().PlayGoHome();
        }

        if (act->reason & LGR_WIN) {
            // 播放到达终点音效
            SoundMgr::GetInstance().PlayJump();
        }

        if (act->reason & LGR_JUMP) {
            // 播放跳跃音效
            SoundMgr::GetInstance().PlayJump();
        }

        if (act->reason & LGR_FLY) {
            // 播放飞行音效
            SoundMgr::GetInstance().PlayJump();
        }

    }

    void Plane::OnActionEnd(UIAction* act) {
    }

    void Plane::next_action() {
        // 无动作立刻回调
        if (m_stUIActions.empty()) {
            OnActionStart(NULL);
            SetActionTime(0.0f);
            OnActionEnd(NULL);

            Judger::GetInstance().DoNext();
            return;
        }

        // 无UI对象立刻回调
        if (NULL == m_pUILayout) {
            while (!m_stUIActions.empty()) {
                auto act = m_stUIActions.front();
                OnActionStart(&act);
                m_stUIActions.pop_front();
                SetActionTime(0.0f);
                OnActionEnd(&act);
            }

            Judger::GetInstance().DoNext();
            return;
        }

        // 正在执行
        UIAction& act = m_stUIActions.front();
        if (UIAT_RUNNING == act.type)
            return;

        using namespace cocos2d;

        ActionInterval* ui_action = NULL;
        switch (act.type)
        {
        case UIAT_MOVE:
            if (act.duration > std::numeric_limits<float>::epsilon())
                ui_action = MoveTo::create(act.duration, Vec2(act.param[0], act.param[1]));
            break;

        case UIAT_DISABLE:
            ui_action = FadeTo::create(act.duration, 128);
            break;

        case UIAT_ENABLE:
            ui_action = FadeTo::create(act.duration, 255);
            break;

        default:
            assert(ui_action);
            break;
        }

        act.type = UIAT_RUNNING;

        auto callback = [this]() {
            // 动作执行完毕，出列
            if (!m_stUIActions.empty()) {
                auto act = m_stUIActions.front();
                this->AddActionTime(-(act.delay + act.duration));
                m_stUIActions.pop_front();
                OnActionEnd(&act);
            }

            this->next_action();
        };

        Sequence* seq = NULL;
        if (ui_action && act.delay > 0)
            seq = Sequence::create(DelayTime::create(m_stUIActions.front().delay), ui_action, CallFunc::create(callback), NULL);
        else if (ui_action)
            seq = Sequence::create(ui_action, CallFunc::create(callback), NULL);

        // 没有动作序列就立即回调
        OnActionStart(&act);

        if (seq)
            m_pUILayout->runAction(seq);
        else
            callback();
    }
}
