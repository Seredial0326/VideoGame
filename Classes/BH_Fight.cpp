#include "BH_Fight.h"
#include "ResourceManager.h"
#include "StartMenuScene.h"
#include "Variables.h"
#include <random>

USING_NS_CC;

const int ATTACK_HOOK = 1;         // Gancho
const int ATTACK_DOUBLE = 2;      // Golpe doble
const int ANIMATION_TAUNT = 3;    // Animación burlesca

void BHFIGHT::updateHealthBars()
{
    // Escala las barras según la salud restante
    float playerScaleX = static_cast<float>(playerHealth)*10 / 100.0f;
    float rivalScaleX = static_cast<float>(rivalHealth)*10 / 100.0f;
    float playerFatigueScaleX = static_cast<float>(playerFatigue)*2 / 100.0f;

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
    playerHealth = std::max(0, playerHealth - damageToPlayer);
    rivalHealth = std::max(0, rivalHealth - damageToRival);

    // Actualizar barras de salud
    updateHealthBars();

    // Verificar si alguien perdió
    if (playerHealth == 0)
    {
        CCLOG("¡El jugador perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, BHFIGHT::createScene(), Color3B::RED));
    }
    else if (rivalHealth == 0)
    {
        CCLOG("¡El rival perdió!");
        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, BHFIGHT::createScene(), Color3B::GREEN));

        Director::getInstance()->replaceScene(TransitionFade::create(1.0f, StartMenu::createScene()));

    }
}

void BHFIGHT::setupBackground()
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
    float rivalLifeRatio = (float)rivalHealth / MAX_HEALTH;

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
    }, reactionTime, "ReactionTimer");

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
void BHFIGHT::processAI(int playerAction, cocos2d::Sprite* rivalSprite, cocos2d::Action* protectAction, cocos2d::Action* BH_GPIAnimate, cocos2d::Action* BH_GPDAnimate, cocos2d::Action* BH_GPIBAnimate, cocos2d::Action* BH_GPDBAnimate, cocos2d::Action* BH_EsquivaIzq, cocos2d::Action* BH_EsquivaDer, cocos2d::Action* BH_GUAnimate, cocos2d::Action* BH_Burla, bool& isDefending) {

    int randomValue = random(0,100);

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

    auto LM_GAMD1= ResourceManager::getInstance()->getSpriteFrame("LM_S1");
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
    auto protectAnimacion = Animation::createWithSpriteFrames(protectFrames, 0.8f);
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
    auto BH_BurAnimacion = Animation::createWithSpriteFrames(BH_BurFrames, 1.0f);
    auto BH_Bur = Animate::create(BH_BurAnimacion);
    BH_Bur->retain();

    Vector<SpriteFrame*> BH_BurlaFrames = { BH_Risa, BH_Lengua, BH_PI };
    auto BH_BurlaAnimacion = Animation::createWithSpriteFrames(BH_BurlaFrames, 1.0f);
    auto BH_Burla = Animate::create(BH_BurlaAnimacion);
    BH_Burla->retain();

    Vector<SpriteFrame*> BH_HookFrames = { BH_JabIzq1, BH_JabIzq2, BH_PI };
    auto BH_HookAnimacion = Animation::createWithSpriteFrames(BH_HookFrames, 2.0f);
    auto BH_Hook = Animate::create(BH_HookAnimacion);
    BH_Hook->retain();

    Vector<SpriteFrame*> BH_DobFrames = { BH_GD1, BH_GD2, BH_PI };
    auto BH_DobAnimacion = Animation::createWithSpriteFrames(BH_DobFrames, 2.0f);
    auto BH_Double = Animate::create(BH_DobAnimacion);
    BH_Double->retain();


    Vector<SpriteFrame*> GoDownFrames = { LM_GR };
    auto GoDownAnimacion = Animation::createWithSpriteFrames(GoDownFrames, 1.0f);
    auto GoDownAnimate = Animate::create(GoDownAnimacion);
    GoDownAnimate->retain();

    Vector<SpriteFrame*> BloqFrames = { LM_GR };
    auto BloqAnimacion = Animation::createWithSpriteFrames(BloqFrames, 1.0f);
    auto BloqAnimate = Animate::create(BloqAnimacion);
    BloqAnimate->retain();

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
        if (playerFatigue < fatigueThreshold){
            CCLOG("¡Demasiado cansado para atacar!");
            return;
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_Q) { // Jab Izquierdo
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_E) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_D) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_A) { // Jab Derecho
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_W) { // Upper Cut
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_Z) // Bloquear
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
        else if (keyCode == EventKeyboard::KeyCode::KEY_C) // Bloquear
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
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

    return true;
};

void BHFIGHT::menuCloseCallback(Ref* pSender)
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
            auto newScene = BHFIGHT::createScene();
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