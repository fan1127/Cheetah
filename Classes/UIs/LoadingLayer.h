//
//  LoadingLayer.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/8/2557 BE.
//
//

#ifndef __Cheetah__LoadingLayer__
#define __Cheetah__LoadingLayer__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>

USING_NS_CC;

class ModalDialog;
class LoadingLayer : public CCLayerColor {
    
    float m_velocity;
    float m_duration;
    float m_currentTime;
    
    cocos2d::extension::CCSkeletonAnimation *petSpine;
    cocos2d::CCLabelBMFont *m_loadingLabel;
    int animateIndex;
    
public:
    static LoadingLayer* create();
    virtual bool init();
    
    void animateLoading();
    
    void show();
    void hide();

    void showInDialog(ModalDialog*);
    void hideInDialog();
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);

};

#endif /* defined(__Cheetah__LoadingLayer__) */
