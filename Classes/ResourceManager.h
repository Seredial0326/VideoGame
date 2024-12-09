#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "cocos2d.h"
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    static ResourceManager* getInstance();
    void loadSpriteFrames();
    cocos2d::SpriteFrame* getSpriteFrame(const std::string& name);

private:
    ResourceManager() {}
    ~ResourceManager() {}
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::unordered_map<std::string, cocos2d::SpriteFrame*> spriteFrames;
};

#endif // RESOURCEMANAGER_H
