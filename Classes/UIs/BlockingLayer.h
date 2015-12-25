//
//  BlockingLayer.h
//  Cheetah
//
//  Created by Vaclav Samec on 11/28/2557 BE.
//
//

#ifndef __Cheetah__BlockingLayer__
#define __Cheetah__BlockingLayer__

#include "cocos2d.h"

USING_NS_CC;

class BlockingLayer : public CCLayerColor
{
public:
    static BlockingLayer* create();

    virtual bool init();

    void show();
    void hide();
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
};


#endif /* defined(__Cheetah__BlockingLayer__) */
