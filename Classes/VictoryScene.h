#ifndef __VICTORY_SCENE_H__
#define __VICTORY_SCENE_H__

#include "cocos2d.h"

class VictoryScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    CREATE_FUNC(VictoryScene);
};

#endif // __VICTORY_SCENE_H__
