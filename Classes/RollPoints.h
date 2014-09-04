#ifndef _FLY_CHESS_ROLLPOINTS_H_
#define _FLY_CHESS_ROLLPOINTS_H_

#include <list>
#include <memory>

#include "Macro.h"
#include "Singleton.h"

#include "ui/UILayout.h"

namespace fc {
    class RollPoints : public Singleton<RollPoints> {
    public:
        RollPoints();

        void Reset();

        void SetUI(cocos2d::ui::Layout*);

        void OnUpdate();

        void RollPoint();

        cocos2d::ui::Layout* CreateUI();

    private:
        cocos2d::ui::Layout* m_pRollUI;
    };

}
#endif /* defined(_FLY_CHESS_ROLLPOINTS_H_) */
