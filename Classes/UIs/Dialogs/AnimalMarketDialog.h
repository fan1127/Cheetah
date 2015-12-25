//
//  AnimalMarketDialog.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#ifndef __Cheetah__AnimalMarketDialog__
#define __Cheetah__AnimalMarketDialog__

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Settings.h"

#include "../../Entities/Pet.h"
#include "../../Entities/PlayerState.h"
#include "ScrollDialog.h"

#define MAX_PETS_FOR_LOADING_SPINE 6

#define ANIMAL_ITEM_WIDTH (267.0f*sizeMultiplier)
#define ANIMAL_ITEM_HEIGHT (239.0f*sizeMultiplier)
//NOTE: cannot use below, because each _item content size varies
//#define ANIMAL_ITEM_WIDTH (_item->getContentSize().width)
//#define ANIMAL_ITEM_HEIGHT (_item->getContentSize().height)

class CCNikScrollView;
class CCClippingLayer;
class AnimalMarketItem;

class AnimalMarketDialog : public ScrollDialog
{

#ifdef kDialogFooterTextEnabled
    CCLabelBMFont*          m_label_footer;
#endif

    PlayerState*            m_playerState;
    
    bool                    m_selectButtonEnabled;
    float                   m_openAnimalDialogDelay;

public:

    static AnimalMarketDialog *create(CCModalController *parentModalController_, PlayerState* playerState_);

    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);
    
    virtual void update(float delta_);
    
    virtual void updateContent();

    void selectAnimalItem(AnimalMarketItem *item);
    
    virtual void onNote(cocos2d::CCObject* obj_);

    void setSelectButtonEnabled(bool enabled_) { m_selectButtonEnabled = enabled_; }
};

#endif /* defined(__Cheetah__AnimalMarketDialog__) */
