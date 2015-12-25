//
//  InstantDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/21/2557 BE.
//
//

#ifndef __Cheetah__InstantDialog__
#define __Cheetah__InstantDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCNikButton.h"
#include "../../VOs/GDBuildingVO.h"
#include "../../VOs/GDPetVO.h"
#include "../../Entities/Building.h"
#include "../../Entities/Pet.h"
#include "../../Entities/Ability.h"
#include "../../Helpers/CCLabelUpdater.h"
#include "SimpleDialog.h"

enum InstantType {
    IT_UNKNOWN,
    IT_INSTANT_UPGRADE,
    IT_INSTANT_ENERGY,
    IT_FINISH_BUILD,
    IT_FINISH_UPGRADE,
    IT_FINISH_ABILITY,
    IT_FINISHCURRENTBUILD_AND_UPGRADE,
    IT_FINISHCURRENTBUILD_AND_BUILD,
    IT_FINISHCURRENTBUILD_AND_BUILD_BUYRESOURCE,
    IT_FINISHCURRENTBUILD_AND_UPGRADE_BUYRESOURCE,
    IT_FINISHCURRENTUPGRADE_AND_UPGRADE,
    IT_FINISHCURRENTUPGRADE_AND_BUILD,
    IT_FINISHCURRENTUPGRADE_AND_BUILD_BUYRESOURCE,
    IT_FINISHCURRENTUPGRADE_AND_UPGRADE_BUYRESOURCE,
    IT_BUILD_BUYRESOURCE,
    IT_UPGRADE_BUYRESOURCE,
    IT_START_RACE,
    IT_UPGRADE_ABILITY_BUYRESOURCE,
    IT_CUTTREE_BUYRESOURCE,
    IT_FUSE_ANIMAL_BUYRESOURCE, //get new
    IT_EVOLVE_ANIMAL_BUYRESOURCE, // trade
    IT_CLOUD_BUYRESOURCE,
    IT_DECORATION_BUYRESOURCE,
    IT_BUYCOIN_WITHGEMS,
    IT_BUYFOOD_WITHGEMS,
    IT_BUYHABITAT_WITHGEMS,
    IT_ENTER_GEM_SHOP,
};

class InstantDialog : public SimpleDialog
{
    void*                   m_entity_optional;
    void*                   m_entity;
    
    GDBuildingVO            m_bvo;
    
    InstantType             m_type;
    int                     m_missing_resource_amount;
    int                     m_param_1;
    GDPetVO                 m_pet_vo;
    
public:
    CCMenuItemSprite    *_instantButton;
    CCMenu              *instantMenu;
    
    static InstantDialog* createWithType(CCModalController* parentModalController_, InstantType type_, void* entity_, void* optional_entity_);
    static InstantDialog* createWithBuildingVO(CCModalController* parentModalController_, GDBuildingVO bvo_);
    static InstantDialog* create(CCModalController* parentModalController_, void* entity_);
    static InstantDialog* create(CCModalController* parentModalController_, void* entity_, void* optional_entity_);
    static InstantDialog* create(CCModalController* parentModalController_, InstantType* type_, void* entity_);


    bool initWithBuildingVO(CCModalController* parentModalController_, GDBuildingVO bvo_, CCSize size);
    bool init(CCModalController* parentModalController_, void* entity_, void* optional_entity_, CCSize size);
    bool initWithType(CCModalController* parentModalController_, InstantType* type_, void* entity_, CCSize size);
    
    static InstantDialog* createWithResourceType(CCModalController* parentModalController_, RESOURCE_TYPE type_, int amount_, CCSize size);
    bool initWithResourceType(CCModalController* parentModalController_, RESOURCE_TYPE type_, int amount_, CCSize size);
    
    bool compose();

    void addInstantButton(Building *building_);
    void addInstantButton(std::string textShow);
    void addInstantButton(Pet *pet_);
    void addInstantButton(Ability *ability_);
    void addInstantButton(int value_);
    void addInstantButton(CCLabelUpdater* updater_);
    void launchGemsStore();
    void enterGemsStore();
    virtual void onButton(cocos2d::CCObject* sender_);
    
    static void updateCurrentResourceValues(void *sender, std::string resp_json);
    static void onDidInstant(void *sender, std::string resp_json);
    static void onDidInstantBuyCoinsOrFood(void *sender, std::string resp_json);
    
    static void closeAllDialogs(cocos2d::CCObject* sender);
    
};

#endif /* defined(__Cheetah__InstantDialog__) */
