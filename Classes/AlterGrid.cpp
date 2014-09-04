
#include "Config.h"
#include "Plane.h"
#include "AlterGrid.h"

namespace fc {


    AlterGrid::AlterGrid(int x_, int y_, int id_, EnPlayerColor color_, int straight_grid_) :
        Grid(x_, y_, id_, color_) {
        straight_grid = straight_grid_;
    }

    AlterGrid::grid_ptr AlterGrid::OnPass(Plane& plane, int& left_point) {
        grid_ptr ret = Grid::OnPass(plane, left_point);
        grid_ptr straight;

        if (Color() == plane.Color()) {
            grid_ptr straight = GridPool::Find(straight_grid);
        }

        return straight ? straight: ret;
    }
}