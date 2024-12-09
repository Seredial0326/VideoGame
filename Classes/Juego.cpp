#include "NK_Fight.h"
#include "BH_Fight.h"
#include "GJ_Fight.h"
#include "ResourceManager.h"
#include "StartMenuScene.h"
#include "Variables.h"
#include "random"
#include "VictoryScene.h"
#include "GameOverScene.h"
#include "GameOverTotal.h"
#include "Name.h"

USING_NS_CC;

const int HOOK_ATTACK = 1;
const int DOWN_ATTACK = 2;
const int JAB_ATTACK = 3;
const int SUPPER = 4;

const int RECT_ATTACK = 1;
const int ANIMATION_TAUNT = 4;

const int ATTACK_HOOK = 1;
const int ATTACK_DOUBLE = 2;

int vidacountGJ = 3;
int vidacountNK = 3;
int contcaidas = 0;
int contDerrotas = 0;
int currentState = 2;
int BHHealth = 300;

void NKFIGHT::changeGameState(GameState newState) {
    CCLOG("LLAMADO");
    switch (newState) {
    case GameState::START_MENU:
        CCLOG("Estado: Menú principal");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));
        break;

    case GameState::IN_GAME:
        CCLOG("Estado: En el juego");
        this->resumeGame();
        break;

    case GameState::PAUSED:
        CCLOG("Estado: Pausado");
        this->pauseGame();
        break;

    case GameState::GAME_OVER:
        CCLOG("Estado: Juego terminado");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        break;

    case GameState::VICTORY:
        CCLOG("Estado: Victoria");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, VictoryScene::createScene()));
        break;
    }
}

void NKFIGHT::pauseGame() {
    Director::getInstance()->pause();
    auto pauseLayer = LayerColor::create(Color4B(0, 0, 0, 128));
    auto label = Label::createWithTTF("Pausa", "fonts/Marker Felt.ttf", 32);
    label->setPosition(Director::getInstance()->getVisibleSize() / 2);
    pauseLayer->addChild(label);
    this->addChild(pauseLayer, 10, "PauseLayer");
}

void NKFIGHT::resumeGame() {
    Director::getInstance()->resume();
    this->removeChildByName("PauseLayer");
}

void NKFIGHT::updateHealthBars()
{
    if (playerHealth < 0) playerHealth = 1;
    if (playerHealth > 100) playerHealth = 100;
    if (rivalHealth < 0) rivalHealth = 1;
    if (rivalHealth > 100) rivalHealth = 100;

    float playerScaleX = static_cast<float>(playerHealth) * 10 / 100.0f;
    float rivalScaleX = static_cast<float>(rivalHealth) * 10 / 100.0f;
    float playerFatigueScaleX = static_cast<float>(playerFatigue) * 2 / 100.0f;

    playerHealthBar->setScaleX(playerScaleX);
    rivalHealthBar->setScaleX(rivalScaleX);
    playerFatigueBar->setScaleX(playerFatigueScaleX);
}

void NKFIGHT::setupHealthBars()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    playerHealthBar = Sprite::create("HealthBarLM.png");
    playerHealthBar->setAnchorPoint(Vec2(0, 0.5));
    playerHealthBar->setPosition(origin.x + 50, visibleSize.height - 50);
    playerHealthBar->setScaleX(10.0f);
    playerHealthBar->setScaleY(5.0f);
    this->addChild(playerHealthBar, 1);

    rivalHealthBar = Sprite::create("HealthBarBH.png");
    rivalHealthBar->setAnchorPoint(Vec2(0, 0.5));
    rivalHealthBar->setPosition(origin.x + visibleSize.width - 1000, visibleSize.height - 50);
    rivalHealthBar->setScaleX(10.0f);
    rivalHealthBar->setScaleY(5.0f);
    this->addChild(rivalHealthBar, 1);

    playerFatigueBar = Sprite::create("HealthBarLM.png");
    playerFatigueBar->setAnchorPoint(Vec2(0, 0.5));
    playerFatigueBar->setPosition(origin.x + 50, visibleSize.height - 100);
    rivalHealthBar->setScaleX(2.0f);
    playerFatigueBar->setScaleY(1.0f);
    this->addChild(playerFatigueBar, 1);
}

void NKFIGHT::setupTimer() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto counterLabel = Label::createWithTTF("3' 00", "fonts/Marker Felt.ttf", 24);
    counterLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - counterLabel->getContentSize().height));
    counterLabel->setColor(Color3B::WHITE);
    this->addChild(counterLabel, 4);

    this->schedule([this, counterLabel](float delta) {
        if (totalTime > 0) {
            totalTime--;
            int minutes = totalTime / 60;
            int seconds = totalTime % 60;

            std::string timeString = std::to_string(minutes) + "' " +
                (seconds < 10 ? "0" : "") + std::to_string(seconds);
            counterLabel->setString(timeString);
        }
        else {
            contDerrotas++;
            this->unschedule("UpdateCounter");
            counterLabel->setString("¡Fin del tiempo!");
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        }, 1.0f, "UpdateCounter");
}

void NKFIGHT::reduceFatigue()
{
    if (playerFatigue >= fatigueThreshold)
    {
        playerFatigue -= 30;
        CCLOG("Fatiga del jugador: %d", playerFatigue);
        updateHealthBars();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

void NKFIGHT::recoverFatigue(float delta)
{
    if (playerFatigue < 100)
    {
        playerFatigue = std::min(100, playerFatigue + 1); // Recuperación gradual
        updateHealthBars();
    }
}

void NKFIGHT::applyDamage(int damageToPlayer, int damageToRival)
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Reducir salud
    playerHealth = std::max(1, playerHealth - damageToPlayer);
    rivalHealth = std::max(1, rivalHealth - damageToRival);

    // Actualizar barras de salud
    updateHealthBars();

    if (playerHealth == 1) {
        playerHealth = 70;
        contcaidas++;
        if (contcaidas == 3) {
            contcaidas = 0;
            contDerrotas++;
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        if (contDerrotas == 3) {
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverTotal::createScene()));
        }
        updateHealthBars();
    }
}

void NKFIGHT::setupBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("BoxingRing1.png");

    background->setLocalZOrder(0);

    float scaleY = visibleSize.height / background->getContentSize().height;
    background->setScale(scaleY);

    background->setPosition(background->getContentSize().width * scaleY / 2 - 600, visibleSize.height / 2 + origin.y);

    auto clippingNode = ClippingNode::create();
    clippingNode->setContentSize(Size(background->getContentSize().width / 2 * scaleY, visibleSize.height));
    clippingNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    clippingNode->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    auto stencil = DrawNode::create();
    Vec2 rectangle[4] = {
        Vec2(0, 0),
        Vec2(clippingNode->getContentSize().width, 0),
        Vec2(clippingNode->getContentSize().width, clippingNode->getContentSize().height),
        Vec2(0, clippingNode->getContentSize().height)
    };
    stencil->drawPolygon(rectangle, 4, Color4F(1, 1, 1, 1), 0, Color4F(0, 0, 0, 0));
    clippingNode->setStencil(stencil);

    clippingNode->addChild(background);
    this->addChild(clippingNode, -1);

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [background, scaleY](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (keyCode == EventKeyboard::KeyCode::KEY_0) {
            background->setPositionX(background->getPositionX() + 50);
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_1) {
            background->setPositionX(background->getPositionX() - 50); // Mover a la derecha
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void NKFIGHT::rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_JDAnimate, cocos2d::Action* NK_JIAnimate, cocos2d::Action* NK_GADAnimate, cocos2d::Action* NK_GAIAnimate, cocos2d::Action* NK_GBDAnimate, cocos2d::Action* NK_GBIAnimate, cocos2d::Action* NK_GIAnimate, cocos2d::Action* NK_GDAnimate)
{
    // Generar un número aleatorio entre 0 y 99
    int decision = random(0, 99);

    if (decision < 90) // 70% de probabilidad de atacar
    {
        if (rivalFatigue >= rivalFatigueThreshold)
        {
            isReacting = true;
            rivalAttack(rivalSprite, NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate);
            CCLOG("El rival ataca. Fatiga restante: %d", rivalFatigue);
        }
        else
        {
            CCLOG("El rival está demasiado cansado para atacar.");
        }
    }
}

void NKFIGHT::rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_JDAnimate, cocos2d::Action* NK_JIAnimate, cocos2d::Action* NK_GADAnimate, cocos2d::Action* NK_GAIAnimate, cocos2d::Action* NK_GBDAnimate, cocos2d::Action* NK_GBIAnimate, cocos2d::Action* NK_GIAnimate, cocos2d::Action* NK_GDAnimate)
{
    if (isAnimating) return;
    float rivalLifeRatio = (float)rivalHealth / MAX_HEALTH;

    int numran = random(0, 11);

    // Determinar tipo de ataque según la vida del rival
    if (rivalHealth > 40) {
        if (numran >= 6) attackType = DOWN_ATTACK;
        else attackType = JAB_ATTACK;
    }
    else {
        if (numran >= 8) attackType = DOWN_ATTACK;
        else if (numran >= 5) attackType = JAB_ATTACK;
        else attackType = HOOK_ATTACK;
    }


    // Ejecutar acción según el tipo de ataque
    int dos = random(0, 2);
    switch (attackType)
    {
    case HOOK_ATTACK:
        rivalSprite->stopAllActions();
        if (vidacountNK == 1) {
            //auto dobleH = Sequence::create(NK_GADAnimate, NK_GAIAnimate, nullptr);
            rivalSprite->runAction(NK_GADAnimate);
        }
        else {
            if (dos) {
                lado = true;
                rivalSprite->runAction(NK_GADAnimate);
            }
            else {
                lado = false;
                rivalSprite->runAction(NK_GAIAnimate);
            }
        }
        break;
    case JAB_ATTACK:
        rivalSprite->stopAllActions();
        if (vidacountNK == 1) {
            //auto dobleJ = Sequence::create(NK_JDAnimate, NK_JIAnimate, nullptr);
            rivalSprite->runAction(NK_JDAnimate);
        }
        else {
            if (dos) {
                lado = true;
                rivalSprite->runAction(NK_JDAnimate);
            }
            else {
                lado = false;
                rivalSprite->runAction(NK_JIAnimate);
            }
        }
        break;
    case DOWN_ATTACK:
        rivalSprite->stopAllActions();
        if (vidacountNK == 1) {
            //auto dobleD = Sequence::create(NK_GBDAnimate, NK_GBIAnimate, nullptr);
            rivalSprite->runAction(NK_GBDAnimate);
        }
        else {
            if (dos) {
                lado = true;
                rivalSprite->runAction(NK_GBDAnimate);
            }
            else {
                lado = false;
                rivalSprite->runAction(NK_GBIAnimate);
            }
        }
        break;
    }

    this->scheduleOnce([this](float delta) {
        resolveAttack();
        }, 2.0f, "ReactionTimer");

}

void NKFIGHT::resolveAttack()
{
    if (!isReacting) // Si el jugador no reaccionó
    {
        if (attackType == JAB_ATTACK || attackType == DOWN_ATTACK) applyDamage(20, 0);
        else if (attackType == HOOK_ATTACK) applyDamage(30, 0);
        // Reiniciar estado
        isReacting = false;
    }
    else
    {
        playerReact();
    }
}

void NKFIGHT::playerReact()
{
    if (!isReacting) return; // Ignorar si no está en ventana de reacción

    if ((attackType == JAB_ATTACK && lado && reactionAct == 3) ||
        (attackType == JAB_ATTACK && !lado && reactionAct == 3) ||
        (attackType == DOWN_ATTACK && lado && reactionAct == 1) ||
        (attackType == DOWN_ATTACK && !lado && reactionAct == 2) ||
        (attackType == HOOK_ATTACK && lado && reactionAct == 1) ||
        (attackType == HOOK_ATTACK && !lado && reactionAct == 2))
    {
        CCLOG("¡Reaccionaste correctamente!");
        isReacting = false; // Detener el ataque
    }
    else
    {
        if (attackType == JAB_ATTACK || attackType == DOWN_ATTACK) applyDamage(20, 0);
        else if (attackType == HOOK_ATTACK) applyDamage(30, 0);
        // Reiniciar estado
        isReacting = false;
        CCLOG("Reacción incorrecta, recibirás daño.");
    }
}

void NKFIGHT::SupperAttack(cocos2d::Sprite* rivalSprite) {

    // Define la animación de movimiento del rival
    if (!isAnimating) // Solo si no está en otra acción

        isAnimating = true;
    isReacting = true;

    auto callback = CallFunc::create([=]() {
        isAnimating = false;
        isReacting = false;
        });
    {
        auto NK_SUPER2 = ResourceManager::getInstance()->getSpriteFrame("NK20");
        auto NK_SUPER3 = ResourceManager::getInstance()->getSpriteFrame("NK21");
        auto NK_SUPER4 = ResourceManager::getInstance()->getSpriteFrame("NK22");
        auto NK_SUPER5 = ResourceManager::getInstance()->getSpriteFrame("NK52");
        auto NK_SUPER6 = ResourceManager::getInstance()->getSpriteFrame("NK53");
        auto NK_SUPER7 = ResourceManager::getInstance()->getSpriteFrame("NK48");
        auto NK_SUPER8 = ResourceManager::getInstance()->getSpriteFrame("NK50");
        auto NK_SUPER9 = ResourceManager::getInstance()->getSpriteFrame("NK13");
        auto NK_SUPER10 = ResourceManager::getInstance()->getSpriteFrame("NK49");
        auto NK_SUPER11 = ResourceManager::getInstance()->getSpriteFrame("NK51");
        auto NK_SUPER12 = ResourceManager::getInstance()->getSpriteFrame("NK5");

        Vector<SpriteFrame*> SUPFrames = { NK_SUPER2, NK_SUPER3, NK_SUPER4, NK_SUPER5, NK_SUPER6, NK_SUPER7, NK_SUPER8, NK_SUPER9, NK_SUPER10, NK_SUPER11, NK_SUPER12 };
        auto SUPAnimacion = Animation::createWithSpriteFrames(SUPFrames, 0.3f);
        auto SUPAnimate = Animate::create(SUPAnimacion);
        SUPAnimate->retain();

        auto moveBy = MoveBy::create(1.0f, Vec2(0, +150)); // Moverse ligeramente hacia la derecha
        auto moveBack = MoveBy::create(1.0f, Vec2(0, -150));              // Regresar a la posición original

        // Secuencia completa
        auto Super = Sequence::create(moveBy, SUPAnimate, moveBack, callback, nullptr);

        rivalSprite->runAction(Super);
    }
}

void NKFIGHT::processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_GLPBDAnimate, cocos2d::Action* NK_GLPBIAnimate, cocos2d::Action* NK_GLPJDAnimate, cocos2d::Action* NK_GLPJIAnimate, cocos2d::Action* NK_GLPGDAnimate, cocos2d::Action* NK_GLPGIAnimate, cocos2d::Action* NK_BLBAnimate, cocos2d::Action* NK_BLAAnimate, cocos2d::Action* NK_ESQADAnimate, cocos2d::Action* NK_ESQAIAnimate, cocos2d::Action* NK_ESQIAnimate, cocos2d::Action* NK_ESQDAnimate, bool& isDefending) {

    int randomValue = random(0, 100);

    if (playerAction == 1) { // Supongamos que 1 es un ataque del jugador
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            rivalSprite->stopAllActions();
            isDefending = true;
            rivalSprite->runAction(NK_ESQIAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPJIAnimate);
            applyDamage(0, 10);
        }
    }
    else if (playerAction == 2) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_ESQDAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPJDAnimate);
        }
    }
    else if (playerAction == 3) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_ESQAIAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPGDAnimate);
        }
    }
    else if (playerAction == 4) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_ESQADAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPGIAnimate);
        }
    }
    else if (playerAction == 5) {
        if (randomValue < 100) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_BLAAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPGDAnimate);
        }
    }
    else if (playerAction == 6) {
        if (randomValue < 40) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_BLBAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPBDAnimate);
        }
    }
    else if (playerAction == 7) {
        if (randomValue < 40) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_BLBAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(NK_GLPBIAnimate);
        }
    }
}

void NKFIGHT::processAttack()
{
    if (playerFatigue >= fatigueThreshold)
    {
        reduceFatigue();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

void NKFIGHT::checkLife(cocos2d::Sprite* rivalSprite, cocos2d::Sprite* playerSprite, cocos2d::Sprite* unoSprite, cocos2d::Action* NK_KOAnimate, cocos2d::Action* NK_LevAnimate, cocos2d::Action* HastaTresAnimate, cocos2d::Action* HastaSeisAnimate, cocos2d::Action* HastaDiezAnimate) {
    
    if (rivalHealth == 1) {
        isAnimating = true;
        isReacting = true;
        
        auto delay = cocos2d::DelayTime::create(2.0f); // Pausa de 2 segundos
        auto callback = cocos2d::CallFunc::create([]() {
            CCLOG("¡Se ejecutó después del delay!");
        });
        auto sequence = cocos2d::Sequence::create(delay, callback, nullptr);

        rivalSprite->stopAllActions();
        rivalSprite->runAction(NK_KOAnimate);
        rivalSprite->runAction(sequence);

        for (int i = 0; i < 7; i++) {
            rivalHealth += 10;
            updateHealthBars();
        }
        rivalSprite->stopAllActions();
        rivalSprite->runAction(NK_LevAnimate);
        rivalSprite->runAction(sequence);

        isAnimating = false;
        isReacting = false;
    }
}

Scene* NKFIGHT::createScene()
{
    return NKFIGHT::create();
}

bool NKFIGHT::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    setupBackground();
    setupTimer();
    setupHealthBars();

    // Recuperar fatiga con el tiempo
    this->schedule([this](float delta) {
        recoverFatigue(delta);
        }, 0.05f, "RecoverFatigue");

    // Obtener los SpriteFrame desde el ResourceManager
    auto LM_PI = ResourceManager::getInstance()->getSpriteFrame("LM_PI");
    auto LM_JabIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq1");
    auto LM_JabIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq2");
    auto LM_JabDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer1");
    auto LM_JabDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer2");

    Vector<SpriteFrame*> jabIzqFrames = { LM_JabIzq1, LM_JabIzq2, LM_PI };
    auto jabIzqAnimacion = Animation::createWithSpriteFrames(jabIzqFrames, 0.3f);
    auto jabIzqAnimate = Animate::create(jabIzqAnimacion);
    jabIzqAnimate->retain();

    Vector<SpriteFrame*> jabDerFrames = { LM_JabDer1, LM_JabDer2, LM_PI };
    auto jabDerAnimacion = Animation::createWithSpriteFrames(jabDerFrames, 0.3f);
    auto jabDerAnimate = Animate::create(jabDerAnimacion);
    jabDerAnimate->retain();

    auto LM_GanIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq1");
    auto LM_GanIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq2");
    auto LM_GanIzq3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq3");
    auto LM_GanIzq4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq4");
    auto LM_GanDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer1");
    auto LM_GanDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer2");
    auto LM_GanDer3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer3");
    auto LM_GanDer4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer4");

    Vector<SpriteFrame*> GanDerFrames = { LM_GanDer1, LM_GanDer2, LM_GanDer3, LM_GanDer4, LM_PI };
    auto GanDerAnimacion = Animation::createWithSpriteFrames(GanDerFrames, 0.3f);
    auto GanDerAnimate = Animate::create(GanDerAnimacion);
    GanDerAnimate->retain();

    Vector<SpriteFrame*> GanIzqFrames = { LM_GanIzq1, LM_GanIzq2, LM_GanIzq3, LM_GanIzq4, LM_PI };
    auto GanIzqAnimacion = Animation::createWithSpriteFrames(GanIzqFrames, 0.3f);
    auto GanIzqAnimate = Animate::create(GanIzqAnimacion);
    GanIzqAnimate->retain();

    auto LM_GAMD1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_GAMD2 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");

    Vector<SpriteFrame*> UpperCutFrames = { LM_GAMD1, LM_GAMD2, LM_PI };
    auto UpperCutAnimacion = Animation::createWithSpriteFrames(UpperCutFrames, 0.6f);
    auto UpperCutAnimate = Animate::create(UpperCutAnimacion);
    UpperCutAnimate->retain();

    auto LM_GBI2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI2");
    auto LM_GBI1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI1");
    auto LM_GBD2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD2");
    auto LM_GBD1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD1");

    Vector<SpriteFrame*> GBIFrames = { LM_GBI1, LM_GBI2, LM_PI };
    auto GBIAnimacion = Animation::createWithSpriteFrames(GBIFrames, 0.4f);
    auto GBIAnimate = Animate::create(GBIAnimacion);
    GBIAnimate->retain();

    Vector<SpriteFrame*> GBDFrames = { LM_GBD1, LM_GBD2, LM_PI };
    auto GBDAnimacion = Animation::createWithSpriteFrames(GBDFrames, 0.4f);
    auto GBDAnimate = Animate::create(GBDAnimacion);
    GBDAnimate->retain();

    auto LM_GR = ResourceManager::getInstance()->getSpriteFrame("LM_GR");
    auto LM_ES = ResourceManager::getInstance()->getSpriteFrame("LM_ES");
    auto LM_KOI2 = ResourceManager::getInstance()->getSpriteFrame("LM_KOI2");

    auto LM_EI = ResourceManager::getInstance()->getSpriteFrame("LM_EI");
    auto LM_ED = ResourceManager::getInstance()->getSpriteFrame("LM_ED");

    Vector<SpriteFrame*> EsquivaIFrames = { LM_EI, LM_PI };
    auto EsquivaIAnimacion = Animation::createWithSpriteFrames(EsquivaIFrames, 1.0f);
    auto EsquivaIAnimate = Animate::create(EsquivaIAnimacion);
    EsquivaIAnimate->retain();

    Vector<SpriteFrame*> EsquivaDFrames = { LM_ED, LM_PI };
    auto EsquivaDAnimacion = Animation::createWithSpriteFrames(EsquivaDFrames, 1.0f);
    auto EsquivaDAnimate = Animate::create(EsquivaDAnimacion);
    EsquivaDAnimate->retain();

    auto LM_DD = ResourceManager::getInstance()->getSpriteFrame("LM_DD");
    auto LM_KOD1 = ResourceManager::getInstance()->getSpriteFrame("LM_KOD1");

    auto LM_S1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_S2 = ResourceManager::getInstance()->getSpriteFrame("LM_S2");
    auto LM_S3 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");
    auto LM_S4 = ResourceManager::getInstance()->getSpriteFrame("LM_S4");
    auto LM_S5 = ResourceManager::getInstance()->getSpriteFrame("LM_S5");

    Vector<SpriteFrame*> SuperFrames = { LM_S1, LM_S2, LM_S3, LM_S4, LM_S5, LM_PI };
    auto SuperAnimacion = Animation::createWithSpriteFrames(SuperFrames, 0.2f);
    auto SuperAnimate = Animate::create(SuperAnimacion);
    SuperAnimate->retain();

    Vector<SpriteFrame*> GoDownFrames = { LM_GR, LM_PI };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR, LM_PI };
    auto BloqAnimacion = Animation::createWithSpriteFrames(BloqFrames, 1.0f);
    auto BloqAnimate = Animate::create(BloqAnimacion);
    BloqAnimate->retain();

    auto NK_ANIM1 = ResourceManager::getInstance()->getSpriteFrame("NK1");
    auto NK_ANIM2 = ResourceManager::getInstance()->getSpriteFrame("NK2");
    auto NK_ANIM3 = ResourceManager::getInstance()->getSpriteFrame("NK3");
    auto NK_ANIM4 = ResourceManager::getInstance()->getSpriteFrame("NK4");

    auto NK_ANIM5 = ResourceManager::getInstance()->getSpriteFrame("NK5");
    auto NK_PI = ResourceManager::getInstance()->getSpriteFrame("NK6");
    auto NK_PREPG = ResourceManager::getInstance()->getSpriteFrame("NK7");
    auto NK_MOV = ResourceManager::getInstance()->getSpriteFrame("NK8");

    auto NK_JABI1 = ResourceManager::getInstance()->getSpriteFrame("NK9");
    auto NK_JABI2 = ResourceManager::getInstance()->getSpriteFrame("NK11");
    auto NK_JABI3 = ResourceManager::getInstance()->getSpriteFrame("NK12");

    auto NK_JABD1 = ResourceManager::getInstance()->getSpriteFrame("NK47");
    auto NK_JABD2 = ResourceManager::getInstance()->getSpriteFrame("NK48");
    auto NK_JABD3 = ResourceManager::getInstance()->getSpriteFrame("NK49");

    auto NK_RI2 = ResourceManager::getInstance()->getSpriteFrame("NK10");
    auto NK_RD2 = ResourceManager::getInstance()->getSpriteFrame("NK50");

    auto NK_GD3 = ResourceManager::getInstance()->getSpriteFrame("NK13");
    auto NK_GI3 = ResourceManager::getInstance()->getSpriteFrame("NK51");

    auto NK_GBD1 = ResourceManager::getInstance()->getSpriteFrame("NK14");
    auto NK_GBD2 = ResourceManager::getInstance()->getSpriteFrame("NK15");
    auto NK_GBD3 = ResourceManager::getInstance()->getSpriteFrame("NK17");
    auto NK_GBI1 = ResourceManager::getInstance()->getSpriteFrame("NK63");
    auto NK_GBI2 = ResourceManager::getInstance()->getSpriteFrame("NK64");
    auto NK_GBI3 = ResourceManager::getInstance()->getSpriteFrame("NK65");

    auto NK_GPAD1 = ResourceManager::getInstance()->getSpriteFrame("NK18");
    auto NK_GPAD2 = ResourceManager::getInstance()->getSpriteFrame("NK54");
    auto NK_GPAI1 = ResourceManager::getInstance()->getSpriteFrame("NK55");
    auto NK_GPAI2 = ResourceManager::getInstance()->getSpriteFrame("NK19");

    auto NK_GB = ResourceManager::getInstance()->getSpriteFrame("NK23");

    auto NK_GAD = ResourceManager::getInstance()->getSpriteFrame("NK24");
    auto NK_ESQAD = ResourceManager::getInstance()->getSpriteFrame("NK25");
    auto NK_GAI = ResourceManager::getInstance()->getSpriteFrame("NK56");
    auto NK_ESQAI = ResourceManager::getInstance()->getSpriteFrame("NK57");

    auto NK_GLPJI = ResourceManager::getInstance()->getSpriteFrame("NK26");
    auto NK_GLPGI = ResourceManager::getInstance()->getSpriteFrame("NK27");
    auto NK_GLPJD = ResourceManager::getInstance()->getSpriteFrame("NK58");
    auto NK_GLPGD = ResourceManager::getInstance()->getSpriteFrame("NK59");
    //auto GJ28 = ResourceManager::getInstance()->getSpriteFrame("NK28");

    auto NK_ESQDER = ResourceManager::getInstance()->getSpriteFrame("NK29");
    auto NK_GLPSD = ResourceManager::getInstance()->getSpriteFrame("NK30");
    auto NK_GLPBD = ResourceManager::getInstance()->getSpriteFrame("NK31");

    auto NK_ESQIZQ = ResourceManager::getInstance()->getSpriteFrame("NK60");
    auto NK_GLPSI = ResourceManager::getInstance()->getSpriteFrame("NK61");
    auto NK_GLPBI = ResourceManager::getInstance()->getSpriteFrame("NK62");
    //auto GJ32 = ResourceManager::getInstance()->getSpriteFrame("NK32");

    Vector<SpriteFrame*> NK_ESQDFrames = { NK_ESQDER, NK_PI };
    auto NK_ESQDAnimacion = Animation::createWithSpriteFrames(NK_ESQDFrames, 1.0f);
    auto NK_ESQDAnimate = Animate::create(NK_ESQDAnimacion);
    NK_ESQDAnimate->retain();

    Vector<SpriteFrame*> NK_ESQIFrames = { NK_ESQIZQ, NK_PI };
    auto NK_ESQIAnimacion = Animation::createWithSpriteFrames(NK_ESQIFrames, 1.0f);
    auto NK_ESQIAnimate = Animate::create(NK_ESQIAnimacion);
    NK_ESQIAnimate->retain();

    Vector<SpriteFrame*> NK_ESQADFrames = { NK_ESQAD, NK_PI };
    auto NK_ESQADAnimacion = Animation::createWithSpriteFrames(NK_ESQADFrames, 1.0f);
    auto NK_ESQADAnimate = Animate::create(NK_ESQADAnimacion);
    NK_ESQADAnimate->retain();

    Vector<SpriteFrame*> NK_ESQAIFrames = { NK_ESQAI, NK_PI };
    auto NK_ESQAIAnimacion = Animation::createWithSpriteFrames(NK_ESQAIFrames, 1.0f);
    auto NK_ESQAIAnimate = Animate::create(NK_ESQAIAnimacion);
    NK_ESQAIAnimate->retain();

    Vector<SpriteFrame*> NK_BLBFrames = { NK_GB, NK_PI };
    auto NK_BLBAnimacion = Animation::createWithSpriteFrames(NK_BLBFrames, 1.0f);
    auto NK_BLBAnimate = Animate::create(NK_BLBAnimacion);
    NK_BLBAnimate->retain();

    Vector<SpriteFrame*> NK_BLAFrames = { NK_GAD, NK_PI };
    auto NK_BLAAnimacion = Animation::createWithSpriteFrames(NK_BLAFrames, 1.0f);
    auto NK_BLAAnimate = Animate::create(NK_BLAAnimacion);
    NK_BLAAnimate->retain();

    Vector<SpriteFrame*> NK_GLPBDFrames = { NK_GLPBD, NK_PI };
    auto NK_GLPBDAnimacion = Animation::createWithSpriteFrames(NK_GLPBDFrames, 1.0f);
    auto NK_GLPBDAnimate = Animate::create(NK_GLPBDAnimacion);
    NK_GLPBDAnimate->retain();

    Vector<SpriteFrame*> NK_GLPBIFrames = { NK_GLPBI, NK_PI };
    auto NK_GLPBIAnimacion = Animation::createWithSpriteFrames(NK_GLPBIFrames, 1.0f);
    auto NK_GLPBIAnimate = Animate::create(NK_GLPBIAnimacion);
    NK_GLPBIAnimate->retain();

    Vector<SpriteFrame*> NK_GLPJDFrames = { NK_GLPJD, NK_PI };
    auto NK_GLPJDAnimacion = Animation::createWithSpriteFrames(NK_GLPJDFrames, 1.0f);
    auto NK_GLPJDAnimate = Animate::create(NK_GLPJDAnimacion);
    NK_GLPJDAnimate->retain();

    Vector<SpriteFrame*> NK_GLPJIFrames = { NK_GLPJI, NK_PI };
    auto NK_GLPJIAnimacion = Animation::createWithSpriteFrames(NK_GLPJIFrames, 1.0f);
    auto NK_GLPJIAnimate = Animate::create(NK_GLPJIAnimacion);
    NK_GLPJIAnimate->retain();

    Vector<SpriteFrame*> NK_GLPGDFrames = { NK_GLPGD, NK_PI };
    auto NK_GLPGDAnimacion = Animation::createWithSpriteFrames(NK_GLPGDFrames, 1.0f);
    auto NK_GLPGDAnimate = Animate::create(NK_GLPGDAnimacion);
    NK_GLPGDAnimate->retain();

    Vector<SpriteFrame*> NK_GLPGIFrames = { NK_GLPGI, NK_PI };
    auto NK_GLPGIAnimacion = Animation::createWithSpriteFrames(NK_GLPGIFrames, 1.0f);
    auto NK_GLPGIAnimate = Animate::create(NK_GLPGIAnimacion);
    NK_GLPGIAnimate->retain();

    Vector<SpriteFrame*> NK_GLPSFrames = { NK_GLPSD, NK_GLPSI, NK_PI };
    auto NK_GLPSAnimacion = Animation::createWithSpriteFrames(NK_GLPSFrames, 1.0f);
    auto NK_GLPSAnimate = Animate::create(NK_GLPSAnimacion);
    NK_GLPSAnimate->retain();

    Vector<SpriteFrame*> NK_GDFrames = { NK_JABD1, NK_GBD1, NK_GD3, NK_PI };
    auto NK_GDAnimacion = Animation::createWithSpriteFrames(NK_GDFrames, 1.0f);
    auto NK_GDAnimate = Animate::create(NK_GDAnimacion);
    NK_GDAnimate->retain();

    Vector<SpriteFrame*> NK_GIFrames = { NK_JABI1, NK_GBI1, NK_GI3, NK_PI };
    auto NK_GIAnimacion = Animation::createWithSpriteFrames(NK_GIFrames, 1.0f);
    auto NK_GIAnimate = Animate::create(NK_GIAnimacion);
    NK_GIAnimate->retain();

    Vector<SpriteFrame*> NK_JDFrames = { NK_JABD1, NK_JABD2, NK_JABD3, NK_PI };
    auto NK_JDAnimacion = Animation::createWithSpriteFrames(NK_JDFrames, 1.0f);
    auto NK_JDAnimate = Animate::create(NK_JDAnimacion);
    NK_JDAnimate->retain();

    Vector<SpriteFrame*> NK_JIFrames = { NK_JABI1, NK_JABI2, NK_JABI3, NK_PI };
    auto NK_JIAnimacion = Animation::createWithSpriteFrames(NK_JIFrames, 1.0f);
    auto NK_JIAnimate = Animate::create(NK_JIAnimacion);
    NK_JIAnimate->retain();

    Vector<SpriteFrame*> NK_GBDFrames = { NK_GBD1, NK_GBD2, NK_GBD3, NK_PI };
    auto NK_GBDAnimacion = Animation::createWithSpriteFrames(NK_GBDFrames, 1.0f);
    auto NK_GBDAnimate = Animate::create(NK_GBDAnimacion);
    NK_GBDAnimate->retain();

    Vector<SpriteFrame*> NK_GBIFrames = { NK_GBI1, NK_GBI2, NK_GBI3, NK_PI };
    auto NK_GBIAnimacion = Animation::createWithSpriteFrames(NK_GBIFrames, 1.0f);
    auto NK_GBIAnimate = Animate::create(NK_GBIAnimacion);
    NK_GBIAnimate->retain();

    Vector<SpriteFrame*> NK_GADFrames = { NK_GPAD1, NK_GPAD2, NK_PI };
    auto NK_GADAnimacion = Animation::createWithSpriteFrames(NK_GADFrames, 1.0f);
    auto NK_GADAnimate = Animate::create(NK_GADAnimacion);
    NK_GADAnimate->retain();

    Vector<SpriteFrame*> NK_GAIFrames = { NK_GPAI1, NK_GPAI2, NK_PI };
    auto NK_GAIAnimacion = Animation::createWithSpriteFrames(NK_GAIFrames, 1.0f);
    auto NK_GAIAnimate = Animate::create(NK_GAIAnimacion);
    NK_GAIAnimate->retain();

    auto rivalSprite = Sprite::createWithSpriteFrame(NK_PI);
    rivalSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    this->addChild(rivalSprite);

    auto playerSprite = Sprite::createWithSpriteFrame(LM_PI);
    playerSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 250));
    this->addChild(playerSprite);

    playerSprite->setLocalZOrder(3);

    rivalSprite->setLocalZOrder(2);

    playerHealthBar->setLocalZOrder(1);
    rivalHealthBar->setLocalZOrder(1);

    auto uno = ResourceManager::getInstance()->getSpriteFrame("1");
    auto dos = ResourceManager::getInstance()->getSpriteFrame("2");
    auto tres = ResourceManager::getInstance()->getSpriteFrame("3");
    auto cuatro = ResourceManager::getInstance()->getSpriteFrame("4");
    auto cinco = ResourceManager::getInstance()->getSpriteFrame("5");
    auto seis = ResourceManager::getInstance()->getSpriteFrame("6");
    auto siete = ResourceManager::getInstance()->getSpriteFrame("7");
    auto ocho = ResourceManager::getInstance()->getSpriteFrame("8");
    auto nueve = ResourceManager::getInstance()->getSpriteFrame("9");
    auto diez = ResourceManager::getInstance()->getSpriteFrame("10");

    Vector<SpriteFrame*> HastaTresFr = { uno, dos, tres };
    auto HastaTresAnimacion = Animation::createWithSpriteFrames(HastaTresFr, 1.0f);
    auto HastaTresAnimate = Animate::create(HastaTresAnimacion);
    HastaTresAnimate->retain();

    Vector<SpriteFrame*> HastaSeisFr = { uno, dos, tres, cuatro, cinco, seis };
    auto HastaSeisAnimacion = Animation::createWithSpriteFrames(HastaSeisFr, 1.0f);
    auto HastaSeisAnimate = Animate::create(HastaSeisAnimacion);
    HastaSeisAnimate->retain();

    Vector<SpriteFrame*> HastaDiezFr = { uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve, diez };
    auto HastaDiezAnimacion = Animation::createWithSpriteFrames(HastaDiezFr, 1.0f);
    auto HastaDiezAnimate = Animate::create(HastaDiezAnimacion);
    HastaDiezAnimate->retain();

    auto unoSprite = Sprite::createWithSpriteFrame(uno);
    unoSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    this->addChild(unoSprite,5);


    auto NK_KO1 = ResourceManager::getInstance()->getSpriteFrame("NK33");
    auto NK_KO2 = ResourceManager::getInstance()->getSpriteFrame("NK34");
    auto NK_KO3 = ResourceManager::getInstance()->getSpriteFrame("NK35");
    auto NK_LEV1 = ResourceManager::getInstance()->getSpriteFrame("NK36");
    auto NK_LEV2 = ResourceManager::getInstance()->getSpriteFrame("NK37");
    auto NK_LEV3 = ResourceManager::getInstance()->getSpriteFrame("NK38");
    auto NK_LEV4 = ResourceManager::getInstance()->getSpriteFrame("NK39");
    auto NK_LEV5 = ResourceManager::getInstance()->getSpriteFrame("NK42");
    auto NK_LEV6 = ResourceManager::getInstance()->getSpriteFrame("NK43");
    auto NK_LEV7 = ResourceManager::getInstance()->getSpriteFrame("NK44");

    Vector<SpriteFrame*> NK_KOFr = { NK_KO1, NK_KO2, NK_KO3 };
    auto NK_KOAnimacion = Animation::createWithSpriteFrames(NK_KOFr, 1.0f);
    auto NK_KOAnimate = Animate::create(NK_KOAnimacion);
    NK_KOAnimate->retain();

    Vector<SpriteFrame*> NK_LevFr = { NK_KO3, NK_LEV1, NK_LEV2, NK_LEV3, NK_LEV4, NK_LEV5, NK_LEV6, NK_LEV7 };
    auto NK_LevAnimacion = Animation::createWithSpriteFrames(NK_LevFr, 1.0f);
    auto NK_LevAnimate = Animate::create(NK_LevAnimacion);
    NK_LevAnimate->retain();

    unoSprite->setVisible(false);

    this->schedule([this, rivalSprite, NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate](float delta) {
        if (!isAnimating && !isReacting) {
            rivalAction(rivalSprite, NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate);
        }
    }, 1.5f, "RivalActionScheduler");

    this->scheduleOnce([this](float delta) {
        if (rivalFatigue < 100)
        {
            rivalFatigue = std::min(100, rivalFatigue + 3);
            CCLOG("Fatiga del rival recuperada a: %d", rivalFatigue);
        }
        }, 3.0f, "RecoverRivalFatigue");

    bool isDefending = false;

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (isAnimating) return;
        if (playerFatigue < fatigueThreshold) {
            CCLOG("¡Demasiado cansado para atacar!");
            return;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_Q) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Izquierdo)");
            isAnimating = true;

            processAI(1, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_E) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Derecho)");
            isAnimating = true;

            processAI(2, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Derecho)");
            isAnimating = true;

            processAI(3, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(4, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (UpperCut)");
            isAnimating = true;

            processAI(5, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(UpperCutAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 30);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_Z) {
            CCLOG("El jugador lanza un ataque (Golpe Bajo Izquierdo)");
            isAnimating = true;

            processAI(6, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBIAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_C) {
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(7, rivalSprite, NK_GLPBDAnimate, NK_GLPBIAnimate, NK_GLPJDAnimate, NK_GLPJIAnimate, NK_GLPGDAnimate, NK_GLPGIAnimate, NK_BLBAnimate, NK_BLAAnimate, NK_ESQADAnimate, NK_ESQAIAnimate, NK_ESQIAnimate, NK_ESQDAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBDAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(-10, 0));
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, EsquivaIAnimate, moveBack, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(10, 0));
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(EsquivaDAnimate, callback, nullptr));
            reactionAct = 2;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_R) {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(BloqAnimate, callback, nullptr));
            reactionAct = 3;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
            isReacting = true;
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(0, -200));
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, GoDownAnimate, moveBack, callback, nullptr));
            reactionAct = 4;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_P) {
            if (currentState == 2) {
                currentState = 3;
                changeGameState(GameState::PAUSED);
                CCLOG("Pausa");
            }
            else if (currentState == 3) {
                changeGameState(GameState::IN_GAME);
                currentState = 2;
            }
        }

        checkLife(rivalSprite, playerSprite, unoSprite, NK_KOAnimate, NK_LevAnimate, HastaTresAnimate, HastaSeisAnimate, HastaDiezAnimate);
        if (vidacountNK == 0) {
            playerSprite->stopAllActions();
            Director::getInstance()->replaceScene(TransitionFade::create(2.0f, NK_Victory::createScene()));
        }

        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

    return true;
};

void NKFIGHT::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}



void BHFIGHT::changeGameState(GameState newState) {
    CCLOG("LLAMADO");
    switch (newState) {
    case GameState::START_MENU:
        CCLOG("Estado: Menú principal");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));
        break;

    case GameState::IN_GAME:
        CCLOG("Estado: En el juego");
        this->resumeGame();
        break;

    case GameState::PAUSED:
        CCLOG("Estado: Pausado");
        this->pauseGame();
        break;

    case GameState::GAME_OVER:
        CCLOG("Estado: Juego terminado");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        break;

    case GameState::VICTORY:
        CCLOG("Estado: Victoria");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, VictoryScene::createScene()));
        break;
    }
}

void BHFIGHT::pauseGame() {
    Director::getInstance()->pause();
    auto pauseLayer = LayerColor::create(Color4B(0, 0, 0, 128));
    auto label = Label::createWithTTF("Pausa", "fonts/Marker Felt.ttf", 32);
    label->setPosition(Director::getInstance()->getVisibleSize() / 2);
    pauseLayer->addChild(label);
    this->addChild(pauseLayer, 10, "PauseLayer");
}

void BHFIGHT::resumeGame() {
    Director::getInstance()->resume();
    this->removeChildByName("PauseLayer");
}

void BHFIGHT::updateHealthBars()
{
    if (playerHealth < 0) playerHealth = 1;
    if (playerHealth > 100) playerHealth = 100;
    if (BHHealth < 0) BHHealth = 1;
    if (BHHealth > 100) BHHealth = 100;

    float playerScaleX = static_cast<float>(playerHealth) * 10 / 100.0f;
    float rivalScaleX = static_cast<float>(BHHealth) * 10 / 100.0f;
    float playerFatigueScaleX = static_cast<float>(playerFatigue) * 2 / 100.0f;

    playerHealthBar->setScaleX(playerScaleX);
    rivalHealthBar->setScaleX(rivalScaleX);
    playerFatigueBar->setScaleX(playerFatigueScaleX);
}

void BHFIGHT::setupHealthBars()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Crear barra de salud del jugador
    playerHealthBar = Sprite::create("HealthBarLM.png");
    playerHealthBar->setAnchorPoint(Vec2(0, 0.5));
    playerHealthBar->setPosition(origin.x + 50, visibleSize.height - 50);
    playerHealthBar->setScaleX(10.0f);
    playerHealthBar->setScaleY(5.0f);
    this->addChild(playerHealthBar, 1);

    // Crear barra de salud del rival
    rivalHealthBar = Sprite::create("HealthBarBH.png");
    rivalHealthBar->setAnchorPoint(Vec2(0, 0.5));
    rivalHealthBar->setPosition(origin.x + visibleSize.width - 1000, visibleSize.height - 50);
    rivalHealthBar->setScaleX(10.0f);
    rivalHealthBar->setScaleY(5.0f);
    this->addChild(rivalHealthBar, 1);

    // Barra de fatiga del jugador
    playerFatigueBar = Sprite::create("HealthBarLM.png");
    playerFatigueBar->setAnchorPoint(Vec2(0, 0.5));
    playerFatigueBar->setPosition(origin.x + 50, visibleSize.height - 100);
    rivalHealthBar->setScaleX(2.0f);
    playerFatigueBar->setScaleY(1.0f);
    this->addChild(playerFatigueBar, 1);
}

void BHFIGHT::setupTimer() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto counterLabel = Label::createWithTTF("3' 00", "fonts/Marker Felt.ttf", 24);
    counterLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - counterLabel->getContentSize().height));
    counterLabel->setColor(Color3B::WHITE);
    this->addChild(counterLabel, 4);

    this->schedule([this, counterLabel](float delta) {
        if (totalTime > 0) {
            totalTime--;
            int minutes = totalTime / 60;
            int seconds = totalTime % 60;

            std::string timeString = std::to_string(minutes) + "' " +
                (seconds < 10 ? "0" : "") + std::to_string(seconds);
            counterLabel->setString(timeString);
        }
        else {
            contDerrotas++;
            this->unschedule("UpdateCounter");
            counterLabel->setString("¡Fin del tiempo!");
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        }, 1.0f, "UpdateCounter");
}

void BHFIGHT::reduceFatigue()
{
    if (playerFatigue >= fatigueThreshold)
    {
        playerFatigue -= 30; // Reducimos fatiga con cada ataque
        CCLOG("Fatiga del jugador: %d", playerFatigue);
        updateHealthBars();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

void BHFIGHT::recoverFatigue(float delta)
{
    if (playerFatigue < 100)
    {
        playerFatigue = std::min(100, playerFatigue + 1); // Recuperación gradual
        updateHealthBars();
    }
}

void BHFIGHT::applyDamage(int damageToPlayer, int damageToRival)
{
    // Reducir salud
    playerHealth = std::max(1, playerHealth - damageToPlayer);
    BHHealth = std::max(1, BHHealth - damageToRival);

    // Actualizar barras de salud
    updateHealthBars();

    if (playerHealth == 1) {
        playerHealth = 70;
        contcaidas++;
        if (contcaidas == 3) {
            contcaidas = 0;
            contDerrotas++;
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        if (contDerrotas == 3) {
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverTotal::createScene()));
        }
        updateHealthBars();
    }
}

void BHFIGHT::setupBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Crear la imagen de fondo
    auto background = Sprite::create("BoxingRing2.png");

    background->setLocalZOrder(0);


    // Ajustar la altura de la imagen al tamaño visible
    float scaleY = visibleSize.height / background->getContentSize().height;
    background->setScale(scaleY);

    // Posicionar la imagen para que esté centrada
    background->setPosition(background->getContentSize().width * scaleY / 2 - 600, visibleSize.height / 2 + origin.y);

    // Crear un ClippingNode para recortar la imagen
    auto clippingNode = ClippingNode::create();
    clippingNode->setContentSize(Size(background->getContentSize().width / 2 * scaleY, visibleSize.height)); // Mostrar solo la mitad del ancho
    clippingNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    clippingNode->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // Crear un stencil (forma de recorte) para el ClippingNode
    auto stencil = DrawNode::create();
    Vec2 rectangle[4] = {
        Vec2(0, 0),
        Vec2(clippingNode->getContentSize().width, 0),
        Vec2(clippingNode->getContentSize().width, clippingNode->getContentSize().height),
        Vec2(0, clippingNode->getContentSize().height)
    };
    stencil->drawPolygon(rectangle, 4, Color4F(1, 1, 1, 1), 0, Color4F(0, 0, 0, 0));
    clippingNode->setStencil(stencil);

    // Agregar la imagen al ClippingNode
    clippingNode->addChild(background);
    this->addChild(clippingNode, -1);

    // Evento para mover el fondo
    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [background, scaleY](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (keyCode == EventKeyboard::KeyCode::KEY_0) {
            background->setPositionX(background->getPositionX() + 50); // Mover a la izquierda
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_1) {
            background->setPositionX(background->getPositionX() - 50); // Mover a la derecha
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void BHFIGHT::checkLife() {

    if (BHHealth == 1){
        for (int i = 0; i < 7; i++) {
            BHHealth += 10;
            updateHealthBars();
        }
    }
}

void BHFIGHT::rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur)
{
    // Generar un número aleatorio entre 0 y 99
    int decision = random(0, 99);

    if (decision < 70) // 70% de probabilidad de atacar
    {
        if (rivalFatigue >= rivalFatigueThreshold)
        {
            isReacting = true;
            rivalAttack(rivalSprite, BH_Hook, BH_Double, BH_Bur);
            CCLOG("El rival ataca. Fatiga restante: %d", rivalFatigue);
        }
        else
        {
            CCLOG("El rival está demasiado cansado para atacar.");
        }
    }

    // Recuperar fatiga del rival gradualmente
    this->scheduleOnce([this](float delta) {
        if (rivalFatigue < 100)
        {
            rivalFatigue = std::min(100, rivalFatigue + 5); // Recuperar 5 puntos
            CCLOG("Fatiga del rival recuperada a: %d", rivalFatigue);
        }
        }, 1.0f, "RecoverRivalFatigue");
}

void BHFIGHT::rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Hook, cocos2d::Action* BH_Double, cocos2d::Action* BH_Bur)
{
    if (isAnimating) return;
    float rivalLifeRatio = (float)BHHealth / MAX_HEALTH;

    // Determinar tipo de ataque según la vida del rival
    if (rivalLifeRatio > 0.6)
    {
        attackType = (rand() % 10 < 5) ? ANIMATION_TAUNT : ATTACK_HOOK;
    }
    else if (rivalLifeRatio > 0.3)
    {
        attackType = (rand() % 10 < 5) ? ATTACK_HOOK : ATTACK_DOUBLE;
    }
    else
    {
        attackType = (rand() % 10 < 8) ? ATTACK_DOUBLE : ATTACK_HOOK;
    }

    // Ejecutar acción según el tipo de ataque
    switch (attackType)
    {
    case ATTACK_HOOK:
        rivalSprite->stopAllActions();
        rivalSprite->runAction(BH_Hook);
        break;
    case ATTACK_DOUBLE:
        rivalSprite->stopAllActions();
        rivalSprite->runAction(BH_Double);
        break;
    case ANIMATION_TAUNT:
        playRivalIdleAnimation(rivalSprite, BH_Bur);
        break;
    }

    this->scheduleOnce([this](float delta) {
        resolveAttack();
        }, 1.0f, "ReactionTimer");
}

void BHFIGHT::resolveAttack()
{
    if (!isReacting) // Si el jugador no reaccionó
    {
        if (attackType == ATTACK_HOOK) applyDamage(15, 0);
        else if (attackType == ATTACK_DOUBLE) applyDamage(25, 0);
        // Reiniciar estado
        isReacting = false;
    }
    else
    {
        playerReact();
    }
}

void BHFIGHT::playerReact()
{
    if (!isReacting) return; // Ignorar si no está en ventana de reacción

    if ((attackType == ATTACK_HOOK && reactionAct == 1) ||
        (attackType == ATTACK_DOUBLE && reactionAct == 2))
    {
        CCLOG("¡Reaccionaste correctamente!");
        isReacting = false; // Detener el ataque
    }
    else
    {
        if (attackType == ATTACK_HOOK) applyDamage(15, 0);
        else if (attackType == ATTACK_DOUBLE) applyDamage(25, 0);
        // Reiniciar estado
        isReacting = false;
        CCLOG("Reacción incorrecta, recibirás daño.");
    }
}

void BHFIGHT::playRivalIdleAnimation(cocos2d::Sprite* rivalSprite, cocos2d::Action* BH_Moving)
{
    // Define la animación de movimiento del rival
    if (!isAnimating) // Solo si no está en otra acción

        isAnimating = true;

    auto callback = CallFunc::create([=]() {
        isAnimating = false;
        });
    {
        rivalSprite->stopAllActions();
        auto moveBy = MoveBy::create(1.0f, Vec2(10, 0)); // Moverse ligeramente hacia la derecha
        auto moveBack = moveBy->reverse();               // Regresar a la posición original

        // Combinar movimiento y animación
        auto moveAndAnimate = Spawn::create(moveBy, BH_Moving, nullptr);

        // Secuencia completa
        auto sequence = Sequence::create(moveAndAnimate, moveBack, callback, nullptr);

        rivalSprite->runAction(sequence);

        CCLOG("El rival realiza una animación de movimiento.");
    }
}

void BHFIGHT::processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* protectAction, cocos2d::Action* BH_GPIAnimate, cocos2d::Action* BH_GPDAnimate, cocos2d::Action* BH_GPIBAnimate, cocos2d::Action* BH_GPDBAnimate, cocos2d::Action* BH_EsquivaIzq, cocos2d::Action* BH_EsquivaDer, cocos2d::Action* BH_GUAnimate, cocos2d::Action* BH_Burla, bool& isDefending) {

    int randomValue = random(0, 100);

    if (playerAction == 1) { // Supongamos que 1 es un ataque del jugador
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            rivalSprite->stopAllActions();
            isDefending = true;
            rivalSprite->runAction(protectAction);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GPIAnimate);
            applyDamage(0, 10);
        }
    }
    else if (playerAction == 2) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_EsquivaDer);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GPDAnimate);
        }
    }
    else if (playerAction == 3) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(protectAction);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GPIAnimate);
        }
    }
    else if (playerAction == 4) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(protectAction);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GPDAnimate);
        }
    }
    else if (playerAction == 5) {
        if (randomValue < 80) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(protectAction);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GUAnimate);
        }
    }
    else if (playerAction == 6 || playerAction == 7) {
        if (randomValue < 100) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_Burla);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(BH_GPIBAnimate);
        }
    }
}

void BHFIGHT::processAttack()
{
    if (playerFatigue >= fatigueThreshold)
    {
        reduceFatigue();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

Scene* BHFIGHT::createScene()
{
    return BHFIGHT::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool BHFIGHT::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    setupBackground();
    setupTimer();
    setupHealthBars();

    // Recuperar fatiga con el tiempo
    this->schedule([this](float delta) {
        recoverFatigue(delta);
        }, 0.1f, "RecoverFatigue");


    // Obtener los SpriteFrame desde el ResourceManager
    auto LM_PI = ResourceManager::getInstance()->getSpriteFrame("LM_PI");
    auto LM_JabIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq1");
    auto LM_JabIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq2");
    auto LM_JabDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer1");
    auto LM_JabDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer2");

    Vector<SpriteFrame*> jabIzqFrames = { LM_JabIzq1, LM_JabIzq2, LM_PI };
    auto jabIzqAnimacion = Animation::createWithSpriteFrames(jabIzqFrames, 0.3f);
    auto jabIzqAnimate = Animate::create(jabIzqAnimacion);
    jabIzqAnimate->retain();

    Vector<SpriteFrame*> jabDerFrames = { LM_JabDer1, LM_JabDer2, LM_PI };
    auto jabDerAnimacion = Animation::createWithSpriteFrames(jabDerFrames, 0.3f);
    auto jabDerAnimate = Animate::create(jabDerAnimacion);
    jabDerAnimate->retain();

    auto LM_GanIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq1");
    auto LM_GanIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq2");
    auto LM_GanIzq3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq3");
    auto LM_GanIzq4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq4");
    auto LM_GanDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer1");
    auto LM_GanDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer2");
    auto LM_GanDer3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer3");
    auto LM_GanDer4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer4");

    Vector<SpriteFrame*> GanDerFrames = { LM_GanDer1, LM_GanDer2, LM_GanDer3, LM_GanDer4, LM_PI };
    auto GanDerAnimacion = Animation::createWithSpriteFrames(GanDerFrames, 0.3f);
    auto GanDerAnimate = Animate::create(GanDerAnimacion);
    GanDerAnimate->retain();

    Vector<SpriteFrame*> GanIzqFrames = { LM_GanIzq1, LM_GanIzq2, LM_GanIzq3, LM_GanIzq4, LM_PI };
    auto GanIzqAnimacion = Animation::createWithSpriteFrames(GanIzqFrames, 0.3f);
    auto GanIzqAnimate = Animate::create(GanIzqAnimacion);
    GanIzqAnimate->retain();

    auto LM_GAMD1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_GAMD2 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");

    Vector<SpriteFrame*> UpperCutFrames = { LM_GAMD1, LM_GAMD2, LM_PI };
    auto UpperCutAnimacion = Animation::createWithSpriteFrames(UpperCutFrames, 0.6f);
    auto UpperCutAnimate = Animate::create(UpperCutAnimacion);
    UpperCutAnimate->retain();

    auto LM_GBI2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI2");
    auto LM_GBI1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI1");
    auto LM_GBD2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD2");
    auto LM_GBD1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD1");

    Vector<SpriteFrame*> GBIFrames = { LM_GBI1, LM_GBI2, LM_PI };
    auto GBIAnimacion = Animation::createWithSpriteFrames(GBIFrames, 0.4f);
    auto GBIAnimate = Animate::create(GBIAnimacion);
    GBIAnimate->retain();

    Vector<SpriteFrame*> GBDFrames = { LM_GBD1, LM_GBD2, LM_PI };
    auto GBDAnimacion = Animation::createWithSpriteFrames(GBDFrames, 0.4f);
    auto GBDAnimate = Animate::create(GBDAnimacion);
    GBDAnimate->retain();

    auto LM_GR = ResourceManager::getInstance()->getSpriteFrame("LM_GR");
    auto LM_ES = ResourceManager::getInstance()->getSpriteFrame("LM_ES");
    auto LM_KOI2 = ResourceManager::getInstance()->getSpriteFrame("LM_KOI2");

    auto LM_EI = ResourceManager::getInstance()->getSpriteFrame("LM_EI");
    auto LM_ED = ResourceManager::getInstance()->getSpriteFrame("LM_ED");

    Vector<SpriteFrame*> EsquivaIFrames = { LM_EI, LM_PI };
    auto EsquivaIAnimacion = Animation::createWithSpriteFrames(EsquivaIFrames, 1.0f);
    auto EsquivaIAnimate = Animate::create(EsquivaIAnimacion);
    EsquivaIAnimate->retain();

    Vector<SpriteFrame*> EsquivaDFrames = { LM_ED, LM_PI };
    auto EsquivaDAnimacion = Animation::createWithSpriteFrames(EsquivaDFrames, 1.0f);
    auto EsquivaDAnimate = Animate::create(EsquivaDAnimacion);
    EsquivaDAnimate->retain();

    auto LM_DD = ResourceManager::getInstance()->getSpriteFrame("LM_DD");
    auto LM_KOD1 = ResourceManager::getInstance()->getSpriteFrame("LM_KOD1");

    auto LM_S1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_S2 = ResourceManager::getInstance()->getSpriteFrame("LM_S2");
    auto LM_S3 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");
    auto LM_S4 = ResourceManager::getInstance()->getSpriteFrame("LM_S4");
    auto LM_S5 = ResourceManager::getInstance()->getSpriteFrame("LM_S5");

    Vector<SpriteFrame*> SuperFrames = { LM_S1, LM_S2, LM_S3, LM_S4, LM_S5, LM_PI };
    auto SuperAnimacion = Animation::createWithSpriteFrames(SuperFrames, 0.2f);
    auto SuperAnimate = Animate::create(SuperAnimacion);
    SuperAnimate->retain();

    Vector<SpriteFrame*> GoDownFrames = { LM_GR, LM_PI };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR, LM_PI };
    auto BloqAnimacion = Animation::createWithSpriteFrames(BloqFrames, 1.0f);
    auto BloqAnimate = Animate::create(BloqAnimacion);
    BloqAnimate->retain();

    auto BH_PI = ResourceManager::getInstance()->getSpriteFrame("BH_PI");
    auto BH_Guardia = ResourceManager::getInstance()->getSpriteFrame("BH_Guardia");
    auto BH_Risa = ResourceManager::getInstance()->getSpriteFrame("BH_Risa");
    auto BH_Lengua = ResourceManager::getInstance()->getSpriteFrame("BH_Lengua");

    auto BH_juego = ResourceManager::getInstance()->getSpriteFrame("BH_juego");
    auto BH_Prep1 = ResourceManager::getInstance()->getSpriteFrame("BH_Prep1");
    auto BH_Prep2 = ResourceManager::getInstance()->getSpriteFrame("BH_Prep2");
    auto BH_Prov = ResourceManager::getInstance()->getSpriteFrame("BH_Prov");

    auto BH_JabIzq1 = ResourceManager::getInstance()->getSpriteFrame("BH_JabIzq1");
    auto BH_JabIzq2 = ResourceManager::getInstance()->getSpriteFrame("BH_JabIzq2");
    auto BH_GAI1 = ResourceManager::getInstance()->getSpriteFrame("BH_GAI1");
    auto BH_GAI2 = ResourceManager::getInstance()->getSpriteFrame("BH_GAI2");

    auto BH_GAI3 = ResourceManager::getInstance()->getSpriteFrame("BH_GAI3");
    auto BH_GD1 = ResourceManager::getInstance()->getSpriteFrame("BH_GD1");
    auto BH_GD2 = ResourceManager::getInstance()->getSpriteFrame("BH_GD2");
    auto BH_GDG = ResourceManager::getInstance()->getSpriteFrame("BH_GDG");

    auto BH_EI1 = ResourceManager::getInstance()->getSpriteFrame("BH_EI1");
    auto BH_EI2 = ResourceManager::getInstance()->getSpriteFrame("BH_EI2");
    auto BH_ED1 = ResourceManager::getInstance()->getSpriteFrame("BH_ED1");
    auto BH_ED2 = ResourceManager::getInstance()->getSpriteFrame("BH_ED2");
    auto BH_GPI1 = ResourceManager::getInstance()->getSpriteFrame("BH_GPI1");
    auto BH_GPD1 = ResourceManager::getInstance()->getSpriteFrame("BH_GPD1");
    auto BH_GPU = ResourceManager::getInstance()->getSpriteFrame("BH_GPU");

    auto BH_GPAI = ResourceManager::getInstance()->getSpriteFrame("BH_GPAI");
    auto BH_GPAD = ResourceManager::getInstance()->getSpriteFrame("BH_GPAD");
    auto BH_Disos = ResourceManager::getInstance()->getSpriteFrame("BH_Disos");
    auto BH_KOI1 = ResourceManager::getInstance()->getSpriteFrame("BH_KOI1");
    auto BH_KOI2 = ResourceManager::getInstance()->getSpriteFrame("BH_KOI2");

    auto BH_KOTI = ResourceManager::getInstance()->getSpriteFrame("BH_KOIT");
    auto BH_KOTD = ResourceManager::getInstance()->getSpriteFrame("BH_KOTD");
    auto BH_levantado = ResourceManager::getInstance()->getSpriteFrame("BH_levantado");
    auto BH_levantado2 = ResourceManager::getInstance()->getSpriteFrame("BH_levantado2");


    Vector<SpriteFrame*> protectFrames = { BH_Guardia, BH_Risa, BH_Lengua, BH_PI };
    auto protectAnimacion = Animation::createWithSpriteFrames(protectFrames, 0.5f);
    auto protectAction = Animate::create(protectAnimacion);
    protectAction->retain();

    Vector<SpriteFrame*> BH_EsquivaIzq = { BH_EI2, BH_PI };
    auto BH_EsquivaIzqAnimacion = Animation::createWithSpriteFrames(BH_EsquivaIzq, 0.8f);
    auto BH_EsquivaIzqAnimate = Animate::create(BH_EsquivaIzqAnimacion);
    BH_EsquivaIzqAnimate->retain();

    Vector<SpriteFrame*> BH_EsquivaDer = { BH_ED2, BH_PI };
    auto BH_EsquivaDerAnimacion = Animation::createWithSpriteFrames(BH_EsquivaDer, 0.8f);
    auto BH_EsquivaDerAnimate = Animate::create(BH_EsquivaDerAnimacion);
    BH_EsquivaDerAnimate->retain();

    Vector<SpriteFrame*> BH_GolpeadoIzq = { BH_GPAI, BH_PI };
    auto BH_GPIAnimation = Animation::createWithSpriteFrames(BH_GolpeadoIzq, 1.0f);
    auto BH_GPIAnimate = Animate::create(BH_GPIAnimation);
    BH_GPIAnimate->retain();

    Vector<SpriteFrame*> BH_GolpeadoDer = { BH_GPAD, BH_PI };
    auto BH_GPDAnimation = Animation::createWithSpriteFrames(BH_GolpeadoDer, 1.0f);
    auto BH_GPDAnimate = Animate::create(BH_GPDAnimation);
    BH_GPDAnimate->retain();

    Vector<SpriteFrame*> BH_GolpeadoBDer = { BH_GPD1, BH_PI };
    auto BH_GPDBAnimation = Animation::createWithSpriteFrames(BH_GolpeadoBDer, 1.0f);
    auto BH_GPDBAnimate = Animate::create(BH_GPDBAnimation);
    BH_GPDBAnimate->retain();

    Vector<SpriteFrame*> BH_GolpeadoBIzq = { BH_GPI1, BH_PI };
    auto BH_GPIBAnimation = Animation::createWithSpriteFrames(BH_GolpeadoBIzq, 1.0f);
    auto BH_GPIBAnimate = Animate::create(BH_GPIBAnimation);
    BH_GPIBAnimate->retain();

    Vector<SpriteFrame*> BH_GUFrames = { BH_GPU, BH_PI };
    auto BH_GUAnimacion = Animation::createWithSpriteFrames(BH_GUFrames, 1.0f);
    auto BH_GUAnimate = Animate::create(BH_GUAnimacion);
    BH_GUAnimate->retain();

    Vector<SpriteFrame*> BH_BurFrames = { BH_juego, BH_Prov, BH_PI };
    auto BH_BurAnimacion = Animation::createWithSpriteFrames(BH_BurFrames, 0.4f);
    auto BH_Bur = Animate::create(BH_BurAnimacion);
    BH_Bur->retain();

    Vector<SpriteFrame*> BH_BurlaFrames = { BH_Risa, BH_Lengua, BH_PI };
    auto BH_BurlaAnimacion = Animation::createWithSpriteFrames(BH_BurlaFrames, 0.4f);
    auto BH_Burla = Animate::create(BH_BurlaAnimacion);
    BH_Burla->retain();

    Vector<SpriteFrame*> BH_HookFrames = { BH_JabIzq1, BH_JabIzq2, BH_PI };
    auto BH_HookAnimacion = Animation::createWithSpriteFrames(BH_HookFrames, 0.7f);
    auto BH_Hook = Animate::create(BH_HookAnimacion);
    BH_Hook->retain();

    Vector<SpriteFrame*> BH_DobFrames = { BH_GD1, BH_GD2, BH_PI };
    auto BH_DobAnimacion = Animation::createWithSpriteFrames(BH_DobFrames, 0.7f);
    auto BH_Double = Animate::create(BH_DobAnimacion);
    BH_Double->retain();

    auto rivalSprite = Sprite::createWithSpriteFrame(BH_PI);
    rivalSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 150));
    this->addChild(rivalSprite);

    auto playerSprite = Sprite::createWithSpriteFrame(LM_PI);
    playerSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 250));
    this->addChild(playerSprite);

    // Configurar jugador en la capa superior
    playerSprite->setLocalZOrder(3);

    // Configurar rival en la capa intermedia
    rivalSprite->setLocalZOrder(2);

    // Configurar barras de vida en la capa inferior
    playerHealthBar->setLocalZOrder(1);
    rivalHealthBar->setLocalZOrder(1);


    this->schedule([this, rivalSprite, BH_Hook, BH_Double, BH_Bur](float delta) {
        if (!isAnimating && !isReacting) {
            rivalAction(rivalSprite, BH_Hook, BH_Double, BH_Bur); // Acción del rival
        }
        }, 2.0f, "RivalActionScheduler");

    bool isDefending = false;

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (isAnimating) return;
        if (playerFatigue < fatigueThreshold) {
            CCLOG("¡Demasiado cansado para atacar!");
            return;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_Q) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Izquierdo)");
            isAnimating = true;

            processAI(1, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_E) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Derecho)");
            isAnimating = true;

            processAI(2, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Derecho)");
            isAnimating = true;

            processAI(3, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(4, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) {
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (UpperCut)");
            isAnimating = true;

            processAI(5, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(UpperCutAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 30);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_Z) 
        {
            CCLOG("El jugador lanza un ataque (Golpe Bajo Izquierdo)");
            isAnimating = true;

            processAI(6, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBIAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_C)
        {
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(7, rivalSprite, protectAction, BH_GPIAnimate, BH_GPDAnimate, BH_GPIBAnimate, BH_GPDBAnimate, BH_EsquivaIzqAnimate, BH_EsquivaDerAnimate, BH_GUAnimate, BH_Burla, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBDAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.1f, Vec2(-10, 0)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, EsquivaIAnimate, moveBack, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.1f, Vec2(10, 0)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(EsquivaDAnimate, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_R) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(BloqAnimate, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) // Esquivar
        {
            isReacting = true;
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(0, -200)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, GoDownAnimate, moveBack, callback, nullptr));
            reactionAct = 2;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_P) {
            if (currentState == 2) {
                currentState = 3;
                changeGameState(GameState::PAUSED);
                CCLOG("Pausa");
            }
            else if (currentState == 3) {
                changeGameState(GameState::IN_GAME);
                currentState = 2;
            }
        }

        if (BHHealth <= 1) {
            playerSprite->stopAllActions();
            Director::getInstance()->replaceScene(TransitionFade::create(2.0f, NK_Face::createScene()));
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

    return true;
};

void BHFIGHT::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}



void GJFIGHT::changeGameState(GameState newState) {
    CCLOG("LLAMADO");
    switch (newState) {
    case GameState::START_MENU:
        CCLOG("Estado: Menú principal");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));
        break;

    case GameState::IN_GAME:
        CCLOG("Estado: En el juego");
        this->resumeGame();
        break;

    case GameState::PAUSED:
        CCLOG("Estado: Pausado");
        this->pauseGame();
        break;

    case GameState::GAME_OVER:
        CCLOG("Estado: Juego terminado");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        break;

    case GameState::VICTORY:
        CCLOG("Estado: Victoria");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, BH_Face::createScene()));
        break;
    }
}

void GJFIGHT::pauseGame() {
    Director::getInstance()->pause(); // Pausa el motor del juego
    auto pauseLayer = LayerColor::create(Color4B(0, 0, 0, 128)); // Agregar una capa semitransparente
    auto label = Label::createWithTTF("Pausa", "fonts/Marker Felt.ttf", 32);
    label->setPosition(Director::getInstance()->getVisibleSize() / 2);
    pauseLayer->addChild(label);
    this->addChild(pauseLayer, 10, "PauseLayer");
}

void GJFIGHT::resumeGame() {
    Director::getInstance()->resume(); // Reanuda el motor del juego
    this->removeChildByName("PauseLayer"); // Elimina la capa de pausa
}

void GJFIGHT::updateHealthBars()
{
    if (playerHealth < 0) playerHealth = 1;
    if (playerHealth > 100) playerHealth = 100;
    if (rivalHealth < 0) rivalHealth = 1;
    if (rivalHealth > 100) rivalHealth = 100;

    float playerScaleX = static_cast<float>(playerHealth) * 10 / 100.0f;
    float rivalScaleX = static_cast<float>(rivalHealth) * 10 / 100.0f;
    float playerFatigueScaleX = static_cast<float>(playerFatigue) * 2 / 100.0f;

    playerHealthBar->setScaleX(playerScaleX);
    rivalHealthBar->setScaleX(rivalScaleX);
    playerFatigueBar->setScaleX(playerFatigueScaleX);
}

void GJFIGHT::setupHealthBars()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Crear barra de salud del jugador
    playerHealthBar = Sprite::create("HealthBarLM.png");
    playerHealthBar->setAnchorPoint(Vec2(0, 0.5));
    playerHealthBar->setPosition(origin.x + 50, visibleSize.height - 50);
    playerHealthBar->setScaleX(10.0f);
    playerHealthBar->setScaleY(5.0f);
    this->addChild(playerHealthBar, 1);

    // Crear barra de salud del rival
    rivalHealthBar = Sprite::create("HealthBarBH.png");
    rivalHealthBar->setAnchorPoint(Vec2(0, 0.5));
    rivalHealthBar->setPosition(origin.x + visibleSize.width - 1000, visibleSize.height - 50);
    rivalHealthBar->setScaleX(10.0f);
    rivalHealthBar->setScaleY(5.0f);
    this->addChild(rivalHealthBar, 1);

    // Barra de fatiga del jugador
    playerFatigueBar = Sprite::create("HealthBarLM.png");
    playerFatigueBar->setAnchorPoint(Vec2(0, 0.5));
    playerFatigueBar->setPosition(origin.x + 50, visibleSize.height - 100);
    rivalHealthBar->setScaleX(2.0f);
    playerFatigueBar->setScaleY(1.0f);
    this->addChild(playerFatigueBar, 1);
}

void GJFIGHT::setupTimer() {
    // Crear el Label para el contador
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    auto counterLabel = Label::createWithTTF("3' 00", "fonts/Marker Felt.ttf", 24);
    counterLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - counterLabel->getContentSize().height));
    counterLabel->setColor(Color3B::WHITE); // Color del texto
    this->addChild(counterLabel, 4); // Añadir a la escena con z-order alto

    // Programador para el contador regresivo
    this->schedule([this, counterLabel](float delta) { // Captura 'this' y 'counterLabel'
        if (totalTime > 0) {
            totalTime--; // Reducir el tiempo
            int minutes = totalTime / 60;
            int seconds = totalTime % 60;

            // Formatear el tiempo como "min' seg"
            std::string timeString = std::to_string(minutes) + "' " +
                (seconds < 10 ? "0" : "") + std::to_string(seconds);
            counterLabel->setString(timeString); // Actualizar el texto del Label
        }
        else {
            contDerrotas++;
            this->unschedule("UpdateCounter");
            counterLabel->setString("¡Fin del tiempo!");
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        }, 1.0f, "UpdateCounter");
}

void GJFIGHT::reduceFatigue()
{
    if (playerFatigue >= fatigueThreshold)
    {
        playerFatigue -= 30; // Reducimos fatiga con cada ataque
        CCLOG("Fatiga del jugador: %d", playerFatigue);
        updateHealthBars();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

void GJFIGHT::recoverFatigue(float delta)
{
    if (playerFatigue < 100)
    {
        playerFatigue = std::min(100, playerFatigue + 1); // Recuperación gradual
        updateHealthBars();
    }
}

void GJFIGHT::applyDamage(int damageToPlayer, int damageToRival)
{
    // Reducir salud
    playerHealth = std::max(1, playerHealth - damageToPlayer);
    rivalHealth = std::max(1, rivalHealth - damageToRival);

    // Actualizar barras de salud
    updateHealthBars();

    if (playerHealth == 1) {
        playerHealth = 70;
        contcaidas++;
        if (contcaidas == 3) {
            contcaidas = 0;
            contDerrotas++;
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        }
        if (contDerrotas == 3) {
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverTotal::createScene()));
        }
        updateHealthBars();
    }
}

void GJFIGHT::setupBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Crear la imagen de fondo
    auto background = Sprite::create("BoxingRing1.png");

    background->setLocalZOrder(0);


    // Ajustar la altura de la imagen al tamaño visible
    float scaleY = visibleSize.height / background->getContentSize().height;
    background->setScale(scaleY);

    // Posicionar la imagen para que esté centrada
    background->setPosition(background->getContentSize().width * scaleY / 2 - 600, visibleSize.height / 2 + origin.y);

    // Crear un ClippingNode para recortar la imagen
    auto clippingNode = ClippingNode::create();
    clippingNode->setContentSize(Size(background->getContentSize().width / 2 * scaleY, visibleSize.height)); // Mostrar solo la mitad del ancho
    clippingNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    clippingNode->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // Crear un stencil (forma de recorte) para el ClippingNode
    auto stencil = DrawNode::create();
    Vec2 rectangle[4] = {
        Vec2(0, 0),
        Vec2(clippingNode->getContentSize().width, 0),
        Vec2(clippingNode->getContentSize().width, clippingNode->getContentSize().height),
        Vec2(0, clippingNode->getContentSize().height)
    };
    stencil->drawPolygon(rectangle, 4, Color4F(1, 1, 1, 1), 0, Color4F(0, 0, 0, 0));
    clippingNode->setStencil(stencil);

    // Agregar la imagen al ClippingNode
    clippingNode->addChild(background);
    this->addChild(clippingNode, -1);

    // Evento para mover el fondo
    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [background, scaleY](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (keyCode == EventKeyboard::KeyCode::KEY_0) {
            background->setPositionX(background->getPositionX() + 50); // Mover a la izquierda
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_1) {
            background->setPositionX(background->getPositionX() - 50); // Mover a la derecha
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);
}

void GJFIGHT::checkLife() {

    if (rivalHealth == 1) {
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, BH_Face::createScene()));
    }
}

void GJFIGHT::rivalAction(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_JabIzqAnimate, cocos2d::Action* GJ_JabDerAnimate, cocos2d::Action* GJ_GBIAnimate, cocos2d::Action* GJ_GBDAnimate, cocos2d::Action* GJ_GRDAnimate, cocos2d::Action* GJ_GRIAnimate, cocos2d::Action* GJ_DISAnimate, cocos2d::Action* GJ_ProvAnimate, cocos2d::Action* GJ_UPAnimate)
{
    // Generar un número aleatorio entre 0 y 99
    int decision = random(0, 99);

    if (decision < 70) // 70% de probabilidad de atacar
    {
        if (rivalFatigue >= rivalFatigueThreshold)
        {
            isReacting = true;
            rivalAttack(rivalSprite, GJ_JabIzqAnimate, GJ_JabDerAnimate, GJ_GBIAnimate, GJ_GBDAnimate, GJ_GRDAnimate, GJ_GRIAnimate, GJ_DISAnimate, GJ_ProvAnimate, GJ_UPAnimate);
            CCLOG("El rival ataca. Fatiga restante: %d", rivalFatigue);
        }
        else
        {
            CCLOG("El rival está demasiado cansado para atacar.");
        }
    }
}

void GJFIGHT::rivalAttack(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_JabIzqAnimate, cocos2d::Action* GJ_JabDerAnimate, cocos2d::Action* GJ_GBIAnimate, cocos2d::Action* GJ_GBDAnimate, cocos2d::Action* GJ_GRDAnimate, cocos2d::Action* GJ_GRIAnimate, cocos2d::Action* GJ_DISAnimate, cocos2d::Action* GJ_ProvAnimate, cocos2d::Action* GJ_UPAnimate)
{
    if (isAnimating) return;
    float rivalLifeRatio = (float)rivalHealth / MAX_HEALTH;

    int numran = random(0, 11);

    // Determinar tipo de ataque según la vida del rival
    if (rivalLifeRatio > 0.6)
    {
        if (numran >= 8) attackType = DOWN_ATTACK;
        else if (numran >= 4) attackType = JAB_ATTACK;
        else attackType = RECT_ATTACK;
    }
    else if (rivalLifeRatio > 0.3)
    {
        if (numran >= 6) attackType = JAB_ATTACK;
        else attackType = RECT_ATTACK;
    }
    else
    {
        attackType = RECT_ATTACK;
    }

    // Ejecutar acción según el tipo de ataque
    int dos = random(0, 2);
    switch (attackType)
    {
    case RECT_ATTACK:
        rivalSprite->stopAllActions();
        if (dos) {
            rivalSprite->runAction(GJ_GRDAnimate);
        }
        else {
            rivalSprite->runAction(GJ_GRIAnimate);
        }
        break;
    case JAB_ATTACK:
        rivalSprite->stopAllActions();
        if (dos) {
            rivalSprite->runAction(GJ_JabDerAnimate);
        }
        else {
            rivalSprite->runAction(GJ_JabIzqAnimate);
        }
        break;
    case DOWN_ATTACK:
        rivalSprite->stopAllActions();
        if (dos) {
            rivalSprite->runAction(GJ_GBDAnimate);
        }
        else {
            rivalSprite->runAction(GJ_GBIAnimate);
        }
        break;
    case ANIMATION_TAUNT:
        playRivalIdleAnimation(rivalSprite, GJ_ProvAnimate);
        break;
    }

    this->scheduleOnce([this](float delta) {
        resolveAttack();
        }, reactionTime, "ReactionTimer");

}

void GJFIGHT::resolveAttack()
{
    if (!isReacting) // Si el jugador no reaccionó
    {
        if (attackType == JAB_ATTACK || attackType == RECT_ATTACK) applyDamage(10, 0);
        else if (attackType == DOWN_ATTACK) applyDamage(15, 0);
        // Reiniciar estado
        isReacting = false;
    }
    else
    {
        playerReact();
    }
}

void GJFIGHT::playerReact()
{
    if (!isReacting) return; // Ignorar si no está en ventana de reacción

    if (((attackType == RECT_ATTACK || attackType == JAB_ATTACK) && reactionAct == 1) ||
        (attackType == DOWN_ATTACK && reactionAct == 2))
    {
        CCLOG("¡Reaccionaste correctamente!");
        isReacting = false; // Detener el ataque
    }
    else
    {
        if (attackType == RECT_ATTACK || attackType == JAB_ATTACK) applyDamage(10, 0);
        else if (attackType == DOWN_ATTACK) applyDamage(15, 0);
        // Reiniciar estado
        isReacting = false;
        CCLOG("Reacción incorrecta, recibirás daño.");
    }
}

void GJFIGHT::playRivalIdleAnimation(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_ProvAnimate)
{
    // Define la animación de movimiento del rival
    if (!isAnimating) // Solo si no está en otra acción

        isAnimating = true;
    isReacting = true;

    auto callback = CallFunc::create([=]() {
        isAnimating = false;
        isReacting = false;
        });
    {
        rivalSprite->stopAllActions();
        auto moveBy = MoveBy::create(1.0f, Vec2(0, +150)); // Moverse ligeramente hacia la derecha
        auto moveBack = MoveBy::create(1.0f, Vec2(0, -150));              // Regresar a la posición original

        // Secuencia completa
        auto sequence = Sequence::create(moveBy, GJ_ProvAnimate, moveBack, callback, nullptr);

        rivalSprite->runAction(sequence);

        CCLOG("El rival realiza una animación de movimiento.");
    }
}

void GJFIGHT::processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_GLPDAnimate, cocos2d::Action* GJ_GLPIAnimate, cocos2d::Action* GJ_GLPBDAnimate, cocos2d::Action* GJ_GLPBIAnimate, cocos2d::Action* GJ_GLPJDAnimate, cocos2d::Action* GJ_GLPJIAnimate, cocos2d::Action* GJ_GLPGDAnimate, cocos2d::Action* GJ_GLPGIAnimate, cocos2d::Action* GJ_ESQD_Animate, cocos2d::Action* GJ_ESQI_Animate, cocos2d::Action* GJ_GuardiaAAnimate, cocos2d::Action* GJ_GuardiaBAnimate, bool& isDefending) {

    int randomValue = random(0, 100);

    if (playerAction == 1) { // Supongamos que 1 es un ataque del jugador
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            rivalSprite->stopAllActions();
            isDefending = true;
            rivalSprite->runAction(GJ_ESQI_Animate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPJIAnimate);
            applyDamage(0, 10);
        }
    }
    else if (playerAction == 2) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_ESQD_Animate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPJDAnimate);
        }
    }
    else if (playerAction == 3) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GuardiaAAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPGDAnimate);
        }
    }
    else if (playerAction == 4) {
        if (randomValue < 60) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GuardiaAAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPGIAnimate);
        }
    }
    else if (playerAction == 5) {
        if (randomValue < 100) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GuardiaAAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPJDAnimate);
        }
    }
    else if (playerAction == 6) {
        if (randomValue < 40) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GuardiaBAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPBDAnimate);
        }
    }
    else if (playerAction == 7) {
        if (randomValue < 40) { // 60% de probabilidad de protegerse
            CCLOG("El rival se protege");
            isDefending = true;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GuardiaBAnimate);
        }
        else {
            CCLOG("El rival no se protegió");
            isDefending = false;
            rivalSprite->stopAllActions();
            rivalSprite->runAction(GJ_GLPBIAnimate);
        }
    }
}

void GJFIGHT::processAttack()
{
    if (playerFatigue >= fatigueThreshold)
    {
        reduceFatigue();
    }
    else
    {
        CCLOG("¡Demasiado cansado para atacar!");
    }
}

Scene* GJFIGHT::createScene()
{
    return GJFIGHT::create();
}

bool GJFIGHT::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    setupBackground();
    //setupTimer();
    setupHealthBars();

    // Recuperar fatiga con el tiempo
    this->schedule([this](float delta) {
        recoverFatigue(delta);
        }, 0.1f, "RecoverFatigue");

    // Obtener los SpriteFrame desde el ResourceManager
    auto LM_PI = ResourceManager::getInstance()->getSpriteFrame("LM_PI");
    auto LM_JabIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq1");
    auto LM_JabIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabIzq2");
    auto LM_JabDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer1");
    auto LM_JabDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_JabDer2");

    Vector<SpriteFrame*> jabIzqFrames = { LM_JabIzq1, LM_JabIzq2, LM_PI };
    auto jabIzqAnimacion = Animation::createWithSpriteFrames(jabIzqFrames, 0.3f);
    auto jabIzqAnimate = Animate::create(jabIzqAnimacion);
    jabIzqAnimate->retain();

    Vector<SpriteFrame*> jabDerFrames = { LM_JabDer1, LM_JabDer2, LM_PI };
    auto jabDerAnimacion = Animation::createWithSpriteFrames(jabDerFrames, 0.3f);
    auto jabDerAnimate = Animate::create(jabDerAnimacion);
    jabDerAnimate->retain();

    auto LM_GanIzq1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq1");
    auto LM_GanIzq2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq2");
    auto LM_GanIzq3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq3");
    auto LM_GanIzq4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanIzq4");
    auto LM_GanDer1 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer1");
    auto LM_GanDer2 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer2");
    auto LM_GanDer3 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer3");
    auto LM_GanDer4 = ResourceManager::getInstance()->getSpriteFrame("LM_GanDer4");

    Vector<SpriteFrame*> GanDerFrames = { LM_GanDer1, LM_GanDer2, LM_GanDer3, LM_GanDer4, LM_PI };
    auto GanDerAnimacion = Animation::createWithSpriteFrames(GanDerFrames, 0.3f);
    auto GanDerAnimate = Animate::create(GanDerAnimacion);
    GanDerAnimate->retain();

    Vector<SpriteFrame*> GanIzqFrames = { LM_GanIzq1, LM_GanIzq2, LM_GanIzq3, LM_GanIzq4, LM_PI };
    auto GanIzqAnimacion = Animation::createWithSpriteFrames(GanIzqFrames, 0.3f);
    auto GanIzqAnimate = Animate::create(GanIzqAnimacion);
    GanIzqAnimate->retain();

    auto LM_GAMD1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_GAMD2 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");

    Vector<SpriteFrame*> UpperCutFrames = { LM_GAMD1, LM_GAMD2, LM_PI };
    auto UpperCutAnimacion = Animation::createWithSpriteFrames(UpperCutFrames, 0.6f);
    auto UpperCutAnimate = Animate::create(UpperCutAnimacion);
    UpperCutAnimate->retain();

    auto LM_GBI2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI2");
    auto LM_GBI1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI1");
    auto LM_GBD2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD2");
    auto LM_GBD1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD1");

    Vector<SpriteFrame*> GBIFrames = { LM_GBI1, LM_GBI2, LM_PI };
    auto GBIAnimacion = Animation::createWithSpriteFrames(GBIFrames, 0.4f);
    auto GBIAnimate = Animate::create(GBIAnimacion);
    GBIAnimate->retain();

    Vector<SpriteFrame*> GBDFrames = { LM_GBD1, LM_GBD2, LM_PI };
    auto GBDAnimacion = Animation::createWithSpriteFrames(GBDFrames, 0.4f);
    auto GBDAnimate = Animate::create(GBDAnimacion);
    GBDAnimate->retain();

    auto LM_GR = ResourceManager::getInstance()->getSpriteFrame("LM_GR");
    auto LM_ES = ResourceManager::getInstance()->getSpriteFrame("LM_ES");
    auto LM_KOI2 = ResourceManager::getInstance()->getSpriteFrame("LM_KOI2");

    auto LM_EI = ResourceManager::getInstance()->getSpriteFrame("LM_EI");
    auto LM_ED = ResourceManager::getInstance()->getSpriteFrame("LM_ED");

    Vector<SpriteFrame*> EsquivaIFrames = { LM_EI, LM_PI };
    auto EsquivaIAnimacion = Animation::createWithSpriteFrames(EsquivaIFrames, 1.0f);
    auto EsquivaIAnimate = Animate::create(EsquivaIAnimacion);
    EsquivaIAnimate->retain();

    Vector<SpriteFrame*> EsquivaDFrames = { LM_ED, LM_PI };
    auto EsquivaDAnimacion = Animation::createWithSpriteFrames(EsquivaDFrames, 1.0f);
    auto EsquivaDAnimate = Animate::create(EsquivaDAnimacion);
    EsquivaDAnimate->retain();

    auto LM_DD = ResourceManager::getInstance()->getSpriteFrame("LM_DD");
    auto LM_KOD1 = ResourceManager::getInstance()->getSpriteFrame("LM_KOD1");

    auto LM_S1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_S2 = ResourceManager::getInstance()->getSpriteFrame("LM_S2");
    auto LM_S3 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");
    auto LM_S4 = ResourceManager::getInstance()->getSpriteFrame("LM_S4");
    auto LM_S5 = ResourceManager::getInstance()->getSpriteFrame("LM_S5");

    Vector<SpriteFrame*> SuperFrames = { LM_S1, LM_S2, LM_S3, LM_S4, LM_S5, LM_PI };
    auto SuperAnimacion = Animation::createWithSpriteFrames(SuperFrames, 0.2f);
    auto SuperAnimate = Animate::create(SuperAnimacion);
    SuperAnimate->retain();

    Vector<SpriteFrame*> GoDownFrames = { LM_GR, LM_PI };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR, LM_PI };
    auto BloqAnimacion = Animation::createWithSpriteFrames(BloqFrames, 1.0f);
    auto BloqAnimate = Animate::create(BloqAnimacion);
    BloqAnimate->retain();

    auto GJ_C1 = ResourceManager::getInstance()->getSpriteFrame("GJ1");
    auto GJ_C2 = ResourceManager::getInstance()->getSpriteFrame("GJ2");
    auto GJ_PI = ResourceManager::getInstance()->getSpriteFrame("GJ3");
    auto GJ_MOV = ResourceManager::getInstance()->getSpriteFrame("GJ4");

    auto GJ_ANIM2 = ResourceManager::getInstance()->getSpriteFrame("GJ5");
    auto GJ_ESQ_D = ResourceManager::getInstance()->getSpriteFrame("GJ6");
    auto GJ_ESQ_I = ResourceManager::getInstance()->getSpriteFrame("GJ44");
    auto GJ_GA = ResourceManager::getInstance()->getSpriteFrame("GJ7");
    auto GJ_GB = ResourceManager::getInstance()->getSpriteFrame("GJ8");

    auto GJ_JABIZQ1 = ResourceManager::getInstance()->getSpriteFrame("GJ9");
    auto GJ_JABIZQ2 = ResourceManager::getInstance()->getSpriteFrame("GJ10");
    auto GJ_JABDER1 = ResourceManager::getInstance()->getSpriteFrame("GJ45");
    auto GJ_JABDER2 = ResourceManager::getInstance()->getSpriteFrame("GJ46");

    auto GJ_GRD1 = ResourceManager::getInstance()->getSpriteFrame("GJ11");
    auto GJ_GRD2 = ResourceManager::getInstance()->getSpriteFrame("GJ12");
    auto GJ_GRI1 = ResourceManager::getInstance()->getSpriteFrame("GJ47");
    auto GJ_GRI2 = ResourceManager::getInstance()->getSpriteFrame("GJ48");

    auto GJ_GD2 = ResourceManager::getInstance()->getSpriteFrame("GJ13");
    auto GJ_GD1 = ResourceManager::getInstance()->getSpriteFrame("GJ14");
    auto GJ_GI2 = ResourceManager::getInstance()->getSpriteFrame("GJ50");
    auto GJ_GI1 = ResourceManager::getInstance()->getSpriteFrame("GJ49");

    auto GJ_GBD2 = ResourceManager::getInstance()->getSpriteFrame("GJ15");
    auto GJ_GBD3 = ResourceManager::getInstance()->getSpriteFrame("GJ16");
    auto GJ_GBD1 = ResourceManager::getInstance()->getSpriteFrame("GJ17");
    auto GJ_GBI2 = ResourceManager::getInstance()->getSpriteFrame("GJ51");
    auto GJ_GBI3 = ResourceManager::getInstance()->getSpriteFrame("GJ52");
    auto GJ_GBI1 = ResourceManager::getInstance()->getSpriteFrame("GJ53");

    auto GJ_PROV1 = ResourceManager::getInstance()->getSpriteFrame("GJ18");
    auto GJ_PROV2 = ResourceManager::getInstance()->getSpriteFrame("GJ19");
    auto GJ_PROV3 = ResourceManager::getInstance()->getSpriteFrame("GJ20");
    //auto GJ21 = ResourceManager::getInstance()->getSpriteFrame("GJ21");
    //auto GJ22 = ResourceManager::getInstance()->getSpriteFrame("GJ22");
    auto GJ_UP2 = ResourceManager::getInstance()->getSpriteFrame("GJ23");

    auto GJ_UP1 = ResourceManager::getInstance()->getSpriteFrame("GJ24");
    auto GJ_DIS1 = ResourceManager::getInstance()->getSpriteFrame("GJ25");
    auto GJ_DIS2 = ResourceManager::getInstance()->getSpriteFrame("GJ26");
    //auto GJ27 = ResourceManager::getInstance()->getSpriteFrame("GJ27");
    //auto GJ28 = ResourceManager::getInstance()->getSpriteFrame("GJ28");

    auto GJ_ANIM4 = ResourceManager::getInstance()->getSpriteFrame("GJ29");
    //auto GJ30 = ResourceManager::getInstance()->getSpriteFrame("GJ30");
    //auto GJ31 = ResourceManager::getInstance()->getSpriteFrame("GJ31");
    //auto GJ32 = ResourceManager::getInstance()->getSpriteFrame("GJ32");

    auto GJ_GOLPEADOAD = ResourceManager::getInstance()->getSpriteFrame("GJ33");
    auto GJ_GOLPEADOAI = ResourceManager::getInstance()->getSpriteFrame("GJ54");

    Vector<SpriteFrame*> GJ_GLPDFrames = { GJ_GOLPEADOAD, GJ_PI };
    auto GJ_GLPDAnimacion = Animation::createWithSpriteFrames(GJ_GLPDFrames, 1.0f);
    auto GJ_GLPDAnimate = Animate::create(GJ_GLPDAnimacion);
    GJ_GLPDAnimate->retain();

    Vector<SpriteFrame*> GJ_GLPIFrames = { GJ_GOLPEADOAI, GJ_PI };
    auto GJ_GLPIAnimacion = Animation::createWithSpriteFrames(GJ_GLPIFrames, 1.0f);
    auto GJ_GLPIAnimate = Animate::create(GJ_GLPIAnimacion);
    GJ_GLPIAnimate->retain();

    auto GJ_GOLPEADOBI = ResourceManager::getInstance()->getSpriteFrame("GJ34");
    auto GJ_GOLPEADOBD = ResourceManager::getInstance()->getSpriteFrame("GJ55");

    Vector<SpriteFrame*> GJ_GLPBDFrames = { GJ_GOLPEADOBD, GJ_PI };
    auto GJ_GLPBDAnimacion = Animation::createWithSpriteFrames(GJ_GLPBDFrames, 1.0f);
    auto GJ_GLPBDAnimate = Animate::create(GJ_GLPBDAnimacion);
    GJ_GLPBDAnimate->retain();

    Vector<SpriteFrame*> GJ_GLPBIFrames = { GJ_GOLPEADOBI, GJ_PI };
    auto GJ_GLPBIAnimacion = Animation::createWithSpriteFrames(GJ_GLPBIFrames, 1.0f);
    auto GJ_GLPBIAnimate = Animate::create(GJ_GLPBIAnimacion);
    GJ_GLPBIAnimate->retain();

    auto GJ_GOLPEADOJD = ResourceManager::getInstance()->getSpriteFrame("GJ35");
    auto GJ_GOLPEADOJI = ResourceManager::getInstance()->getSpriteFrame("GJ56");

    Vector<SpriteFrame*> GJ_GLPJDFrames = { GJ_GOLPEADOJD, GJ_PI };
    auto GJ_GLPJDAnimacion = Animation::createWithSpriteFrames(GJ_GLPJDFrames, 1.0f);
    auto GJ_GLPJDAnimate = Animate::create(GJ_GLPJDAnimacion);
    GJ_GLPJDAnimate->retain();

    Vector<SpriteFrame*> GJ_GLPJIFrames = { GJ_GOLPEADOJI, GJ_PI };
    auto GJ_GLPJIAnimacion = Animation::createWithSpriteFrames(GJ_GLPJIFrames, 1.0f);
    auto GJ_GLPJIAnimate = Animate::create(GJ_GLPJIAnimacion);
    GJ_GLPJIAnimate->retain();

    auto GJ_GOLPEADOGI = ResourceManager::getInstance()->getSpriteFrame("GJ36");
    auto GJ_GOLPEADOGD = ResourceManager::getInstance()->getSpriteFrame("GJ57");

    Vector<SpriteFrame*> GJ_GLPGDFrames = { GJ_GOLPEADOGD, GJ_PI };
    auto GJ_GLPGDAnimacion = Animation::createWithSpriteFrames(GJ_GLPGDFrames, 1.0f);
    auto GJ_GLPGDAnimate = Animate::create(GJ_GLPGDAnimacion);
    GJ_GLPGDAnimate->retain();

    Vector<SpriteFrame*> GJ_GLPGIFrames = { GJ_GOLPEADOGI, GJ_PI };
    auto GJ_GLPGIAnimacion = Animation::createWithSpriteFrames(GJ_GLPGIFrames, 1.0f);
    auto GJ_GLPGIAnimate = Animate::create(GJ_GLPGIAnimacion);
    GJ_GLPGIAnimate->retain();

    auto GJ_KO1 = ResourceManager::getInstance()->getSpriteFrame("GJ37");
    auto GJ_KO2 = ResourceManager::getInstance()->getSpriteFrame("GJ38");
    auto GJ_KO3 = ResourceManager::getInstance()->getSpriteFrame("GJ39");

    Vector<SpriteFrame*> GJ_KOFrames = { GJ_KO1, GJ_KO2, GJ_KO3 };
    auto GJ_KOAnimacion = Animation::createWithSpriteFrames(GJ_KOFrames, 3.0f);
    auto GJ_KOAnimate = Animate::create(GJ_KOAnimacion);
    GJ_KOAnimate->retain();

    auto GJ_LEV1 = ResourceManager::getInstance()->getSpriteFrame("GJ40");
    auto GJ_LEV2 = ResourceManager::getInstance()->getSpriteFrame("GJ41");
    auto GJ_LEV3 = ResourceManager::getInstance()->getSpriteFrame("GJ42");
    auto GJ_LEV4 = ResourceManager::getInstance()->getSpriteFrame("GJ43");

    Vector<SpriteFrame*> GJ_ResFrames = { GJ_LEV1, GJ_LEV2, GJ_LEV3, GJ_LEV4, GJ_PI };
    auto GJ_ResAnimacion = Animation::createWithSpriteFrames(GJ_ResFrames, 3.0f);
    auto GJ_ResAnimate = Animate::create(GJ_ResAnimacion);
    GJ_ResAnimate->retain();


    Vector<SpriteFrame*> GJ_ESQD_Frames = { GJ_ESQ_D, GJ_PI };
    auto GJ_ESQD_Animacion = Animation::createWithSpriteFrames(GJ_ESQD_Frames, 1.0f);
    auto GJ_ESQD_Animate = Animate::create(GJ_ESQD_Animacion);
    GJ_ESQD_Animate->retain();

    Vector<SpriteFrame*> GJ_ESQI_Frames = { GJ_ESQ_I, GJ_PI };
    auto GJ_ESQI_Animacion = Animation::createWithSpriteFrames(GJ_ESQI_Frames, 1.0f);
    auto GJ_ESQI_Animate = Animate::create(GJ_ESQI_Animacion);
    GJ_ESQI_Animate->retain();

    Vector<SpriteFrame*> GJ_GuardiaAFrames = { GJ_GA, GJ_PI };
    auto GJ_GuardiaAAnimacion = Animation::createWithSpriteFrames(GJ_GuardiaAFrames, 1.0f);
    auto GJ_GuardiaAAnimate = Animate::create(GJ_GuardiaAAnimacion);
    GJ_GuardiaAAnimate->retain();

    Vector<SpriteFrame*> GJ_GuardiaBFrames = { GJ_GB, GJ_PI };
    auto GJ_GuardiaBAnimacion = Animation::createWithSpriteFrames(GJ_GuardiaBFrames, 1.0f);
    auto GJ_GuardiaBAnimate = Animate::create(GJ_GuardiaBAnimacion);
    GJ_GuardiaBAnimate->retain();

    Vector<SpriteFrame*> GJ_JabIzqFrames = { GJ_JABIZQ1, GJ_JABIZQ2, GJ_PI };
    auto GJ_JabIzqAnimacion = Animation::createWithSpriteFrames(GJ_JabIzqFrames, 0.8f);
    auto GJ_JabIzqAnimate = Animate::create(GJ_JabIzqAnimacion);
    GJ_JabIzqAnimate->retain();

    Vector<SpriteFrame*> GJ_JabDerFrames = { GJ_JABDER1, GJ_JABDER2, GJ_PI };
    auto GJ_JabDerAnimacion = Animation::createWithSpriteFrames(GJ_JabDerFrames, 0.8f);
    auto GJ_JabDerAnimate = Animate::create(GJ_JabDerAnimacion);
    GJ_JabDerAnimate->retain();

    Vector<SpriteFrame*> GJ_GBIFrames = { GJ_GBI1, GJ_GBI2, GJ_GBI3, GJ_PI };
    auto GJ_GBIAnimacion = Animation::createWithSpriteFrames(GJ_GBIFrames, 0.8f);
    auto GJ_GBIAnimate = Animate::create(GJ_GBIAnimacion);
    GJ_GBIAnimate->retain();

    Vector<SpriteFrame*> GJ_GBDFrames = { GJ_GBD1, GJ_GBD2, GJ_GBD3, GJ_PI };
    auto GJ_GBDAnimacion = Animation::createWithSpriteFrames(GJ_GBDFrames, 0.8f);
    auto GJ_GBDAnimate = Animate::create(GJ_GBDAnimacion);
    GJ_GBDAnimate->retain();

    Vector<SpriteFrame*> GJ_GRDFrames = { GJ_GRD1, GJ_GRD2, GJ_PI };
    auto GJ_GRDAnimacion = Animation::createWithSpriteFrames(GJ_GRDFrames, 0.8f);
    auto GJ_GRDAnimate = Animate::create(GJ_GRDAnimacion);
    GJ_GRDAnimate->retain();

    Vector<SpriteFrame*> GJ_GRIFrames = { GJ_GRI1, GJ_GRI2, GJ_PI };
    auto GJ_GRIAnimacion = Animation::createWithSpriteFrames(GJ_GRIFrames, 0.8f);
    auto GJ_GRIAnimate = Animate::create(GJ_GRIAnimacion);
    GJ_GRIAnimate->retain();

    Vector<SpriteFrame*> GJ_ProvFrames = { GJ_PROV1, GJ_PROV2, GJ_PROV3, GJ_PI };
    auto GJ_ProvAnimacion = Animation::createWithSpriteFrames(GJ_ProvFrames, 0.8f);
    auto GJ_ProvAnimate = Animate::create(GJ_ProvAnimacion);
    GJ_ProvAnimate->retain();

    Vector<SpriteFrame*> GJ_UPFrames = { GJ_UP1, GJ_UP2, GJ_PI };
    auto GJ_UPAnimacion = Animation::createWithSpriteFrames(GJ_UPFrames, 0.8f);
    auto GJ_UPAnimate = Animate::create(GJ_UPAnimacion);
    GJ_UPAnimate->retain();

    Vector<SpriteFrame*> GJ_DISFrames = { GJ_DIS1, GJ_DIS2, GJ_PI };
    auto GJ_DISAnimacion = Animation::createWithSpriteFrames(GJ_DISFrames, 0.8f);
    auto GJ_DISAnimate = Animate::create(GJ_DISAnimacion);
    GJ_DISAnimate->retain();

    auto rivalSprite = Sprite::createWithSpriteFrame(GJ_PI);
    rivalSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    this->addChild(rivalSprite);

    auto playerSprite = Sprite::createWithSpriteFrame(LM_PI);
    playerSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 250));
    this->addChild(playerSprite);

    // Configurar jugador en la capa superior
    playerSprite->setLocalZOrder(3);

    // Configurar rival en la capa intermedia
    rivalSprite->setLocalZOrder(2);

    // Configurar barras de vida en la capa inferior
    playerHealthBar->setLocalZOrder(1);
    rivalHealthBar->setLocalZOrder(1);


    this->schedule([this, rivalSprite, GJ_JabIzqAnimate, GJ_JabDerAnimate, GJ_GBIAnimate, GJ_GBDAnimate, GJ_GRDAnimate, GJ_GRIAnimate, GJ_DISAnimate, GJ_ProvAnimate, GJ_UPAnimate](float delta) {
        if (!isAnimating && !isReacting) {
            rivalAction(rivalSprite, GJ_JabIzqAnimate, GJ_JabDerAnimate, GJ_GBIAnimate, GJ_GBDAnimate, GJ_GRDAnimate, GJ_GRIAnimate, GJ_DISAnimate, GJ_ProvAnimate, GJ_UPAnimate); // Acción del rival
        }
        }, 1.5f, "RivalActionScheduler");

    // Recuperar fatiga del rival gradualmente
    this->scheduleOnce([this](float delta) {
        if (rivalFatigue < 100)
        {
            rivalFatigue = std::min(100, rivalFatigue + 5); // Recuperar 5 puntos
            CCLOG("Fatiga del rival recuperada a: %d", rivalFatigue);
        }
        }, 1.0f, "RecoverRivalFatigue");

    /*
    this->schedule([this, rivalSprite, BH_Bur](float delta) {
        if (!isAnimating && !isReacting) {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
            });

            rivalSprite->stopAllActions();
            rivalSprite->runAction(Sequence::create(BH_Bur, callback, nullptr));
        }
    }, 1.0f, "RivalIdleScheduler");
    */

    // Variable para estado de defensa
    bool isDefending = false;

    // Evento para capturar el ataque del jugador
    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) mutable {
        if (isAnimating) return;
        if (playerFatigue < fatigueThreshold) {
            CCLOG("¡Demasiado cansado para atacar!");
            return;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_Q) { // Jab Izquierdo
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Izquierdo)");
            isAnimating = true;

            processAI(1, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_E) { // Jab Derecho
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Jab Derecho)");
            isAnimating = true;

            processAI(2, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(jabDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 10);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) { // Jab Derecho
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Derecho)");
            isAnimating = true;

            processAI(3, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanDerAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) { // Jab Derecho
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(4, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GanIzqAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) { // Upper Cut
            if (isReacting) return;
            CCLOG("El jugador lanza un ataque (UpperCut)");
            isAnimating = true;

            processAI(5, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(UpperCutAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 30);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_Z) // Bloquear
        {
            CCLOG("El jugador lanza un ataque (Golpe Bajo Izquierdo)");
            isAnimating = true;

            processAI(6, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBIAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_C) // Bloquear
        {
            CCLOG("El jugador lanza un ataque (Gancho Izquierdo)");
            isAnimating = true;

            processAI(7, rivalSprite, GJ_GLPDAnimate, GJ_GLPIAnimate, GJ_GLPBDAnimate, GJ_GLPBIAnimate, GJ_GLPJDAnimate, GJ_GLPJIAnimate, GJ_GLPGDAnimate, GJ_GLPGIAnimate, GJ_ESQD_Animate, GJ_ESQI_Animate, GJ_GuardiaAAnimate, GJ_GuardiaBAnimate, isDefending);

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(GBDAnimate, callback, nullptr));

            reduceFatigue();

            if (isDefending) {
                CCLOG("El golpe fue bloqueado por el rival");
            }
            else {
                applyDamage(0, 20);
                CCLOG("El rival recibe daño");
            }
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.1f, Vec2(-10, 0)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, EsquivaIAnimate, moveBack, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.1f, Vec2(10, 0)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(EsquivaDAnimate, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_R) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(BloqAnimate, callback, nullptr));
            reactionAct = 1;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) // Esquivar
        {
            isReacting = true;
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(0, -200)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(moveBy, GoDownAnimate, moveBack, callback, nullptr));
            reactionAct = 2;
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_P) {
            if (currentState == 2) {
                currentState = 3;
                changeGameState(GameState::PAUSED);
                CCLOG("Pausa");
            }
            else if (currentState == 3) {
                changeGameState(GameState::IN_GAME);
                currentState = 2;
            }
        }


        checkLife();
        if (vidacountGJ > 0) {
            if (rivalHealth < 40) {
                playRivalIdleAnimation(rivalSprite, GJ_UPAnimate);
                rivalHealth += 50;
                vidacountGJ--;
                updateHealthBars();
            }

        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

    return true;
};

void GJFIGHT::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}

Scene* StartMenu::createScene()
{
    return StartMenu::create();
}

bool StartMenu::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("fondomenu.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    auto Titulo = ResourceManager::getInstance()->getSpriteFrame("Titulo");

    auto TituloSprite = Sprite::createWithSpriteFrame(Titulo);
    TituloSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 375));
    this->addChild(TituloSprite);

    auto Start = Sprite::create("PushStart.png");
    if (Start){
        Start->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));
        this->addChild(Start);
    }else{
        CCLOG("Error: no se pudo cargar Start.png");
    }

    auto Quit = Sprite::create("Quit.png");
    if (Quit) {
        Quit->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));
        this->addChild(Quit);
    }
    else {
        CCLOG("Error: no se pudo cargar Quit.png");
    }

    auto listener2 = EventListenerMouse::create();
    listener2->onMouseDown = [this, Start, Quit](Event* event) {

        EventMouse* mouseEvent = (EventMouse*)event;
        Vec2 mousePos = mouseEvent->getLocation();
        mousePos.y = Director::getInstance()->getVisibleSize().height - mousePos.y;

        if (Start && Start->getBoundingBox().containsPoint(mousePos)) {
            log("Start Selected");
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, Name::createScene()));
        }
        else if (Quit && Quit->getBoundingBox().containsPoint(mousePos)) {
            log("Start Selected");
            Director::getInstance()->end();
        }

        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

    return true;
}

Scene* GJ_Face::createScene()
{
    return GJ_Face::create();
}

bool GJ_Face::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("GabbyJay.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    return true;
}

Scene* NK_Face::createScene()
{
    return NK_Face::create();
}

bool NK_Face::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("NickBruiser.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    return true;

}

Scene* BH_Face::createScene()
{
    return BH_Face::create();
}

bool BH_Face::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("WorldCircuit.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    return true;

}

Scene* NK_Victory::createScene()
{
    return NK_Victory::create();
}

bool NK_Victory::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("NK_END.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    return true;

}