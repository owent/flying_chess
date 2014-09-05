#include <assert.h>
#include <cstdlib>
#include "cocos2d.h"

#include "Config.h"

#include "Player.h"

#include "AlterGrid.h"
#include "FlyGrid.h"
#include "StraightGrid.h"
#include "TargetGrid.h"

namespace fc {
    Config::Config() {
        auto cfg_data = cocos2d::FileUtils::getInstance()->getDataFromFile("XML/StaticData.xml");

        tinyxml2::XMLDocument xml_doc;
        xml_doc.Parse((const char*)cfg_data.getBytes(), cfg_data.getSize());

        auto root = xml_doc.FirstChildElement("Root");
        auto func_node = root->FirstChildElement("Grids");
        init_grids(func_node);

        func_node = root->FirstChildElement("Players");
        init_players(func_node);

        func_node = root->FirstChildElement("Plane");
        init_plane(func_node);

        func_node = root->FirstChildElement("Rolls");
        init_rolls(func_node);

        func_node = root->FirstChildElement("AI");
        init_ai(func_node);
    }


    void Config::init_grids(tinyxml2::XMLElement* xml_node) {
        if (NULL == xml_node)
            return;

        GridCfg.Height = GridCfg.Width = 35.0;
        GridCfg.MarginLeft = GridCfg.MarginTop = 37.5;

        {
            auto cfg_text = xml_node->Attribute("Width");
            if (cfg_text)
                GridCfg.Width = strtof(cfg_text, NULL);

            cfg_text = xml_node->Attribute("Height");
            if (cfg_text)
                GridCfg.Height = strtof(cfg_text, NULL);

            cfg_text = xml_node->Attribute("Margin");
            if (cfg_text)
                GridCfg.MarginLeft = GridCfg.MarginTop = strtof(cfg_text, NULL);
        }


        // ´´½¨Æå×ÓÁ´
        auto grid_node = xml_node->FirstChildElement("Grid");
        while (grid_node) {
            int x = grid_node->IntAttribute("X");
            int y = grid_node->IntAttribute("Y");
            int id = grid_node->IntAttribute("ID");
            EnPlayerColor color = (EnPlayerColor) grid_node->IntAttribute("Color");
            const char* name = grid_node->Attribute("Class");
            assert(name);
            assert(!GridPool::Find(id));

            int pre = 0;
            int next = 0;
            int jump = 0;
            int straight = 0;
            int cross = 0;
            {
                auto val_node = grid_node->FirstChildElement("Prev");
                if (val_node)
                    pre = val_node->IntAttribute("ID");
            }

            {
                auto val_node = grid_node->FirstChildElement("Next");
                if (val_node)
                    next = val_node->IntAttribute("ID");
            }

            {
                auto val_node = grid_node->FirstChildElement("Jump");
                if (val_node)
                    jump = val_node->IntAttribute("ID");
            }

            {
                auto val_node = grid_node->FirstChildElement("Straight");
                if (val_node)
                    straight = val_node->IntAttribute("ID");
            }

            {
                auto val_node = grid_node->FirstChildElement("Cross");
                if (val_node)
                    cross = val_node->IntAttribute("ID");
            }


            // ====================================
            if (0 == strcmp("Grid", name)) {
                Grid* g = new Grid(x, y, id, color);
                g->Init(pre, next, jump);
                GridPool::m[id] = GridPool::grid_ptr(g);
            } else if (0 == strcmp("FlyingGrid", name)) {
                Grid* g = new FlyGrid(x, y, id, color, cross);
                g->Init(pre, next, jump);
                GridPool::m[id] = GridPool::grid_ptr(g);
            } else if (0 == strcmp("AlterGrid", name)) {
                Grid* g = new AlterGrid(x, y, id, color, straight);
                g->Init(pre, next, jump);
                GridPool::m[id] = GridPool::grid_ptr(g);
            } else if (0 == strcmp("StraightGrid", name)) {
                Grid* g = new StraightGrid(x, y, id, color);
                g->Init(pre, next, jump);
                GridPool::m[id] = GridPool::grid_ptr(g);
            } else if (0 == strcmp("TargetGrid", name)) {
                Grid* g = new TargetGrid(x, y, id, color);
                g->Init(pre, next, jump);
                GridPool::m[id] = GridPool::grid_ptr(g);
            } else {
                assert(false && "Unsupported grid type");
            }

            grid_node = grid_node->NextSiblingElement("Grid");
        }
    }

    void Config::init_players(tinyxml2::XMLElement* xml_node) {
        if (NULL == xml_node)
            return;

        auto player = xml_node->FirstChildElement("Player");
        for (int i = 0; i < EPC_MAX && player; ++i, player = player->NextSiblingElement("Player")) {
            int home_grids[EP_MAX] = { 0 };
            int color = player->IntAttribute("Color");

            auto plane = player->FirstChildElement("Plane");
            for (int j = 0; j < EP_MAX && plane; ++j, plane = plane->NextSiblingElement("Plane")) {
                home_grids[j] = plane->IntAttribute("Grid");
            }

            Player::Pool[color].Init((EnPlayerColor) color, home_grids);
        }
    }

    void Config::init_plane(tinyxml2::XMLElement* xml_node) {
        if (NULL == xml_node)
            return;

        Plane.MoveSpeed = 10.0;
        Plane.JumpSpeed = 40.0;
        Plane.FlySpeed = 80.0;
        Plane.GoHomeSpeed = 100.0;

        auto cfg_node = xml_node->FirstChildElement("MoveSpeed");
        if (cfg_node)
            Plane.MoveSpeed = strtof(cfg_node->FirstChild()->ToText()->Value(), NULL);

        cfg_node = xml_node->FirstChildElement("JumpSpeed");
        if (cfg_node)
            Plane.JumpSpeed = strtof(cfg_node->FirstChild()->ToText()->Value(), NULL);

        cfg_node = xml_node->FirstChildElement("FlySpeed");
        if (cfg_node)
            Plane.FlySpeed = strtof(cfg_node->FirstChild()->ToText()->Value(), NULL);

        cfg_node = xml_node->FirstChildElement("GoHomeSpeed");
        if (cfg_node)
            Plane.GoHomeSpeed = strtof(cfg_node->FirstChild()->ToText()->Value(), NULL);
    }

    void Config::init_rolls(tinyxml2::XMLElement* xml_node) {
        memset(&RollCfg, 0, sizeof(RollCfg));
        if (NULL == xml_node)
            return;

        RollCfg.Height = xml_node->FloatAttribute("Height");
        RollCfg.Width = xml_node->FloatAttribute("Width");
        RollCfg.Duration = xml_node->FloatAttribute("Duration");

        auto roll = xml_node->FirstChildElement("Roll");
        for (int i = 0; i < EPC_MAX && roll; ++i, roll = roll->NextSiblingElement("Roll")) {
            int color = roll->IntAttribute("Color");

            RollCfg.Rolls[color].color = static_cast<EnPlayerColor>(color);
            RollCfg.Rolls[color].x = roll->IntAttribute("X");
            RollCfg.Rolls[color].y = roll->IntAttribute("Y");
        }
    }

    void Config::init_ai(tinyxml2::XMLElement* xml_node) {
        AICfg.Move = 1;
        AICfg.Jump = 4;
        AICfg.Fly = 12;
        AICfg.KillEnermy = 20;
        AICfg.KillFriend = -50;
        AICfg.Win = 30;
        AICfg.Start = 30;

        if (NULL == xml_node)
            return;

        AICfg.Move = xml_node->IntAttribute("Move");
        AICfg.Jump = xml_node->IntAttribute("Jump");
        AICfg.Fly = xml_node->IntAttribute("Fly");
        AICfg.KillEnermy = xml_node->IntAttribute("KillEnermy");
        AICfg.KillFriend = xml_node->IntAttribute("KillFriend");
        AICfg.Win = xml_node->IntAttribute("Win");
        AICfg.Start = xml_node->IntAttribute("Start");
    }
}