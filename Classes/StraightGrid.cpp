
#include "Config.h"
#include "Plane.h"
#include "StraightGrid.h"

namespace fc {


    StraightGrid::StraightGrid(int x_, int y_, int id_, EnPlayerColor color_) :
        Grid(x_, y_, id_, color_) {
    }

    StraightGrid::grid_ptr StraightGrid::OnPass(Plane& plane, int& left_point) {
        plane.DisableFly();
        plane.DisableJump();

        return Grid::OnPass(plane, left_point);
    }
}