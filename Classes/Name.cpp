#include "Name.h"
#include "NK_Fight.h"
#include "BH_Fight.h"
#include "GJ_Fight.h"
#include "StartMenuScene.h"

bool Name::init() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("BoxingRing2.png");
    if (background)
    {
        background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        background->setScale(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
        this->addChild(background, -1);
    }

    auto TituloSprite = Sprite::create("Gametitle.png");
    TituloSprite->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 375));
    this->addChild(TituloSprite);

    auto label = Label::createWithTTF("Introduce el nombre de tu personaje:", "fonts/Marker Felt.ttf", 24);
    label->setPosition(Vec2(Director::getInstance()->getVisibleSize() / 2) + Vec2(0, 100));
    this->addChild(label);

    auto textField = ui::TextField::create("Nombre", "fonts/Marker Felt.ttf", 24);
    textField->setMaxLength(15);
    textField->setMaxLengthEnabled(true);
    textField->setPosition(Director::getInstance()->getVisibleSize() / 2);
    textField->setTextColor(Color4B::WHITE);
    this->addChild(textField);

    auto confirmButton = ui::Button::create("PushStart.png", "PushStartSelected.png");
    confirmButton->setTitleFontSize(24);
    confirmButton->setPosition(Vec2(Director::getInstance()->getVisibleSize() / 2) - Vec2(0, 100));
    this->addChild(confirmButton);

    confirmButton->addClickEventListener([=](Ref* sender) {
        std::string characterName = textField->getString();
        if (!characterName.empty()) {
            CCLOG("Nombre seleccionado: %s", characterName.c_str());
            Director::getInstance()->replaceScene(TransitionFade::create(1.0f, GJ_Face::createScene()));
        }
        else {
            CCLOG("El nombre no puede estar vacío");
        }
        });

    return true;
}