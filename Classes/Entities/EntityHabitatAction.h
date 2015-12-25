//
//  EntityHabitatAction.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/12/2557 BE.
//
//

#ifndef __Cheetah__EntityHabitatAction__
#define __Cheetah__EntityHabitatAction__

#include "cocos2d.h"

#include "Pet.h"
#include "../Helpers/CCLabelUpdater.h"

#define kEntityActionTransitionSpeed    0.1f

#define DEFAULT_ICON_FOOD_POSITION ccp(m_raySprite->getPositionX()-m_iconFood->getContentSize().width*0.6f,m_raySprite->getPositionY())

#define ICON_FOOD_ON_TOUCH_OFFSET_BASE 0.5f
#define ICON_FOOD_ON_TOUCH_OFFSET ccp(0, m_iconFood->getContentSize().height*ICON_FOOD_ON_TOUCH_OFFSET_BASE)
#define HUNGER_BAR_ON_TOUCH_OFFSET ccp(0, m_iconFood->getContentSize().height * (1.1f+ICON_FOOD_ON_TOUCH_OFFSET_BASE))

#define ICON_FOOD_IDLE_ANIM_TIME 0.07f
#define ICON_FOOD_IDLE_ANGLE_DIFF (8)

#define ICON_FOOD_TOUCH_ANIM_TIME 0.1f

enum EntityHabitatActionState {
    
    EHAS_STANDBY,
    EHAS_SHOW_MENU,
    EHAS_GRASP_FOOD,
    EHAS_FED,
    EHAS_REVERT,
    
};

class EntityHabitatAction : public cocos2d::CCLayer {
    
//    void                *m_inst;
//    void                *m_sender;
    
    EntityHabitatActionState m_state;
    void                *m_entity;
//    void                *m_entity_next;
    
    cocos2d::CCSprite   *m_raySprite;
//    cocos2d::CCMenu     *m_menu;
    cocos2d::CCSprite   *m_iconFood;
    cocos2d::CCSprite   *m_arrowSprite;
//    cocos2d::CCAction   *m_callAttention;
    
    CCLabelUpdater      *m_foodAmountUpdater;
    
    bool isFeed;
    bool isFinishing;
    
    bool eatingAnimating;
    bool feedingAnimated;
    
    void                *m_tmp;
    
    void _clear();
    void _restart();
    
public:
    
    virtual bool init();
    CREATE_FUNC(EntityHabitatAction);
    
    void start(void* entity);
    void finish();
    
    virtual void update(float delta_);
    
    static int getHungerFoodToFullFromPet(Pet *_pet);
  
    void showArrowWithAnimation();
    void playFoodIconIdleAnimation();
    void runFoodIconIdleAnimation();
    
    void onFeed(cocos2d::CCObject *sender_);
    void cancelFeed(cocos2d::CCObject *sender_);
    
//    void setSender(void *sender);
//    void (*onReleaseMe)(void *inst_, void* sender_);
    
    virtual bool ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    virtual void ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent);
    
    void delayFeed();
    void eat();
    void endEat();
    
    cocos2d::CCPoint getDefaultIconFoodPosition();// { return DEFAULT_ICON_FOOD_POSITION; }
    EntityHabitatActionState getState() { return m_state; }
    
};

#endif /* defined(__Cheetah__EntityHabitatAction__) */
