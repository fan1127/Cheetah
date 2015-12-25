//
//  NotEnoughSpaceDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/3/2557 BE.
//
//

#ifndef __Cheetah__NotEnoughSpaceDialog__
#define __Cheetah__NotEnoughSpaceDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Settings.h"
#include "SimpleDialog.h"

#define kNESDButtonMax  4

class NotEnoughSpaceDialog : public SimpleDialog
{
    void *                  m_entity;
    cocos2d::CCMenu*        m_menu[kNESDButtonMax];
    
    void setToAvailablePositionAndZoom();
    
public:
    
    static NotEnoughSpaceDialog *create(CCModalController *parentModalController_, void *entity_);

    bool init(CCModalController *parentModalController_, void *entity_, CCSize size);
    
    bool compose();

    virtual void addButton(int index_, const char* label_, const char *value_, const char *icon_filename_, float x_pos_);
    
    virtual void onButton(cocos2d::CCObject *sender_);
    
};

#endif /* defined(__Cheetah__NotEnoughSpaceDialog__) */
