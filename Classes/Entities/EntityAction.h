//
//  EntityAction.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/6/2557 BE.
//
//

#ifndef __Cheetah__EntityAction__
#define __Cheetah__EntityAction__

#include "cocos2d.h"
#include "../Extensions/CCLabelHelper.h"

#include "../UIs/Elements/ProgressBar.h"
#include "EntityHabitatAction.h"
#include "EntityTypes.h"

#include "../UIs/Dialogs/InstantDialog.h"
#include "../UIs/Dialogs/AnimalBarnDialog.h"
#include "../UIs/Dialogs/MiniUpgradeDialog.h"

enum EntityActionButton {
    EAB_INFO,
    EAB_UPGRADE,
    EAB_VIEW,
    EAB_COLLECT,
    EAB_CUT,
    EAB_KEEP,
    EAB_INSTANT_BUILD,
    EAB_INSTANT_UPGRADE,
    EAB_INSTANT_ENERGY,
    EAB_INSTANT_ABILITY,
    EAB_SEND_TO_BARN,
    EAB_SET_AS_HOST,
};

enum EntityActionState {
    
    EAS_STANDBY,
    EAS_SHOW_MENU,
    
};

#define kEntityActionProgressBarMax     2
#define kEntityActionTotalButton        3
#define kEntityActionTransitionSpeed    0.1f
#define kEntityActionTransitionMenuSpeed    0.7f
#define kEntityActionTransitionMenuDelay    0.045f

#define kEntityLabelFontSize kFontSizeVeryLarge-5.0f

#define kOffsetForBigButtons ccp(-20.0f*retinaMultiplier, 0.0f)

class EntityAction : public cocos2d::CCNode {

    EntityType          m_entity_type;
    EntityActionState   m_state;
    
    void*               m_entity;
    void*               m_entity_next;
    
    CCLabelBMFont*      m_entityLabel;
    ProgressBar*        m_progress_bar_new;
    ProgressBar*        m_progress_bar[kEntityActionProgressBarMax];
    cocos2d::CCLayer*   m_compLayer;
    
    cocos2d::CCAction*  m_callAttention[2];
    
    std::string         m_entityTypeCanBeTouched; //only this building can be touched
    std::string         m_entityTypeCanNOTBeTouched; //only this building, cannot be touched
    std::string         m_entityTypeCanBeOpenned;
    bool                m_feedTutorialRunning;
    
    void _clear();
    void _restart();
    
public:
    
    cocos2d::CCLayer*   m_menuLayer;
    EntityHabitatAction *habitatAction;
    
    InstantDialog       *_instantUpgradeDialog;
    AnimalBarnDialog    *_animalBarnDialog;

    MiniUpgradeDialog*  m_upgradeDialog;

    int                 viewMode;
    
    virtual bool init();
    CREATE_FUNC(EntityAction);
    
    virtual void update(float delta_);
    void setToAppropriatePosition();
    
    void *getEntity();
    EntityActionState getState() { return m_state; }
    
    float getScrollViewScale();
    float getEntityActionScale();
    
    void showProgressBarIfConditionMeets(void *building_);
    void hideProgressBarIfConditionMeets(void *building_);
    
    bool start(void* entity_);
    bool start(void* entity_, bool only_feed_action_);
    void finish();
    void finish(void *entity_next_);
    void finish(void *entity_next_, bool also_finish_sub_action_);
    
    void addProgressBar(int index_, void* entity_);
    void addButton(int index_, EntityActionButton button_);
    void setEntityTypeCanBeTouched(std::string type_) { m_entityTypeCanBeTouched = type_; }
    void setEntityTypeCanNOTBeTouched(std::string type_) { m_entityTypeCanNOTBeTouched = type_; }
    void setEntityTypeCanBeOpenned(std::string type_) { m_entityTypeCanBeOpenned = type_; }
    void setFeedTutorialRunning(bool isRunning){ m_feedTutorialRunning = isRunning; }
    void onInfo(cocos2d::CCObject *sender_);
    void onUpgrade(cocos2d::CCObject *sender_);
    void onView(cocos2d::CCObject *sender_);
    void onInstant(cocos2d::CCObject *sender_);
    void onCollect(cocos2d::CCObject *sender_);
    void onCut(cocos2d::CCObject *sender_);
    void onKeep(cocos2d::CCObject *sender_);
    void onSendToBarn(CCObject *sender_);
    static void onSendToBarnCompleted(void* sender, std::string resp_str);
    void onSetAsHost(CCObject *sender_);
    
    static int getGemCost(Building *_building);
    static int getGemCost(Pet *_pet);
    static int getGemCost(Ability *_ability);
    
};

#endif /* defined(__Cheetah__EntityAction__) */
