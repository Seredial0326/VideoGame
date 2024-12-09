#include "GJ_Fight.h"
#include "ResourceManager.h"
#include "StartMenuScene.h"
#include "Variables.h"
#include <random>
#include <VictoryScene.h>
#include <GameOverScene.h>

USING_NS_CC;

// Variables para la salud
//int playerHealth = 100;
//int rivalHealth = 100;
//int MAX_HEALTH = 100;
//int playerFatigue = 100; // Máximo de 100
//const int fatigueThreshold = 30; // Umbral mínimo para poder atacar
//int rivalFatigue = 100; // Fatiga del rival
//const int rivalFatigueThreshold = 20; // Umbral mínimo para atacar
//bool isReacting = false;  // Indica si está en tiempo de reacción
//std::string rivalAttackType = ""; // Tipo de ataque del rival
//float reactionTime = 1.5f; // Tiempo en segundos para reaccionar

//int totalTime = 180;
//int playerReaction = 0;

//int attackType = 0;
//int reactionAct = 0;

const int RECT_ATTACK = 1;         // Gancho
const int DOWN_ATTACK = 2;      // Golpe doble
const int JAB_ATTACK = 3;    // Jab
const int ANIMATION_TAUNT = 4;    // Jab

int vidacount = 3;

int currentState = 2;

// Sprites para las barras de salud
//cocos2d::Sprite* playerHealthBar;
//cocos2d::Sprite* rivalHealthBar;
//cocos2d::Sprite* playerFatigueBar

void GJFIGHT::changeGameState(GameState newState) {
    CCLOG("LLAMADO");
    switch (newState) {
    case GameState::START_MENU:
        CCLOG("Estado: Menú principal");
        // Cargar escena del menú principal
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));
        break;

    case GameState::IN_GAME:
        CCLOG("Estado: En el juego");
        // Reanudar acciones o preparar combate
        this->resumeGame();
        break;

    case GameState::PAUSED:
        CCLOG("Estado: Pausado");
        this->pauseGame();
        break;

    case GameState::GAME_OVER:
        CCLOG("Estado: Juego terminado");
        // Mostrar pantalla de derrota o reiniciar
        //Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GameOverScene::createScene()));
        break;

    case GameState::VICTORY:
        CCLOG("Estado: Victoria");
        // Mostrar pantalla de victoria
        //Director::getInstance()->replaceScene(TransitionFade::create(1.0f, VictoryScene::createScene()));
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
    // Escala las barras según la salud restante
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
            this->unschedule("UpdateCounter"); // Detener el scheduler
            counterLabel->setString("¡Fin del tiempo!"); // Mostrar mensaje de fin
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

    // Verificar si alguien perdió
    if (playerHealth == 1)
    {
        CCLOG("¡El jugador perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GJFIGHT::createScene(), Color3B::RED));
    }
    else if (rivalHealth == 1)
    {
        CCLOG("¡El rival perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GJFIGHT::createScene(), Color3B::GREEN));

        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));

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

/*
void HelloWorld::checkPlayerReaction(int attackType)
{
    if (attackType == ATTACK_HOOK)
    {
        // Verificar si el jugador bloqueó a tiempo
        if (playerReaction == 1)
        {
            log("Jugador bloqueó el gancho.");
        }
        else
        {
            playerHealth -= 10; // El daño puede ser ajustado
            log("Jugador recibió daño del gancho.");
        }
    }
    else if (attackType == ATTACK_DOUBLE)
    {
        // Verificar si el jugador esquivó a tiempo
        if (playerReaction == 2)
        {
            log("Jugador esquivó el golpe doble.");
        }
        else
        {
            playerHealth -= 15; // Ajusta el daño
            log("Jugador recibió daño del golpe doble.");
        }
    }
}
*/

// Definición de la función como miembro de HelloWorld
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

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
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
    auto jabIzqAnimacion = Animation::createWithSpriteFrames(jabIzqFrames, 0.5f);
    auto jabIzqAnimate = Animate::create(jabIzqAnimacion);
    jabIzqAnimate->retain();

    Vector<SpriteFrame*> jabDerFrames = { LM_JabDer1, LM_JabDer2, LM_PI };
    auto jabDerAnimacion = Animation::createWithSpriteFrames(jabDerFrames, 0.5f);
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
    auto GanDerAnimacion = Animation::createWithSpriteFrames(GanDerFrames, 1.0f);
    auto GanDerAnimate = Animate::create(GanDerAnimacion);
    GanDerAnimate->retain();

    Vector<SpriteFrame*> GanIzqFrames = { LM_GanIzq1, LM_GanIzq2, LM_GanIzq3, LM_GanIzq4, LM_PI };
    auto GanIzqAnimacion = Animation::createWithSpriteFrames(GanIzqFrames, 1.0f);
    auto GanIzqAnimate = Animate::create(GanIzqAnimacion);
    GanIzqAnimate->retain();

    auto LM_GAMD1 = ResourceManager::getInstance()->getSpriteFrame("LM_S1");
    auto LM_GAMD2 = ResourceManager::getInstance()->getSpriteFrame("LM_S3");

    Vector<SpriteFrame*> UpperCutFrames = { LM_GAMD1, LM_GAMD2, LM_PI };
    auto UpperCutAnimacion = Animation::createWithSpriteFrames(UpperCutFrames, 0.5f);
    auto UpperCutAnimate = Animate::create(UpperCutAnimacion);
    UpperCutAnimate->retain();

    auto LM_GBI2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI2");
    auto LM_GBI1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBI1");
    auto LM_GBD2 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD2");
    auto LM_GBD1 = ResourceManager::getInstance()->getSpriteFrame("LM_GBD1");

    Vector<SpriteFrame*> GBIFrames = { LM_GBI1, LM_GBI2, LM_PI };
    auto GBIAnimacion = Animation::createWithSpriteFrames(GBIFrames, 0.5f);
    auto GBIAnimate = Animate::create(GBIAnimacion);
    GBIAnimate->retain();

    Vector<SpriteFrame*> GBDFrames = { LM_GBD1, LM_GBD2, LM_PI };
    auto GBDAnimacion = Animation::createWithSpriteFrames(GBDFrames, 0.5f);
    auto GBDAnimate = Animate::create(GBDAnimacion);
    GBDAnimate->retain();

    auto LM_GR = ResourceManager::getInstance()->getSpriteFrame("LM_GR");
    auto LM_ES = ResourceManager::getInstance()->getSpriteFrame("LM_ES");
    auto LM_KOI2 = ResourceManager::getInstance()->getSpriteFrame("LM_KOI2");

    auto LM_EI = ResourceManager::getInstance()->getSpriteFrame("LM_EI");
    auto LM_ED = ResourceManager::getInstance()->getSpriteFrame("LM_ED");

    Vector<SpriteFrame*> EsquivaIFrames = { LM_EI, LM_PI };
    auto EsquivaIAnimacion = Animation::createWithSpriteFrames(EsquivaIFrames, 2.0f);
    auto EsquivaIAnimate = Animate::create(EsquivaIAnimacion);
    EsquivaIAnimate->retain();

    Vector<SpriteFrame*> EsquivaDFrames = { LM_ED, LM_PI };
    auto EsquivaDAnimacion = Animation::createWithSpriteFrames(EsquivaDFrames, 2.0f);
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
    auto SuperAnimacion = Animation::createWithSpriteFrames(SuperFrames, 2.0f);
    auto SuperAnimate = Animate::create(SuperAnimacion);
    SuperAnimate->retain();

    Vector<SpriteFrame*> GoDownFrames = { LM_GR };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR };
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
    }, 3.0f, "RecoverRivalFatigue");

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
        }else if (keyCode == EventKeyboard::KeyCode::KEY_P) {
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

        

        if (vidacount > 0) {
            if (rivalHealth < 40) {
                playRivalIdleAnimation(rivalSprite, GJ_UPAnimate);
                rivalHealth += 50;
                vidacount--;
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

    auto levelOne = Sprite::create("PushStart.png");
    if (levelOne)
    {
        levelOne->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));
        this->addChild(levelOne);
    }
    else
    {
        CCLOG("Error: no se pudo cargar levelOne.png");
    }

    auto listener2 = EventListenerMouse::create();
    listener2->onMouseDown = [this, levelOne](Event* event) {

        EventMouse* mouseEvent = (EventMouse*)event;
        Vec2 mousePos = mouseEvent->getLocation();
        mousePos.y = Director::getInstance()->getVisibleSize().height - mousePos.y;

        if (levelOne && levelOne->getBoundingBox().containsPoint(mousePos)) {
            log("Level 1 Selected");
            auto newScene = GJFIGHT::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, newScene));
        }

        /*
        this->runAction(Sequence::create(
            DelayTime::create(2.5f),
            CallFunc::create([backgroundId]() {

                AudioEngine::resume(backgroundId);

                }), nullptr));

        if (nextScene && nextScene->getBoundingBox().containsPoint(mousePos)) {
            log("Next Scene...");
            AudioEngine::stop(backgroundId);
            Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MainMenu::createScene()));
        }
        */

        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener2, this);

    return true;
}