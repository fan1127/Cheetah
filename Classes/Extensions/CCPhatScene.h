//
//  CCPhatScene.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/56 BE.
//
//

#ifndef __Cheetah__CCPhatScene__
#define __Cheetah__CCPhatScene__

#include "cocos2d.h"

class CCPhatScene: public cocos2d::CCScene {
    
public:
    
    virtual bool init();
    CREATE_FUNC(CCPhatScene);
  
    bool saveSS();
    
};

#endif /* defined(__Cheetah__CCPhatScene__) */
