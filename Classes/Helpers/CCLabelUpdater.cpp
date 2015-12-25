//
//  CCLabelUpdater.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/12/2557 BE.
//
//

#include "CCLabelUpdater.h"
USING_NS_CC;


CCLabelUpdater* CCLabelUpdater::createWithLabel(cocos2d::CCLabelBMFont *label_, Building *building_ ,int (*updateIntFromBuilding)(Building *building))
{
    CCLabelUpdater* labelUpdater = CCLabelUpdater::create();
    
    labelUpdater->addLabel(label_);
    labelUpdater->labelValue = 0;
    
    labelUpdater->building = building_;
    labelUpdater->pet = NULL;
    labelUpdater->ability = NULL;
    
    labelUpdater->updateIntFromBuilding = updateIntFromBuilding;
    labelUpdater->updateIntFromPet = NULL;
    labelUpdater->updateIntFromAbility = NULL;
    
    labelUpdater->callbackSender = NULL;
    labelUpdater->callbackOnceWhenZero = NULL;
    
    labelUpdater->updateValue();
    labelUpdater->scheduleUpdate();
    
    return labelUpdater;
}

CCLabelUpdater* CCLabelUpdater::createWithLabel(cocos2d::CCLabelBMFont *label_, Pet *pet_ ,int (*updateIntFromPet)(Pet *pet))
{
    CCLabelUpdater* labelUpdater = CCLabelUpdater::create();
    
    labelUpdater->addLabel(label_);
    labelUpdater->labelValue = 0;
    
    labelUpdater->building = NULL;
    labelUpdater->pet = pet_;
    labelUpdater->ability = NULL;
    
    labelUpdater->updateIntFromBuilding = NULL;
    labelUpdater->updateIntFromPet = updateIntFromPet;
    labelUpdater->updateIntFromAbility = NULL;
    
    labelUpdater->callbackSender = NULL;
    labelUpdater->callbackOnceWhenZero = NULL;
    
    labelUpdater->updateValue();
    labelUpdater->scheduleUpdate();
    
    return labelUpdater;
}

CCLabelUpdater* CCLabelUpdater::createWithLabel(cocos2d::CCLabelBMFont *label_, Ability *ability_ ,int (*updateIntFromAbility)(Ability *ability))
{
    CCLabelUpdater* labelUpdater = CCLabelUpdater::create();
    
    labelUpdater->addLabel(label_);
    labelUpdater->labelValue = 0;
    
    labelUpdater->building = NULL;
    labelUpdater->pet = NULL;
    labelUpdater->ability = ability_;
    
    labelUpdater->updateIntFromBuilding = NULL;
    labelUpdater->updateIntFromPet = NULL;
    labelUpdater->updateIntFromAbility = updateIntFromAbility;
    
    labelUpdater->callbackSender = NULL;
    labelUpdater->callbackOnceWhenZero = NULL;
    
    labelUpdater->updateValue();
    labelUpdater->scheduleUpdate();
    
    return labelUpdater;
}

void CCLabelUpdater::addLabel(cocos2d::CCLabelBMFont* label_) {
    
    label = label_;
    this->addChild(label);
}

void CCLabelUpdater::update(float delta_) {
    
    this->updateValue();
    
}

void CCLabelUpdater::updateValue()
{
    
    if (this->label == NULL) return;
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    if (updateIntFromBuilding) {
        labelValue = updateIntFromBuilding(building);
    }
    
    if (updateIntFromPet) {
        labelValue = updateIntFromPet(pet);
    }
    
    if (updateIntFromAbility) {
        labelValue = updateIntFromAbility(ability);
    }
    
    snprintf(_buffer, _buffer_size, "%d", labelValue);
    this->label->setString(_buffer);
    
    if (callbackOnceWhenZero) {
        if (labelValue <= 0) {
            if (callbackSender) {
                callbackOnceWhenZero(callbackSender);
                callbackOnceWhenZero = NULL;
            }
        }
    }
    
}
