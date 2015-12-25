//
//  CollectableIcons.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/11/2556 BE.
//
//

#ifndef __Cheetah__CollectableItems__
#define __Cheetah__CollectableItems__

#include "cocos2d.h"
#include "../VOs/GDBuildingVO.h"

class CollectableItem: public cocos2d::CCNode {
    
    cocos2d::CCSprite   *m_itemSprite = NULL;
    
    cocos2d::CCPoint    m_initPosition;
    cocos2d::CCPoint    m_destPosition[2];
    
    float               m_initScale;
    float               m_destScale;
    
    float               m_movingSpeed[0];
    
    float               m_velocityX;
    float               m_velocityY;
    float               m_accelerateY;
    
public:
  
    int                 collectingAmount;
    
    static CollectableItem *createWithSpriteFrameName(const char *framename_);

    bool initWithSpriteFrameName(const char *framename_);
    CREATE_FUNC(CollectableItem);
    
    bool updateMoving(float delta_);
    void setDest(cocos2d::CCPoint dest_position_, float dest_scale_);
    
};

class CollectableItems: public cocos2d::CCLayer {
    
    void*                               m_inst;
    void*                               m_sender;
    
    std::vector<CollectableItem*>       m_items;
    cocos2d::CCPoint                    m_destPosition;
    float                               m_destScale;
    
    bool                                m_running;
    std::string                         m_icon_framename;
    
public:
    
    static CollectableItems* create(std::string icon_framename_);
    
    bool init(std::string icon_framename_);
    virtual void update(float delta_);
    
    void setDest(cocos2d::CCPoint dest_position_, float dest_scale_);
    bool add(cocos2d::CCPoint position_, float scale_, int collecting_amount_,GDBuildingVO bVo = GDBuildingVO());
    void collected(CCObject* object_);
    int getNItems() { return m_items.size(); }
    bool isRunning();
    
    void setSender(void* sender_);
    void (*onCollected)(void* inst_, void* sender_);
    
    void explosionParticle(CCNode *onNode);
    
};

#endif /* defined(__Cheetah__CollectableItems__) */
