//
//  CCPhatScene.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/56 BE.
//
//

#include "CCPhatScene.h"
USING_NS_CC;

bool CCPhatScene::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    return true;
    
}

bool CCPhatScene::saveSS() {
    
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRenderTexture* texture = CCRenderTexture::create((int)screenSize.width, (int)screenSize.height);
    texture->setPosition(screenSize/2);
    texture->begin();
    CCDirector::sharedDirector()->getRunningScene()->visit();
    texture->end();
    //texture->saveToFile("ss.png", kCCImageFormatPNG);

    return true;
    
}