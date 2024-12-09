#ifndef __GJFACE_SCENE_H__
#define __GJFACE_SCENE_H__

#include "cocos2d.h"

class GJ_Face : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GJ_Face);
    void menuCloseCallback(Ref* pSender);
};

#endif // __START_MENU_SCENE_H__
