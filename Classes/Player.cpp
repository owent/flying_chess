
#include <algorithm>
#include <limits>
#include <numeric>
#include "Player.h"


namespace fc {

    Player Player::Pool[EPC_MAX];
    int Player::PlaneRanking = 0;

    void Player::ResetAll(PlayerSelecter sel[EPC_MAX], int plane_count) {
        for (int i = 0; i < EPC_MAX; ++i) {
            Pool[i].Reset(sel[i], plane_count);
        }
    }

    void Player::OnReadyToStart() {
        PlaneRanking = 0;

        for (int i = 0; i < EPC_MAX; ++i) {
            Pool[i].ranking = std::numeric_limits<int>::max();
        }
    }

    void Player::SortByRanking(EnPlayerColor sel[EPC_MAX]) {
        for (int i = 0; i < EPC_MAX; ++i) {
            sel[i] = static_cast<EnPlayerColor>(i);
        }

        std::sort(&sel[0], &sel[EPC_MAX], [](EnPlayerColor l, EnPlayerColor r){
            auto lf = Player::Pool[l].GetStatus();
            auto rf = Player::Pool[r].GetStatus();

            if (lf && rf)
                return Player::Pool[l].ranking < Player::Pool[r].ranking;

            return lf > rf;
        });
    }

    void Player::Init(EnPlayerColor color, int plane_grid[EP_MAX]) {
        ranking = std::numeric_limits<int>::max();
        status = PS_DISABLE;

        for (int i = 0; i < EP_MAX; ++i) {
            planes[i].Init(plane_grid[i], color, this);
        }
    }

    void Player::Reset(PlayerSelecter status_, int plane_num) {
        status = status_;
        for (int i = 0; i < EP_MAX; ++i) {
            planes[i].Reset(status && i < plane_num);
        }
    }

    bool Player::IsAvailable() const {
        return PS_DISABLE != status;
    }

    bool Player::IsGameOver() const {
        return !IsAvailable() || HasRanking();
    };

    bool Player::HasRanking() const {
        return ranking < std::numeric_limits<int>::max();
    }

    void Player::OnPlaneWin() {
        if (!IsAvailable())
            return;

        for (int i = 0; i < EP_MAX; ++i) {
            if (planes[i].IsAvailable())
                return;
        }

        ranking = ++PlaneRanking;
    }

    void Player::OnGameOver() {
        if (IsAvailable() && !HasRanking())
            ranking = ++PlaneRanking;
    }

    const char* Player::GetPlayerName() const {
        static const char* player_names [] = {
            "Green",
            "Blue",
            "Yellow",
            "Pink"
        };

        return player_names [Color()];
    }

    std::tuple<uint8_t, uint8_t, uint8_t> Player::GetPlayerColor() const {
        static std::tuple<uint8_t, uint8_t, uint8_t> player_color [] = {
            std::make_tuple(0, 150, 68),
            std::make_tuple(0, 133, 200),
            std::make_tuple(245, 185, 0),
            std::make_tuple(220, 47, 129)
        };

        return player_color[Color()];
    }

    int Player::PlaneRunningNumber() const {
        int ret = 0;
        for (int i = 0; i < EP_MAX; ++i) {
            if (planes[i].IsAvailable() && planes[i].IsStarted()) {
                ++ret;
            }
        }

        return ret;
    }

    int Player::PlaneHomeNumber() const {
        int ret = 0;
        for (int i = 0; i < EP_MAX; ++i) {
            if (planes[i].IsAvailable() && !planes[i].IsStarted()) {
                ++ret;
            }
        }

        return ret;
    }

    int Player::PlaneWinNumber() const {
        int ret = 0;
        for (int i = 0; i < EP_MAX; ++i) {
            if (planes[i].IsAvailable() && planes[i].IsWin()) {
                ++ret;
            }
        }

        return ret;
    }

    int Player::PlaneTotalNumber() const {
        int ret = 0;
        for (int i = 0; i < EP_MAX; ++i) {
            if (planes[i].IsAvailable()) {
                ++ret;
            }
        }

        return ret;
    }

    Plane& Player::operator[](int i) {
        return planes [i];
    }

    const Plane& Player::operator[](int i) const {
        return planes[i];
    }
}
