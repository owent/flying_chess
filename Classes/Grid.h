#ifndef _FLY_CHESS_GRID_H_
#define _FLY_CHESS_GRID_H_

#include <map>
#include <memory>

#include "Macro.h"

namespace fc {

    class Plane;

    class Grid {
    public:
        typedef std::shared_ptr<Grid> grid_ptr;

    public:
        Grid(int x_, int y_, int id_, EnPlayerColor color_);
        ~Grid(){}

        void Init(int pre_, int next_, int jump_);


        float GetPositionX();

        float GetPositionY();


        void Reset();


        virtual int GetScore(EnPlayerColor color, int point, int left_jump, int left_fly);

        /**
        * @brief Grid的Leave事件
        */
        virtual void OnLeave(Plane& plane, int reason);


        /**
        * @brief Grid的Pass事件
        */
        virtual grid_ptr OnPass(Plane& plane, int& left_point, int reason);

        /**
        * @brief Grid的Arrive事件
        * @note   -- 击毁已有的Plane
        * @note   -- 计算跳跃
        * @note   -- 计算飞行
        * @note   -- 胜利条件
        */
        virtual void OnArrive(Plane& plane, int reason);

        /**
        * @brief Grid的Cross事件
        * @note   -- 击毁已有的Plane
        */
        virtual void OnCross(Plane& plane, Plane* old_plane, grid_ptr from, grid_ptr to);


        inline int ID() const { return id; }
        inline int X() const { return x; }
        inline int Y() const { return y; }
        inline EnPlayerColor Color() const { return color; }

        grid_ptr GetNextGrid();
        grid_ptr GetPreviousGrid();
        grid_ptr GetJumpGrid();

        void MoveHere(Plane& plane, float speed, float delay_time, int reason);
    private:
        virtual void arrive_event(Plane& plane, Plane* old_plane);

    protected:
        int x;
        int y;
        EnPlayerColor color;
        int id;

        int pre;
        int next;
        int jump;

        Plane* place_plane;
    };

    struct GridPool {
        typedef Grid::grid_ptr grid_ptr;
        static std::map<int, grid_ptr > m;

        static grid_ptr Find(int id);
    };

}
#endif /* defined(_FLY_CHESS_PLAYER_H_) */
