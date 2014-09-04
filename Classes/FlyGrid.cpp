
#include "Config.h"
#include "Plane.h"
#include "FlyGrid.h"

namespace fc {


    FlyGrid::FlyGrid(int x_, int y_, int id_, EnPlayerColor color_, int cross_grid_):
        Grid(x_, y_, id_, color_) {
        cross_grid = cross_grid_;
    }

    FlyGrid::grid_ptr FlyGrid::GetCrossGrid() {
        return GridPool::Find(cross_grid);
    }

    void FlyGrid::arrive_event(Plane& plane) {
        if (Color() == plane.Color() && plane.CanFly() && jump != ID()) {

            auto cross_grid = GetCrossGrid();
            if (cross_grid)
                cross_grid->OnCross(plane);

            plane.FlyTo(jump);
        }
    }
}