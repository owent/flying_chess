
#include "Config.h"
#include "Plane.h"
#include "FlyGrid.h"

namespace fc {


    FlyGrid::FlyGrid(int x_, int y_, int id_, EnPlayerColor color_, int cross_grid_):
        Grid(x_, y_, id_, color_) {
        cross_grid = cross_grid_;
    }

    int FlyGrid::GetScore(EnPlayerColor color, int point, int left_jump, int left_fly) {
        if (Color() != color)
            return Grid::GetScore(color, point, left_jump, left_fly);

        if (0 != point) {
            auto next = GetNextGrid();
            return next->GetScore(color, point - point / abs(point), 0, 0) + Config::GetInstance().AICfg.Move;
        }

        int ret = 0;
        if (NULL != place_plane){
            if (place_plane->Color() == color)
                ret += Config::GetInstance().AICfg.KillFriend;
            else
                ret += Config::GetInstance().AICfg.KillEnermy;
        }

        if (left_fly > 0){
            ret += Config::GetInstance().AICfg.Fly;
            auto jump = GetJumpGrid();
            if (jump)
                ret += jump->GetScore(color, point, left_jump, left_fly - 1);
        }

        return point;
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