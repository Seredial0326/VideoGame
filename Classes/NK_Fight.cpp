#include "NK_Fight.h"
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
//reactionTime = 1.5f; // Tiempo en segundos para reaccionar

//int totalTime = 180;
//int playerReaction = 0;

//int attackType = 0;
//int reactionAct = 0;

const int HOOK_ATTACK = 1;
const int DOWN_ATTACK = 2;
const int JAB_ATTACK = 3;
const int SUPPER = 4;

int vidacount = 3;

int currentState = 2;

// Sprites para las barras de salud
//cocos2d::Sprite* playerHealthBar;
//cocos2d::Sprite* rivalHealthBar;
//cocos2d::Sprite* playerFatigueBar

void NKFIGHT::changeGameState(GameState newState) {
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

void NKFIGHT::pauseGame() {
    Director::getInstance()->pause(); // Pausa el motor del juego
    auto pauseLayer = LayerColor::create(Color4B(0, 0, 0, 128)); // Agregar una capa semitransparente
    auto label = Label::createWithTTF("Pausa", "fonts/Marker Felt.ttf", 32);
    label->setPosition(Director::getInstance()->getVisibleSize() / 2);
    pauseLayer->addChild(label);
    this->addChild(pauseLayer, 10, "PauseLayer");
}

void NKFIGHT::resumeGame() {
    Director::getInstance()->resume(); // Reanuda el motor del juego
    this->removeChildByName("PauseLayer"); // Elimina la capa de pausa
}



void NKFIGHT::updateHealthBars()
{
    // Escala las barras según la salud restante
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

void NKFIGHT::setupTimer() {
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

void NKFIGHT::reduceFatigue()
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
    // Reducir salud
    playerHealth = std::max(1, playerHealth - damageToPlayer);
    rivalHealth = std::max(1, rivalHealth - damageToRival);

    // Actualizar barras de salud
    updateHealthBars();

    // Verificar si alguien perdió
    if (playerHealth == 1)
    {
        CCLOG("¡El jugador perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, NKFIGHT::createScene(), Color3B::RED));
    }
    else if (rivalHealth == 1)
    {
        CCLOG("¡El rival perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, NKFIGHT::createScene(), Color3B::GREEN));

        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));

    }
}

void NKFIGHT::setupBackground()
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
    } else {
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
        if (vidacount == 1) {
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
        if (vidacount == 1) {
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
        if (vidacount == 1) {
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
    }, 1.0f, "ReactionTimer");

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
/*
void HelloWorld::playRivalIdleAnimation(cocos2d::Sprite* rivalSprite, cocos2d::Action* GJ_ProvAnimate)
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
*/
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
void NKFIGHT::processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* NK_GLPBDAnimate, cocos2d::Action* NK_GLPBIAnimate, cocos2d::Action* NK_GLPJDAnimate, cocos2d::Action* NK_GLPJIAnimate, cocos2d::Action* NK_GLPGDAnimate, cocos2d::Action* NK_GLPGIAnimate, cocos2d::Action* NK_BLBAnimate, cocos2d::Action* NK_BLAAnimate, cocos2d::Action* NK_ESQADAnimate, cocos2d::Action* NK_ESQAIAnimate, cocos2d::Action* NK_ESQIAnimate, cocos2d::Action* NK_ESQDAnimate, bool& isDefending){

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

Scene* NKFIGHT::createScene()
{
    return NKFIGHT::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
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

    Vector<SpriteFrame*> GoDownFrames = { LM_GR };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR };
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

    auto NK_VIC1 = ResourceManager::getInstance()->getSpriteFrame("NK45");
    auto NK_VIC2 = ResourceManager::getInstance()->getSpriteFrame("NK46");

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

    // Configurar jugador en la capa superior
    playerSprite->setLocalZOrder(3);

    // Configurar rival en la capa intermedia
    rivalSprite->setLocalZOrder(2);

    // Configurar barras de vida en la capa inferior
    playerHealthBar->setLocalZOrder(1);
    rivalHealthBar->setLocalZOrder(1);


    this->schedule([this, rivalSprite, NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate](float delta) {
        if (!isAnimating && !isReacting) {
            rivalAction(rivalSprite, NK_JDAnimate, NK_JIAnimate, NK_GADAnimate, NK_GAIAnimate, NK_GBDAnimate, NK_GBIAnimate, NK_GDAnimate, NK_GIAnimate); // Acción del rival
        }
        }, 1.5f, "RivalActionScheduler");

    // Recuperar fatiga del rival gradualmente
    this->scheduleOnce([this](float delta) {
        if (rivalFatigue < 100)
        {
            rivalFatigue = std::min(100, rivalFatigue + 3); // Recuperar 5 puntos
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_E) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) { // Upper Cut
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_Z) // Bloquear
        {
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_C) // Bloquear
        {
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) // Bloquear
        {
            isAnimating = true;

            auto callback = CallFunc::create([=]() {
                isAnimating = false;
                isReacting = false;
                });

            auto moveBy = MoveBy::create(0.05f, Vec2(-10, 0)); // Moverse ligeramente hacia la derecha
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

            auto moveBy = MoveBy::create(0.05f, Vec2(10, 0)); // Moverse ligeramente hacia la derecha
            auto moveBack = moveBy->reverse();

            playerSprite->stopAllActions();
            playerSprite->runAction(Sequence::create(EsquivaDAnimate, callback, nullptr));
            reactionAct = 2;
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
            reactionAct = 3;
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
        /*
        if (rivalHealth < 40) {
            SupperAttack(rivalSprite);
        }*/

    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

    return true;
};

void NKFIGHT::menuCloseCallback(Ref* pSender)
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
            auto newScene = NKFIGHT::createScene();
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