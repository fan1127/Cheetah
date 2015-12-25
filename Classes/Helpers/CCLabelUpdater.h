//
//  CCLabelUpdater.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/12/2557 BE.
//
//

#ifndef __Cheetah__CCLabelUpdater__
#define __Cheetah__CCLabelUpdater__

#include "cocos2d.h"
#include "../Entities/Building.h"
#include "../Entities/Pet.h"
#include "../Entities/Ability.h"

class CCLabelUpdater : public cocos2d::CCNode {
    
    cocos2d::CCLabelBMFont* label;
    
public:
    
    int labelValue;
    
    Building *building;
    Pet *pet;
    Ability *ability;
    
    int (*updateIntFromBuilding)(Building *building);
    int (*updateIntFromPet)(Pet *pet);
    int (*updateIntFromAbility)(Ability *ability);
    
    cocos2d::CCObject *callbackSender;
    void (*callbackOnceWhenZero)(cocos2d::CCObject* sender_);
    
    static CCLabelUpdater* createWithLabel(cocos2d::CCLabelBMFont *label_, Building *building_ ,int (*updateIntFromBuilding)(Building *building));
    static CCLabelUpdater* createWithLabel(cocos2d::CCLabelBMFont *label_, Pet *pet_ ,int (*updateIntFromPet)(Pet *pet));
    static CCLabelUpdater* createWithLabel(cocos2d::CCLabelBMFont *label_, Ability *ability_ ,int (*updateIntFromAbility)(Ability *ability));
    
    void addLabel(cocos2d::CCLabelBMFont* label_);
    cocos2d::CCLabelBMFont* getLabel() { return label; };
    
    CREATE_FUNC(CCLabelUpdater);
    virtual void update(float delta_);
    void updateValue();
    
    
};

#endif /* defined(__Cheetah__CCLabelUpdater__) */
