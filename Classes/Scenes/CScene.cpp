//
//  CScene.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/27/2557 BE.
//
//

#include "CScene.h"
USING_NS_CC;

bool CScene::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    isoMap = NULL;
    hudPanel = NULL;
    mapToScreenUI = NULL;
    modalController = NULL;
    scrollView = NULL;
    
    return true;
    
}