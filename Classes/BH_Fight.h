#ifndef __BHFIGHT_SCENE_H__
#define __BHFIGHT_SCENE_H__

#include "cocos2d.h"

class BHFIGHT : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    bool isAnimating = false;
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // Prototipo al inicio de HelloWorldScene.cpp
    static void problemLoading(const char* filename);

    void setupBackground();

    void updateHealthBars();

    void changeGameState(GameState newState);

    void pauseGame();

    void resumeGame();

    void checkLife();

    void setupHealthBars();

    void setupTimer();

    void reduceFatigue();

    void recoverFatigue(float delta);

    void processAttack();

    void rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur);

    void rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur);

    void resolveAttack();

    //void showAttackWarning(const std::string& attackType);

    void playerReact();

    void scheduleRivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur);

    void playRivalIdleAnimation(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Moving);

    void checkPlayerReaction(int attackType);

    void applyDamage(int damageToPlayer, int damageToRival);

    void processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* protectAction, cocos2d::Action* BH_GPIAnimate, cocos2d::Action* BH_GPDAnimate, cocos2d::Action* BH_GPIBAnimate, cocos2d::Action* BH_GPDBAnimate, cocos2d::Action* BH_EsquivaIzq, cocos2d::Action* BH_EsquivaDer, cocos2d::Action* BH_GUAnimate, cocos2d::Action* BH_Burla, bool& isDefending);

    Scene* StartMenu();
    
    // implement the "static create()" method manually
    CREATE_FUNC(BHFIGHT);
};

class BH_Face : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(BH_Face);
    void menuCloseCallback(Ref* pSender);
};

#endif // __HELLOWORLD_SCENE_H__
