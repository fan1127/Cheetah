//
//  ProgressBar.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/26/2557 BE.
//
//

#ifndef __Cheetah__ProgressBar__
#define __Cheetah__ProgressBar__

#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCLabelHelper.h"

class ProgressBar : public cocos2d::CCNode {
    
    void*                   m_entity;

    cocos2d::CCSprite*      m_icon;
    CCProgressBar9Sprite*   m_bar;
    CCLabelBMFont*          m_label;
    
public:
    
    static ProgressBar* create(void *entity_);
    bool init(void *entity_);
    
    bool compose();
    void dispose();
    void update();
    void startUpdate();
    
};

#endif /* defined(__Cheetah__ProgressBar__) */
