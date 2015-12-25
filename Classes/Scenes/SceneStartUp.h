//
//  SceneStartup.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/26/56 BE.
//
//

#ifndef __Cheetah__SceneStartup__
#define __Cheetah__SceneStartup__

#include "cocos2d.h"

class SceneStartup: public cocos2d::CCScene {
   
public:
    
    cocos2d::CCLayerColor*          layerColor;
    
    virtual bool init();
    CREATE_FUNC(SceneStartup);
    
};

#endif /* defined(__Cheetah__SceneStartup__) */
