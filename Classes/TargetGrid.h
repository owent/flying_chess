#ifndef _FLY_CHESS_TARGETGRID_H_
#define _FLY_CHESS_TARGETGRID_H_

#include <map>
#include <memory>

#include "Grid.h"

namespace fc {

    class Plane;

    class TargetGrid : public Grid {
    public:
        typedef Grid::grid_ptr grid_ptr;

    public:
        TargetGrid(int x_, int y_, int id_, EnPlayerColor color_);

        virtual int GetScore(EnPlayerColor color, int point, int left_jump, int left_fly);

        /**
        * @brief GridµÄPassÊÂ¼þ
        */
        virtual grid_ptr OnPass(Plane& plane, int& left_point);

    private:
        virtual void arrive_event(Plane& plane);
    };

}
#endif /* defined(_FLY_CHESS_FLYGRID_H_) */
