//
//  MapToScreenUI.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#ifndef __Cheetah__MapToScreenUI__
#define __Cheetah__MapToScreenUI__

#include "cocos2d.h"

class MapToScreenUI : public cocos2d::CCLayer {
    
public:
    
    virtual bool init();
    CREATE_FUNC(MapToScreenUI);
    
    void closeAllUIs();
    
    static void onReleaseObject(void *inst_, void *sender_);
    
};

#endif /* defined(__Cheetah__MapToScreenUI__) */
