#ifndef _FLY_CHESS_SOUNDMGR_H_
#define _FLY_CHESS_SOUNDMGR_H_

#include <list>
#include <memory>

#include "Macro.h"
#include "Singleton.h"

namespace fc {

    class SoundMgr : public Singleton<SoundMgr> {
    public:
        SoundMgr();

        void PlayBackground();
        void StopBackground();

        void PlayGoHome();

        void PlayJump();

        void PlayMove();
    };

}
#endif /* defined(_FLY_CHESS_FLYGRID_H_) */
