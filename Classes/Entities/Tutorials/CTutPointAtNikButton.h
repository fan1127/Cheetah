//
//  CTutPointAtNikButton.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutPointAtNikButton__
#define __Cheetah__CTutPointAtNikButton__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CTut.h"
#include "../../Extensions/CCNikButton.h"

#define CTutPointAtNikButtonFailed "failed"

class CTutPointAtNikButton: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    CCNikButton*    button;
    
    cocos2d::CCSprite*  pointer;
    cocos2d::CCNode*    pointerGroup;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    cocos2d::CCPoint    shift;
    float               angle;
    bool                enableEffect;
    
    bool                autoInactive;
    
    CTutPointAtNikButton(CCNikButton* button_, bool enableEffect_, cocos2d::CCPoint shift_, float angle_, void* sender_, void (*callback_)(void* sender, std::string result_)) {
        
        CTut();
        id = CT_ID_POINT_NIKBUTTON;
        
        button = button_;
        shift = shift_;
        angle = angle_;
        enableEffect = enableEffect_;
        
        pointer = NULL;
        pointerGroup = NULL;
        effect = NULL;
        text = NULL;
        message = "";
        
        autoInactive = true;
        
        sender = sender_;
        callback = callback_;
        
    }
    
    virtual ~CTutPointAtNikButton();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void clearGraphics();
    
    static void onButtonWillActive(void* sender_);
    static void onButtonWillReleased(void* sender_);
    
};


#endif /* defined(__Cheetah__CTutPointAtNikButton__) */
