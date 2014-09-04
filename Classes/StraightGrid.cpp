
#include "Config.h"
#include "Plane.h"
#include "StraightGrid.h"

namespace fc {


    StraightGrid::StraightGrid(int x_, int y_, int id_, EnPlayerColor color_) :
        Grid(x_, y_, id_, color_) {
    }

    int StraightGrid::GetScore(EnPlayerColor color, int point, int left_jump, int left_fly) {
        int ret = Config::GetInstance().AICfg.Move;
        if (0 != point) {
            auto next = GetNextGrid();
            ret += next->GetScore(color, point - point / abs(point), 0, 0);
        }

        return ret;
    }

    StraightGrid::grid_ptr StraightGrid::OnPass(Plane& plane, int& left_point) {
        plane.DisableFly();
        plane.DisableJump();

        return Grid::OnPass(plane, left_point);
    }
}