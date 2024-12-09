#ifndef __GAME_OVER_TOTAL_H__
#define __GAME_OVER_TOTAL_H__

#include "cocos2d.h"

class GameOverTotal : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    CREATE_FUNC(GameOverTotal);
};

#endif // __GAME_OVER_SCENE_H__
