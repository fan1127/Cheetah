//
//  InstantRaceDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 3/31/2557 BE.
//
//

#ifndef __Cheetah__InstantRaceDialog__
#define __Cheetah__InstantRaceDialog__

#include "cocos2d.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Entities/PlayerState.h"
#include "SimpleDialog.h"

enum InstantRaceType {
    IT_INSTANT_COINS,
    IT_INSTANT_ENERGY_FOR_RACE,
    IT_INSTANT_NON,
    IT_INSTANT_ENERGY_AND_GOLD,
};

class InstantRaceDialog : public SimpleDialog
{
    int                     m_missing_resource_amount;
    InstantRaceType             m_type;
    
public:
    
    PlayerState*            mapPlayerState;
    
    static InstantRaceDialog*   create(CCModalController* parentModalController_, InstantRaceType type_, PlayerState* mapPlayerState_);
    bool init(CCModalController* parentModalController_,  PlayerState* mapPlayerState_, CCSize size);
    
    bool compose();
    void addInstantButton(int value_);
    void launchGemsStore();
    
    virtual void onButton(cocos2d::CCObject* sender);
};

#endif /* defined(__Cheetah__InstantRaceDialog__) */
