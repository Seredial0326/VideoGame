#include "AppDelegate.h"
#include "ResourceManager.h"
#include "StartMenuScene.h"
#include "GJ_Fight.h"
#include "NK_Fight.h"
#include "BH_Fight.h"
#include "GameOverScene.h"
#include "Name.h"

// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1920, 1080); // Nueva resolución
static cocos2d::Size smallResolutionSize = cocos2d::Size(960, 540);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1920, 1080);  // Asegurarse de que coincidan
static cocos2d::Size largeResolutionSize = cocos2d::Size(2560, 1440);  // Resolución aún mayor

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

void AppDelegate::initGLContextAttrs()
{
    // Configurar atributos del contexto OpenGL
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
    GLView::setGLContextAttrs(glContextAttrs);
}

static int register_all_packages()
{
    return 0;
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("PruebaGame", cocos2d::Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("PruebaGame");
#endif
        director->setOpenGLView(glview);
    }

    director->setDisplayStats(true);
    director->setAnimationInterval(1.0f / 60);

    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();

    if (frameSize.height > mediumResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
    }
    else if (frameSize.height > smallResolutionSize.height)
    {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
    }
    else
    {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
    }

    register_all_packages();

    ResourceManager::getInstance()->loadSpriteFrames();

    auto startScene = GJFIGHT::createScene();
    //auto startScene = NKFIGHT::createScene();
    //auto startScene = BHFIGHT::createScene();
    //auto startScene = StartMenu::createScene();
    //auto startScene = GameOverScene::createScene();
    //auto startScene = Name::createScene();
    director->runWithScene(startScene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
