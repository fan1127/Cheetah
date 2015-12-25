//
//  CCClippingLayer.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/21/2556 BE.
//
//

#ifndef __Cheetah__CCClippingLayer__
#define __Cheetah__CCClippingLayer__

#include "cocos2d.h"

class CCClippingLayer: public cocos2d::CCLayer {
    
public:
    
    CCNode          *parent = NULL;
    bool            debug = false;
    
    void            *preferenceRootParent = NULL;
    
    virtual void visit();
    CREATE_FUNC(CCClippingLayer);
    
};


#endif /* defined(__Cheetah__CCClippingLayer__) */
