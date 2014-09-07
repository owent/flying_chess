#include <cstdlib>
#include <cstdio>

#include "Config.h"
#include "SoundMgr.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;


// android effect only support ogg  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
#define MAKE_EFFECT_FILE(x)        x ".ogg"
#define MAKE_BKM_FILE(x)           x ".ogg"
#elif( CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)  
#define MAKE_EFFECT_FILE(x)        x ".m4a"
#define MAKE_BKM_FILE(x)           x ".m4a"
#else  
#define MAKE_EFFECT_FILE(x)        x ".wav"
#define MAKE_BKM_FILE(x)           x ".mp3"
#endif // CC_PLATFORM_ANDROID  

namespace fc {


    SoundMgr::SoundMgr(){
        SimpleAudioEngine::getInstance()->preloadBackgroundMusic(MAKE_BKM_FILE("Sound/bgm"));

        SimpleAudioEngine::getInstance()->preloadEffect(MAKE_EFFECT_FILE("Sound/GoHome"));
        SimpleAudioEngine::getInstance()->preloadEffect(MAKE_EFFECT_FILE("Sound/Jump"));
        SimpleAudioEngine::getInstance()->preloadEffect(MAKE_EFFECT_FILE("Sound/Move1"));
        SimpleAudioEngine::getInstance()->preloadEffect(MAKE_EFFECT_FILE("Sound/Move2"));
        SimpleAudioEngine::getInstance()->preloadEffect(MAKE_EFFECT_FILE("Sound/Move3"));
    }

    void SoundMgr::PlayBackground() {
        SimpleAudioEngine::getInstance()->playBackgroundMusic(MAKE_BKM_FILE("Sound/bgm"), true);
    }

    void SoundMgr::StopBackground() {
        SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    }

    void SoundMgr::PlayGoHome() {
        SimpleAudioEngine::getInstance()->playEffect(MAKE_EFFECT_FILE("Sound/GoHome"));
    }

    void SoundMgr::PlayJump() {
        SimpleAudioEngine::getInstance()->playEffect(MAKE_EFFECT_FILE("Sound/Jump"));
    }

    void SoundMgr::PlayMove() {
        int si = rand() % 3;
        char path[32] = {0};
        sprintf(path, MAKE_EFFECT_FILE("Sound/Move%d"), si + 1);

        SimpleAudioEngine::getInstance()->playEffect(path);
    }
}
