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

        bool RollPoint();

        cocos2d::ui::Layout* CreateUI();

        inline bool IsRunning() const { return m_bIsRunning; }
    private:
        cocos2d::ui::Layout* m_pRollUI;
        bool m_bIsRunning;
    };

}
#endif /* defined(_FLY_CHESS_ROLLPOINTS_H_) */
