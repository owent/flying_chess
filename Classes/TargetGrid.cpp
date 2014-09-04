
#include "Config.h"
#include "Plane.h"
#include "TargetGrid.h"

#include "Player.h"

namespace fc {


    TargetGrid::TargetGrid(int x_, int y_, int id_, EnPlayerColor color_) :
        Grid(x_, y_, id_, color_) {
    }

    TargetGrid::grid_ptr TargetGrid::OnPass(Plane& plane, int& left_point) {
        plane.DisableFly();
        plane.DisableJump();

        left_point = -left_point;
        return Grid::OnPass(plane, left_point);
    }

    void TargetGrid::arrive_event(Plane& plane) {
        plane.GoHome(LGR_WIN);
        plane.GetPlayer().OnPlaneWin(++ Player::PlaneRanking);
    }
}