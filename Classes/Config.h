#ifndef _FLY_CHESS_CONFIG_H_
#define _FLY_CHESS_CONFIG_H_

#include "tinyxml2/tinyxml2.h"

#include "Singleton.h"

#include "Macro.h"

namespace fc {
    struct GridConf {
        float Height;
        float Width;
        float MarginLeft;
        float MarginTop;
    };

    struct PlaneConf {
        float MoveSpeed;
        float JumpSpeed;
        float FlySpeed;
        float GoHomeSpeed;
    };

    struct RollConf {
        float Height;
        float Width;
        float Duration;

        typedef struct {
            int x;
            int y;
            EnPlayerColor color;
        } RollsType[EPC_MAX];

        RollsType Rolls;
    };

    struct AIConf {
        int Move;
        int Jump;
        int Fly;
        int KillEnermy;
        int KillFriend;
        int Win;
        int Start;
    };

    class Config :public Singleton<Config> {
    public:
        Config();

    public:

        PlaneConf Plane;
        GridConf GridCfg;
        RollConf RollCfg;
        AIConf AICfg;

    private:
        void init_grids(tinyxml2::XMLElement* xml_node);
        void init_players(tinyxml2::XMLElement* xml_node);
        void init_plane(tinyxml2::XMLElement* xml_node);
        void init_rolls(tinyxml2::XMLElement* xml_node);
        void init_ai(tinyxml2::XMLElement* xml_node);
    };

}
#endif /* defined(_FLY_CHESS_CONFIG_H_) */
