#ifndef _FLY_CHESS_JUDGER_H_
#define _FLY_CHESS_JUDGER_H_

#include <list>
#include <memory>

#include "Macro.h"
#include "Singleton.h"

namespace fc {

    class Plane;

    class Judger : public Singleton<Judger> {
    public:
        Judger();

        void Reset(bool clear = true);
        bool OnRollPointBegin();
        bool OnRollPointEnd(int point);

        bool OnSelectPlane(Plane& plane);

        void DoNext();

        void SetPlaneNumber(int n);

        EnPlayerColor GetTopActionPlayer() const;
    private:
        void next_action();
        void next_player();
        void gameover();

    private:
        int plane_number;
        struct action_event {
            JudgeStatus status;
            EnPlayerColor player;
            action_event(){}
            action_event(JudgeStatus s, EnPlayerColor p) : status(s), player(p){}
        };
        std::list<action_event> events;
        int cache_points;
        action_event cache_action;
    };

}
#endif /* defined(_FLY_CHESS_FLYGRID_H_) */
