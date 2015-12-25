//
//  SceneStartup.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/26/56 BE.
//
//

#include "SceneStartup.h"
USING_NS_CC;

extern float scaleFactor;

bool SceneStartup::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
//    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    
    layerColor = CCLayerColor::create(ccc4(128, 128, 128, 255));
    this->addChild(layerColor);
    
//    label = CCLabelTTF::create("Now loading ...", "NewsflashBB", 128*scaleFactor);
//    label->setPosition(screenSize/2);
//    layerColor->addChild(label);
    
    return true;

}
