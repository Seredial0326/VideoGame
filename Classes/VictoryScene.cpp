#include "VictoryScene.h"
#include "NK_Fight.h"
#include "BH_Fight.h"
#include "GJ_Fight.h"
#include "StartMenuScene.h"

USING_NS_CC;

Scene* VictoryScene::createScene() {
    auto scene = Scene::create();
    auto layer = VictoryScene::create();
    scene->addChild(layer);
    return scene;
}

bool VictoryScene::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Mensaje de Victoria
    auto label = Label::createWithTTF("¡Victoria!", "fonts/Marker Felt.ttf", 36);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 100));
    this->addChild(label);

    // Botón para volver al menú principal
    auto menuLabel = Label::createWithTTF("Menú Principal", "fonts/Marker Felt.ttf", 24);
    auto menuItem = MenuItemLabel::create(menuLabel, [](Ref* sender) {
        Director::getInstance()->replaceScene(StartMenu::createScene());
        });

    // Botón para jugar de nuevo
    auto playAgainLabel = Label::createWithTTF("Jugar de Nuevo", "fonts/Marker Felt.ttf", 24);
    auto playAgainItem = MenuItemLabel::create(playAgainLabel, [](Ref* sender) {
        Director::getInstance()->replaceScene(NKFIGHT::createScene());
        });

    // Menú
    auto menu = Menu::create(menuItem, playAgainItem, nullptr);
    menu->alignItemsVerticallyWithPadding(20.0f);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(menu);

    return true;
}
