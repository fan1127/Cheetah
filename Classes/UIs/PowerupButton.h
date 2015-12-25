//
//  PowerupButton.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/19/2557 BE.
//
//

#ifndef __Cheetah__PowerupButton__
#define __Cheetah__PowerupButton__

#include "../Extensions/CCNikButton.h"
#include "../Settings.h"

//class CCLabelBMFont;

class PowerupButton: public CCNikButton {
    
    int m_cost;
    
    bool m_bEnabled; //not the same as the one in CCNikButton
    
    bool m_forceEnabled;
    bool m_forceEnabledStatus;
    
    SKILL m_skill;
    
    CCLabelBMFont *m_costLabel;
    CCSprite      *m_energyIcon;

    CCSprite*      m_bgNormal;
    CCSprite*      m_bgDisabled;
    
//    CCSpriteFrame *m_enableSpriteFrame;
//    CCSpriteFrame *m_disableSpriteFrame;
    
public:

    static PowerupButton *create(CCSprite* background, CCSprite* disabledBg, CCSprite *normalSprite, CCSprite *disabledSprite, int startingCost, SKILL skill);
    bool init(CCSprite* background, CCSprite* disabledBg, CCSprite *normalSprite, CCSprite *disabledSprite, int startingCost, SKILL skill);
    
    virtual void touchDown();
    virtual void touchUpInside();
    virtual void touchUpOutSide();
    
    void setCostVisible(bool costVisible);
    void forceEnabled(bool forceEnable);
    void setForceEnabledStatus(bool turnOnForceEnable);
    
    virtual void setEnabled(bool enable);
    virtual void setState();
    
    void updateCost(int cost);
    
    SKILL skill() { return m_skill; };
    
    int getCost() { return m_cost; };
    void checkCost(int cost);
//    void resetTimeUsed();
};

#endif /* defined(__Cheetah__PowerupButton__) */
