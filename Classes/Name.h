#include "cocos2d.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class Name : public Scene {
public:
    static Scene* createScene() {
        return Name::create();
    }

    virtual bool init();
    CREATE_FUNC(Name);
};
