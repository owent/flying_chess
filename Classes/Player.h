#ifndef _FLY_CHESS_PLAYER_H_
#define _FLY_CHESS_PLAYER_H_

#include <algorithm>
#include <tuple>
#include <stdint.h>
#include "Macro.h"
#include "Plane.h"

namespace fc {
    class Player {
    public:
        void Init(EnPlayerColor color, int plane_grid[EP_MAX]);

        void Reset(PlayerSelecter status, int plane_num);

        inline EnPlayerColor Color() const { return planes[0].Color(); }

        bool IsAvailable() const;

        bool IsGameOver() const;

        /**
        * @brief 该玩家有飞机到达终点
        */
        void OnPlaneWin(int plane_ranking);

        const char* GetPlayerName() const;

        std::tuple<uint8_t, uint8_t, uint8_t> GetPlayerColor() const;

        int PlaneRunningNumber() const;
        int PlaneHomeNumber() const;
        int PlaneWinNumber() const;
        int PlaneTotalNumber() const;

        inline PlayerSelecter GetStatus() const { return status; }
        inline PlayerSelecter SetStatus(PlayerSelecter ps) { 
            using std::swap; 
            swap(ps, status);
            return ps;
        }

        Plane& operator[](int i);
        const Plane& operator[](int i) const;

    private:
        Plane planes[EP_MAX];
        int ranking;
        PlayerSelecter status;


        // =================== Player Pool ===================
    public:
        static Player Pool[EPC_MAX];
        static int PlaneRanking;

        static void ResetAll(PlayerSelecter sel[EPC_MAX], int plane_count);

        static void SortByRanking(EnPlayerColor sel[EPC_MAX]);
    };

}
#endif /* defined(_FLY_CHESS_PLAYER_H_) */
