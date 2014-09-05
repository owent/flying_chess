
#include "Config.h"
#include "Plane.h"
#include "AlterGrid.h"

namespace fc {


    AlterGrid::AlterGrid(int x_, int y_, int id_, EnPlayerColor color_, int straight_grid_) :
        Grid(x_, y_, id_, color_) {
        straight_grid = straight_grid_;
    }

    int AlterGrid::GetScore(EnPlayerColor color, int point, int left_jump, int left_fly) {
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

        return point;
    }

    AlterGrid::grid_ptr AlterGrid::OnPass(Plane& plane, int& left_point, int reason) {
        grid_ptr ret = Grid::OnPass(plane, left_point, reason);
        grid_ptr straight;

        if (Color() == plane.Color()) {
            straight = GridPool::Find(straight_grid);
        }

        return straight ? straight: ret;
    }
}