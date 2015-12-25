//
//  CScene.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/27/2557 BE.
//
//

#ifndef __Cheetah__CScene__
#define __Cheetah__CScene__

#include "cocos2d.h"

#include "../Entities/IsoMap.h"
#include "../Entities/MapToScreenUI.h"
#include "../Extensions/Modal/CCModalController.h"
#include "../UIs/Panels/HUDPanel.h"
#include "../Extensions/CCNikScrollView.h"

#define kModalControllerZOrder      100

class CScene: public cocos2d::CCScene {
    
public:
    
    IsoMap                  *isoMap;
    HUDPanel                *hudPanel;
    MapToScreenUI           *mapToScreenUI;
    CCModalController       *modalController;
    CCNikScrollView         *scrollView;
    
    CREATE_FUNC(CScene);
    virtual bool init();
    
    virtual void fadeOutThisSceneButWaitForReplay(int sceneIndex_, std::string race_id_) { };
    
};

#endif /* defined(__Cheetah__CScene__) */
