#include <assert.h>
#include <limits>
#include <numeric>
#include <cmath>

#include "Config.h"
#include "Plane.h"
#include "Grid.h"
#include "Judger.h"

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
        is_start = false;

        at_grid = home_grid;

        // UI - Actions
        m_pUILayout = NULL;
        m_stUIActions.clear();
    }


    bool Plane::Move(int point) {
        left_fly = left_jump = 1;
        speed = Config::GetInstance().Plane.MoveSpeed;

        if (is_win)
            return false;

        if (!is_start) {
            // 启动状态转化为启动出发
            point = 1;
        }

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;

        int reason = LGR_MOVE_START;

        using std::abs;
        // 触发Pass首个节点时会消耗一个point
        point = point + point / abs(point);

        while (point) {
            grid->OnLeave(*this, reason);
            reason = LGR_PASS;

            auto next = grid->OnPass(*this, point);
            at_grid = grid->ID();
            if (next)
                grid = next;
            else
                break;
        }

        return MoveTo(grid->ID());
    }

    bool Plane::MoveTo(int grid_id) {
        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;

        at_grid = grid_id;
        is_start = !(at_grid == home_grid);
        grid->OnArrive(*this);

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
        grid->OnLeave(*this, LGR_JUMP);

        return MoveTo(grid_id);
    }

    bool Plane::JumpTo(int grid_id) {
        if (!CanJump())
            return false;

        --left_jump;
        speed = Config::GetInstance().Plane.JumpSpeed;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;
        grid->OnLeave(*this, LGR_JUMP);

        return MoveTo(grid_id);
    }

    bool Plane::GoHome(int reason) {
        if (GetLocateGridID() == home_grid)
            return false;

        speed = Config::GetInstance().Plane.GoHomeSpeed;

        GridPool::grid_ptr grid = GridPool::Find(at_grid);
        if (!grid)
            return false;
        grid->OnLeave(*this, reason);

        bool ret = MoveTo(home_grid);

        // 胜利则追加disable动画
        if (LGR_WIN == reason)
            AddDisableAction(0.3f, 0);
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

    void Plane::AddAnimationAction(float x, float y, float duration, float delay) {
        UIAction act;
        act.type = UIAT_MOVE;
        act.delay = delay;
        act.duration = duration;
        act.param[0] = x;
        act.param[1] = y;
        m_stUIActions.push_back(act);

        next_action();
    }

    void Plane::AddDisableAction(float duration, float delay) {
        UIAction act;
        act.type = UIAT_DISABLE;
        act.delay = delay;
        act.duration = duration;
        m_stUIActions.push_back(act);

        next_action();
    }

    void Plane::AddEnableAction(float duration, float delay) {
        UIAction act;
        act.type = UIAT_ENABLE;
        act.delay = delay;
        act.duration = duration;
        m_stUIActions.push_back(act);

        next_action();
    }

    void Plane::next_action() {
        // 无动作立刻回调
        if (m_stUIActions.empty()) {
            Judger::GetInstance().DoNext();
            return;
        }

        // 无UI对象立刻回调
        if (NULL == m_pUILayout) {
            m_stUIActions.clear();

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
            if (!m_stUIActions.empty())
                m_stUIActions.pop_front();

            this->next_action();
        };

        Sequence* seq = NULL;
        if (ui_action && act.delay > 0)
            seq = Sequence::create(DelayTime::create(m_stUIActions.front().delay), ui_action, CallFunc::create(callback), NULL);
        else if (ui_action)
            seq = Sequence::create(ui_action, CallFunc::create(callback), NULL);
        else
            seq = Sequence::create(CallFunc::create(callback), NULL);

        m_pUILayout->runAction(seq);
    }
}
