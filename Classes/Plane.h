#ifndef _FLY_CHESS_PLANE_H_
#define _FLY_CHESS_PLANE_H_

#include <list>
#include "Macro.h"

#include "ui/UILayout.h"

namespace fc {
    class Player;
    class Grid;
    class Plane {
    public:
        // UI
        enum UIActionType {
            UIAT_RUNNING = 0,
            UIAT_MOVE,
            UIAT_DISABLE,
            UIAT_ENABLE
        };
        struct UIAction {
            UIActionType type;
            int reason;
            float duration;
            float delay;
            float param[2];
        };

    public:
        void Init(int home_grid_, EnPlayerColor color_, Player* player_);

        void Reset(bool available);

        bool Move(int point);

        bool MoveTo(int grid_id, int reason);

        bool FlyTo(int grid_id);

        bool JumpTo(int grid_id);

        bool GoHome(int reason, float delay);

        inline bool CanFly() const { return left_fly > 0; }

        inline bool CanJump() const { return left_jump > 0; }

        inline int GetLeftFly() const { return left_fly; }

        inline int GetLeftJump() const { return left_jump; }

        inline EnPlayerColor Color() const { return color; }

        inline bool IsAvailable() const { return IsStarted() || !is_win; }

        inline bool IsWin() const { return is_win; }

        inline bool IsStarted() const { return at_grid != home_grid; }

        inline void DisableFly() { left_fly = 0; }

        inline void DisableJump() { left_jump = 0; }

        inline int GetLocateGridID() const { return at_grid; }
        inline int GetHomeGridID() const { return home_grid; }

        inline float GetSpeed() const { return speed; }

        inline Player& GetPlayer() const { return *player; }

        void SetUI(cocos2d::ui::Layout*);

        void ActiveAction();
        void AddAnimationAction(Grid& g, float duration, float delay, int reason);
        void AddDisableAction(float duration, float delay, int reason);
        void AddEnableAction(float duration, float delay, int reason);


        float GetActionTime() const { return action_left_time; }
        void SetActionTime(float t) { action_left_time = t; }
        float AddActionTime(float t) { return action_left_time += t; }

        void OnActionStart(UIAction* act);
        void OnActionEnd(UIAction* act);

    private:
        int left_fly;
        int left_jump;
        float speed;

        bool is_win;

        int home_grid;
        Player* player;
        EnPlayerColor color;
        int at_grid;

        cocos2d::ui::Layout* m_pUILayout;
        std::list<UIAction> m_stUIActions;
        float action_left_time;

    private:
        void next_action();
    };

}
#endif /* defined(_FLY_CHESS_PLANE_H_) */
