#include "GameOverScene.h"
#include "NK_Fight.h"
#include "BH_Fight.h"
#include "GJ_Fight.h"
#include "StartMenuScene.h"

USING_NS_CC;

Scene* GameOverScene::createScene() {
    auto scene = Scene::create();
    auto layer = GameOverScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameOverScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del menú
    auto background = Sprite::create("BoxingRing3.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    auto TituloSprite = Sprite::create("Gametitle.png");
    TituloSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 375));
    this->addChild(TituloSprite);

    // Mensaje de Game Over
    auto label = Label::createWithTTF("Game Over", "fonts/Marker Felt.ttf", 50);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height/2));
    this->addChild(label);

    // Botón para reiniciar el juego
    auto restartLabel = Label::createWithTTF("Reintentar", "fonts/Marker Felt.ttf", 36);
    auto restartItem = MenuItemLabel::create(restartLabel, [](Ref* sender) {
        Director::getInstance()->replaceScene(NKFIGHT::createScene());
    });

    // Menú
    auto menu = Menu::create(restartItem, nullptr);
    menu->alignItemsVerticallyWithPadding(20.0f);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 150));
    this->addChild(menu);

    return true;
}
