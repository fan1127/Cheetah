//
//  AnimalBarnDialog.h
//  Cheetah
//
//  Created by Kaweerut on 2/27/57 BE.
//
//

#ifndef __Cheetah__AnimalBarnDialog__
#define __Cheetah__AnimalBarnDialog__

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCProgressBar9Sprite.h"

#include "../../Settings.h"

#include "../../Entities/Pet.h"
#include "../../Entities/PlayerState.h"

#include "../../UIs/Dialogs/AnimalBarnItem.h"
#include "../../UIs/Dialogs/AnimalDialog.h"
#include "ScrollDialog.h"

class CCNikScrollView;
class CCClippingLayer;
class AnimalMarketItem;

class AnimalBarnDialog : public ScrollDialog
{
    
#ifdef kDialogFooterTextEnabled
    CCLabelBMFont*          m_label_footer;
#endif
    CCLabelBMFont*          m_label_footer_right;
    
    PlayerState*            m_playerState;
    
    bool                    m_selectButtonEnabled;
    
    float                   m_openAnimalDialogDelay;
    
public:
    
    AnimalDialog            *_animalDetailDialog;
    AnimalBarnItem          *_lastItem;

    static AnimalBarnDialog *create(CCModalController *parentModalController_, PlayerState* playerState_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);

    virtual void update(float delta_);

    virtual void updateContent();
    
    void setSelectButtonEnabled(bool enabled_) { m_selectButtonEnabled = enabled_; }
    
    void selectAnimalItem(AnimalBarnItem *item);
    
    virtual void onClose(cocos2d::CCObject *sender_);

    void onNote(cocos2d::CCObject* obj_);
};

#endif /* defined(__Cheetah__AnimalBarnDialog__) */
