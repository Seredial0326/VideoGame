#include "HelloWorldScene.h"
#include "ResourceManager.h"
#include "StartMenuScene.h"
#include <random>

USING_NS_CC;

bool StartMenu::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Fondo del men�
    auto background = Sprite::create("fondomenu.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    // Bot�n "Jugar"
    auto playItem = MenuItemImage::create(
        "PushStart.png",       // Imagen normal
        "PushStartSelected.png", // Imagen seleccionada
        [](Ref* sender) {       // Callback al presionar el bot�n
            auto scene = HelloWorld::createScene();
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene, Color3B::WHITE));
        });

    playItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));

    // Bot�n "Salir"
    auto quitItem = MenuItemImage::create(
        "Quit.png",
        "QuitSelected.png",
        [](Ref* sender) {
            Director::getInstance()->end();
        });

    quitItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));

    // Crear el men� y agregar los botones
    auto menu = Menu::create(playItem, quitItem, nullptr);
    menu->setPosition(Vec2::ZERO); // Permite colocar los botones en las posiciones definidas
    this->addChild(menu, 1);

    return true;

    return true;
};

void StartMenu::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}
