//
//  CTutPointAtPowerupButtons.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutPointAtPowerupButtons__
#define __Cheetah__CTutPointAtPowerupButtons__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CTut.h"
#include "../../UIs/RacePanel.h"

#include "../../Settings.h"

class CTutPointAtPowerupButtons: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    RacePanel*          racePanel;
    SKILL               buttonSkill;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::CCNode*    pointerGroup;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    cocos2d::CCPoint    shift;
    float               angle;
    bool                enableEffect;
    
    CTutPointAtPowerupButtons(RacePanel* racePanel_, SKILL buttonSkill_, cocos2d::CCPoint shift_, float angle_, bool enableEffect_, void* sender_, void (*callback_)(void* sender, std::string result_))
    {
        CTut();
        id = CT_ID_POINT_POWERUPBUTTONS;
        
        racePanel = racePanel_;
        buttonSkill = buttonSkill_;
        
        shift = shift_;
        angle = angle_;
        enableEffect = enableEffect_;
        
        pointer = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        pointerGroup = NULL;
        
        sender = sender_;
        callback = callback_;
    }
    
    virtual ~CTutPointAtPowerupButtons();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void clearGraphics();
};

#endif /* defined(__Cheetah__CTutPointAtPowerupButtons__) */
