//
//  MainScene.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__MainScene__
#define __Cheetah__MainScene__

#include "cocos2d.h"

enum SceneList {
    SCENE_STARTUP,
    SCENE_MAP,
    SCENE_RACE,
};

class MainScene: public cocos2d::CCScene {
    
public:
    
    static CCScene *scene();
    
    int sceneSelector;
    int sceneSelected;
    
    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(MainScene);
    
};

#endif /* defined(__Cheetah__MainScene__) */
