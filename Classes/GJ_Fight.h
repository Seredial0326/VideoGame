#ifndef __GJFIGHT_SCENE_H__
#define __GJFIGHT_SCENE_H__

#include "cocos2d.h"
#include "GameState.h"

class GJFIGHT : public cocos2d::Scene
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

    void checkLife();

    void rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_JabIzqAnimate, cocos2d::Action* GJ_JabDerAnimate, cocos2d::Action* GJ_GBIAnimate, cocos2d::Action* GJ_GBDAnimate, cocos2d::Action* GJ_GRDAnimate, cocos2d::Action* GJ_GRIAnimate, cocos2d::Action* GJ_DISAnimate, cocos2d::Action* GJ_ProvAnimate, cocos2d::Action* GJ_UPAnimate);

    void rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_JabIzqAnimate, cocos2d::Action* GJ_JabDerAnimate, cocos2d::Action* GJ_GBIAnimate, cocos2d::Action* GJ_GBDAnimate, cocos2d::Action* GJ_GRDAnimate, cocos2d::Action* GJ_GRIAnimate, cocos2d::Action* GJ_DISAnimate, cocos2d::Action* GJ_ProvAnimate, cocos2d::Action* GJ_UPAnimate);

    void resolveAttack();

    //void showAttackWarning(const std::string& attackType);

    void playerReact();

    void scheduleRivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur);

    void playRivalIdleAnimation(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_ProvAnimate);

    void checkPlayerReaction(int attackType);

    void applyDamage(int damageToPlayer, int damageToRival);

    void processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_GLPDAnimate, cocos2d::Action* GJ_GLPIAnimate, cocos2d::Action* GJ_GLPBDAnimate, cocos2d::Action* GJ_GLPBIAnimate, cocos2d::Action* GJ_GLPJDAnimate, cocos2d::Action* GJ_GLPJIAnimate, cocos2d::Action* GJ_GLPGDAnimate, cocos2d::Action* GJ_GLPGIAnimate, cocos2d::Action* GJ_ESQD_Animate, cocos2d::Action* GJ_ESQI_Animate, cocos2d::Action* GJ_GuardiaAAnimate, cocos2d::Action* GJ_GuardiaBAnimate, bool& isDefending);

    Scene* StartMenu();

    // implement the "static create()" method manually
    CREATE_FUNC(GJFIGHT);
};

class GJ_Face : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GJ_Face);
    void menuCloseCallback(Ref* pSender);
};

#endif // __HELLOWORLD_SCENE_H__
