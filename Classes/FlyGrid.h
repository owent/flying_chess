#ifndef _FLY_CHESS_FLYGRID_H_
#define _FLY_CHESS_FLYGRID_H_

#include <map>
#include <memory>

#include "Grid.h"

namespace fc {

    class Plane;

    class FlyGrid : public Grid {
    public:
        typedef Grid::grid_ptr grid_ptr;

    public:
        FlyGrid(int x_, int y_, int id_, EnPlayerColor color_, int cross_grid_);

        virtual int GetScore(EnPlayerColor color, int point, int left_jump, int left_fly);

        void Init(int pre_, int next_, int jump_);

        grid_ptr GetCrossGrid();
    private:
        virtual void arrive_event(Plane& plane);

        int cross_grid;
    };

}
#endif /* defined(_FLY_CHESS_FLYGRID_H_) */
