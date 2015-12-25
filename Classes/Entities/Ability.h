//
//  Ability.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/7/2557 BE.
//
//

#ifndef __Cheetah__Ability__
#define __Cheetah__Ability__

#include "cocos2d.h"

#include "../VOs/GDAbilityVO.h"
#include "../VOs/PlayerAbilityVO.h"

class Ability : public cocos2d::CCNode {
    
    int                 m_seconds_left_to_finish;
    bool                m_active;
    
public:
    
    GDAbilityVO         aVo;
    PlayerAbilityVO     paVo;
    
    virtual bool init();
    CREATE_FUNC(Ability);
    virtual void update(float delta_);
    
    void setInitial();
    void setActive(bool);
    int getSecondsLeftToFinish();
    float getCurrentProgressRatio();
    
    static void onUpdated(void *sender_, std::string resp_json_str_);
    
};

#endif /* defined(__Cheetah__Ability__) */
