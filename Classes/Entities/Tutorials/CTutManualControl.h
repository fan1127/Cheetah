//
//  CTutManualControl.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutManualControl__
#define __Cheetah__CTutManualControl__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CTut.h"
#include "../../UIs/RacePanel.h"

#include "../../Settings.h"

class CTutManualControl: public CTut {
    
public:
    
    /** Required instances, in order to perform the tutorial, these instances are required */
    
    bool                setToInactive;
    bool                m_isRunningUpdate;
    
    CTutManualControl() {
        
        CTut();
        id = CT_ID_MANUAL_CONTROL;
        
        setToInactive = false;
        m_isRunningUpdate = false;
    }
    
    virtual ~CTutManualControl();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void setToInActiveNow();
    bool isRunningUpdate();
    
};
#endif /* defined(__Cheetah__CTutManualControl__) */
