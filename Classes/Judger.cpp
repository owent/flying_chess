#include <cstdlib>

#include "Config.h"
#include "Plane.h"
#include "Player.h"
#include "Grid.h"
#include "RollPoints.h"
#include "Judger.h"

#include "cocos2d.h"
#include "GameScene.h"

namespace fc {


    Judger::Judger(){
        
    }

    void Judger::Reset(bool clear) {
        cache_action.status = JS_NONE;
        cache_action.player = EPC_PINK;

        // reset grids
        for (auto grid : GridPool::m){
            grid.second->Reset();
        }

        // reset all player
        PlayerSelecter ps[EPC_MAX] = {fc::PS_DISABLE};
        for (int i = 0; i < EPC_MAX && !clear; ++i) {
            ps[i] = Player::Pool[i].GetStatus();
        }

        if (clear) {
            SetPlaneNumber(EP_MAX);
            RollPoints::GetInstance().Reset();
        }

        Player::ResetAll(ps, plane_number);
        events.clear();
    }

    bool Judger::OnRollPointBegin() {
        if (events.empty() || events.front().status != JS_ROLL_POINTS) {
            return false;
        }

        bool ret = RollPoints::GetInstance().RollPoint();
        if (!ret)
            return ret;

        cache_action = events.front();
        events.front().status = JS_NONE; // 已响应

        return ret;
    }

    bool Judger::OnRollPointEnd(int point) {
        if (events.empty() || events.front().status != JS_NONE) {
            cocos2d::log("[ERROR]: Roll points end but action error.");
            return false;
        }
        cache_points = point;

        // 扔出六的可以多扔一次
        if (6 == point) {
            auto it = events.begin();
            ++it;
            events.insert(it, action_event(JS_ROLL_POINTS, cache_action.player));
        }

        auto iter = events.begin();
        if (6 == point || Player::Pool[iter->player].PlaneRunningNumber()){
            ++iter;
            events.insert(iter, action_event(JS_SELECT_PLANE, cache_action.player));
        }

        DoNext();
        return true;
    }

    bool Judger::OnSelectPlane(Plane& plane) {
        if (events.empty() || events.front().status != JS_SELECT_PLANE) {
            return false;
        }

        // 颜色不匹配
        if (plane.Color() != cache_action.player) {
            return false;
        }

        // 棋子不可用或以胜利
        if (!plane.IsAvailable() || plane.IsWin()) {
            return false;
        }

        // 棋子未出门
        if (6 != cache_points && !plane.IsStarted()) {
            return false;
        }

        cache_action = events.front();
        events.front().status = JS_NONE; // 已响应

        plane.Move(cache_points);
        return true;
    }

    void Judger::DoNext() {
        // 本次action执行完毕，出列
        if (!events.empty() && 
            JS_NONE == events.front().status &&
            !RollPoints::GetInstance().IsRunning()
            ) {
            events.pop_front();
            cocos2d::log("action done, pop.");
        }

        // 进入下一个action
        next_action();
    }

    void Judger::SetPlaneNumber(int n) {
        plane_number = (n - 1) % EP_MAX + 1;
    }

    EnPlayerColor Judger::GetTopActionPlayer() const {
        if (events.empty()) {
            cocos2d::log("[WARNING]: Get top action but judger has no actions.");
            return EPC_GREEN;
        }

        return events.front().player;
    }

    void Judger::next_action() {
        if (events.empty())
            next_player();

        // game over
        if (events.empty())
            return;

        if (JS_ROLL_POINTS == events.front().status)
            RollPoints::GetInstance().OnUpdate();

        // AI
        action_event evt = events.front();
        if (PS_AI != Player::Pool[evt.player].GetStatus()) {
            return;
        }

        // roll points
        if (JS_ROLL_POINTS == evt.status) {
            // 模拟点击骰子
            OnRollPointBegin();

        } else if (JS_SELECT_PLANE == evt.status) {
            // 选择一个飞机
            int plane_score[EP_MAX] = {0}, sel_index = 0;
            for (int i = 0; i < EP_MAX; ++i) {
                Plane& plane = Player::Pool[evt.player][i];
                if (!plane.IsAvailable() || plane.IsWin())
                    continue;

                int rdn = rand() & 0xFF;
                do {
                    // 胜利则跳过
                    if (plane.IsWin()) {
                        plane_score[i] = std::numeric_limits<int>::min();
                        break;
                    }

                    // 不可用则跳过
                    if (!plane.IsAvailable()) {
                        plane_score[i] = std::numeric_limits<int>::min();
                        break;
                    }

                    auto grid = GridPool::Find(plane.GetLocateGridID());
                    // 启动得分
                    if (!plane.IsStarted() && 6 == cache_points) {
                        plane_score[i] = Config::GetInstance().AICfg.Start;
                        plane_score[i] += grid->GetScore(plane.Color(), 1, plane.GetLeftJump(), plane.GetLeftFly());
                        break;
                    }

                    // 普通得分
                    if (plane.IsStarted()) {
                        plane_score[i] = grid->GetScore(plane.Color(), cache_points, plane.GetLeftJump(), plane.GetLeftFly());
                        break;
                    }

                    plane_score[i] = std::numeric_limits<int>::min();
                } while (false);
                if (plane_score[i])
                    plane_score[i] = (plane_score[i] << 8) | rdn;

                if (plane_score[i] > plane_score[sel_index])
                    sel_index = i;
            }

            OnSelectPlane(Player::Pool[evt.player][sel_index]);
        }
    }

    void Judger::next_player() {
        int player_index = (cache_action.player + 1) % EPC_MAX;
        int available_num = 0, select_index = EPC_MAX;
        for (int i = 0; i < EPC_MAX; ++i, player_index = (player_index + 1) % EPC_MAX) {
            if (!Player::Pool[player_index].IsAvailable())
                continue;

            if (Player::Pool[player_index].IsGameOver())
                continue;

            if (0 == available_num)
                select_index = player_index;
            ++available_num;
        }

        if (select_index >= EPC_MAX || available_num < 1) {
            gameover();
            return;
        }

        events.push_back(action_event(JS_ROLL_POINTS, (EnPlayerColor) select_index));
    }

    void Judger::gameover() {
        cocos2d::log("gameover");
        GameScene* game_scene = dynamic_cast<GameScene*>(cocos2d::Director::getInstance()->getRunningScene());
        if (NULL != game_scene)
            game_scene->onGameOver();
    }
}