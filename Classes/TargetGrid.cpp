
#include "Config.h"
#include "Plane.h"
#include "TargetGrid.h"

#include "Player.h"

namespace fc {


    TargetGrid::TargetGrid(int x_, int y_, int id_, EnPlayerColor color_) :
        Grid(x_, y_, id_, color_) {
    }

    int TargetGrid::GetScore(EnPlayerColor color, int point, int left_jump, int left_fly) {
        if (0 == point)
            return Config::GetInstance().AICfg.Win;

        return -Grid::GetScore(color, point, left_jump, left_fly);
    }

    TargetGrid::grid_ptr TargetGrid::OnPass(Plane& plane, int& left_point, int reason) {
        plane.DisableFly();
        plane.DisableJump();

        left_point = -left_point;
        return Grid::OnPass(plane, left_point, reason);
    }

    void TargetGrid::arrive_event(Plane& plane, Plane* old_plane) {
        plane.GoHome(LGR_WIN, 0.0f);
        plane.GetPlayer().OnPlaneWin();
    }
}