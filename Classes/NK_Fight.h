#ifndef __NKFIGHT_SCENE_H__
#define __NKFIGHT_SCENE_H__

#include "cocos2d.h"
#include "GameState.h"

class NKFIGHT : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    bool isAnimating = false;

    //GameState currentState; // Agrega esta variable

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // Prototipo al inicio de HelloWorldScene.cpp
    static void problemLoading(const char* filename);

    void changeGameState(GameState newState);

    void pauseGame();

    void resumeGame();

    void setupBackground();

    void updateHealthBars();

    void setupHealthBars();

    void setupTimer();

    void reduceFatigue();

    void recoverFatigue(float delta);

    void processAttack();

//NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate

    void rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_JDAnimate, cocos2d::Action* NK_JIAnimate, cocos2d::Action* NK_GADAnimate, cocos2d::Action* NK_GAIAnimate, cocos2d::Action* NK_GBDAnimate, cocos2d::Action* NK_GBIAnimate, cocos2d::Action* NK_GIAnimate, cocos2d::Action* NK_GDAnimate);

    void rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_JDAnimate, cocos2d::Action* NK_JIAnimate, cocos2d::Action* NK_GADAnimate, cocos2d::Action* NK_GAIAnimate, cocos2d::Action* NK_GBDAnimate, cocos2d::Action* NK_GBIAnimate, cocos2d::Action* NK_GIAnimate, cocos2d::Action* NK_GDAnimate);

    void resolveAttack();

    void SupperAttack(cocos2d::Sprite* rivalSprite);

    //void showAttackWarning(const std::string& attackType);

    void playerReact();

    void checkLife(cocos2d::Sprite* rivalSprite, cocos2d::Sprite* playerSprite, cocos2d::Sprite* unoSprite, cocos2d::Action* NK_KOAnimate, cocos2d::Action* NK_LevAnimate, cocos2d::Action* HastaTresAnimate, cocos2d::Action* HastaSeisAnimate, cocos2d::Action* HastaDiezAnimate);

    void applyDamage(int damageToPlayer, int damageToRival);

    //NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate

    void processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_GLPBDAnimate, cocos2d::Action* NK_GLPBIAnimate, cocos2d::Action* NK_GLPJDAnimate, cocos2d::Action* NK_GLPJIAnimate, cocos2d::Action* NK_GLPGDAnimate, cocos2d::Action* NK_GLPGIAnimate, cocos2d::Action* NK_BLBAnimate, cocos2d::Action* NK_BLAAnimate, cocos2d::Action* NK_ESQADAnimate, cocos2d::Action* NK_ESQAIAnimate, cocos2d::Action* NK_ESQIAnimate, cocos2d::Action* NK_ESQDAnimate, bool& isDefending);

    // implement the "static create()" method manually
    CREATE_FUNC(NKFIGHT);
};

class NK_Face : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(NK_Face);
    void menuCloseCallback(Ref* pSender);
};

class NK_Victory : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(NK_Victory);
    void menuCloseCallback(Ref* pSender);
};


#endif // __HELLOWORLD_SCENE_H__
