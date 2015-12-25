//
//  AnimalTradeDialog.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#ifndef __Cheetah__AnimalTradeDialog__
#define __Cheetah__AnimalTradeDialog__

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCProgressBar9Sprite.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Settings.h"
#include "../../Entities/Pet.h"
#include "../../Entities/PlayerState.h"

#define kAnimalIsAtMaxLevel (pet->ppVo.level >= pet->pVo.max_level)
#define kNextEvoIsUnknown (pet->pVo.next_evolution.size() > 1)

#define mult1 0.7f
#define mult2 0.5f
#define FIRST_STAGE_DURATION 2.88f
#define FIRST_STAGE_DURATION_2 2.66f
#define SECOND_STAGE_DURATION 0.233f
#define SECOND_STAGE_DURATION_2 0.2f
#define SPIN_DURATION 20.0f
#define SPIN_ANGLE 360.0f

#define ITEMS_SCALE 2.8f
#define ITEMS_SCALE_TO_2 1.5f
#define ITEMS_SCALE_TO_3 0.0f

#define BLOCK_TOUCH_SPRITE_TAG 99
#define DOT_BG_TAG 11
#define ITEMS_GROUP_TAG 22
#define FUSE_BATCHNODE_TAG 33
#define PET_SPINE_TAG 44
#define TARGET_PET_SPINE_TAG 55

#define TIME_UNTIL_RANDOM_LIGHT_PARTICLE 0.02f
#define PARTICLE_SCALE 0.35f

class AnimalInfoPopoverDialog;
class CCNikScrollView;
class CCClippingLayer;
class CCNikButton;
class ItemStoragePopoverDialog;
class AnimalTradeItem;

class AnimalTradeDialog : public ScrollDialog
{
    CCNode                  *dialogGroup;
    
    ItemStoragePopoverDialog*   m_popOver;
    CCNikButton *_infoButton;
    CCNikButton *_infoMyAnimalButton;
    AnimalInfoPopoverDialog*     m_animalPopOver;
    cocos2d::CCSprite*      m_infoBgSprite;

    cocos2d::CCMenu*        m_noteButton;
    
    cocos2d::CCLabelBMFont* m_label_coin_value;
    bool                    m_complete;
    bool                    m_not_enough_items;
    bool                    m_not_enough_animal_level;
    bool                    m_not_enough_coins;
    bool                    m_not_enough_market_level;
    bool                    m_not_enough_space_to_fuse;
    cocos2d::CCString topSpeed ;
    cocos2d::CCString connerSpeed ;
    cocos2d::CCString jump ;

    int                     m_unlocks_at_level;
    
    float                   m_fuseAnimationTime;
    bool                    m_starsExploded;
    float                   m_timeUntilUpdateStarts;
    float                   m_timeUntilRandomLightParticle;
    
    PlayerState*            m_playerState;
    
    bool                    m_closeButtonEnabled;
    bool                    m_selectButtonEnabled;
    std::string             m_targetPetTitle;
    
public:
    
    bool                    m_tradeEffectRunning;
    
    std::string             m_resp_json_str_;
    
    cocos2d::CCMenu*        menuTrade;
    
    GDPetVO                 vo;
    Pet*                    pet;
    
    cocos2d::CCMenu         *m_menuInfo      = NULL;
    static AnimalTradeDialog* create(CCModalController *parentModalController_, GDPetVO vo_, Pet *pet_, PlayerState* playerState_);
    
    bool init(CCModalController* parentModalController_, GDPetVO vo_, Pet *pet_, PlayerState* playerState_, CCSize size);
    
    virtual void updateContent();
    void selectedItemUpgrade(AnimalTradeItem *item);
    void onButton(cocos2d::CCObject *sender_);
    
    void setSelectButtonEnabled(bool enabled_) { m_selectButtonEnabled = enabled_; }
    
    void runTrading(CCObject *sender_, bool instant);
    
    void animateTrade(CCObject *sender, GDPetVO petVo_, GDPetVO vo_);
    void clearTradeAnimation(CCObject *sender);
    void clearTradeAnimation2();
    void clearTradeAnimation3();
    
    void playerFuseEvolveSound(CCObject *sender);
    void playerFuseEvolveSound2(CCObject *sender);
    
    void onSearch(CCObject *sender);
    
    float randNum();
    float randNum(float min, float max);
    float randNum(double randNum, float min, float max);
    
    virtual void update(float delta_);
    
    void runPetFused(std::string resp_json_str_);
    void runPetEvolved(std::string resp_json_str_);
    
    static void onPetFused(void* sender_, std::string resp_json_str_);
    static void onPetEvolved(void* sender_, std::string resp_json_str_);
    
    static void onHitBG(void* inst_, void* sender_);
    
    void onNote(cocos2d::CCObject* obj_);
    
};

#endif /* defined(__Cheetah__AnimalTradeDialog__) */
