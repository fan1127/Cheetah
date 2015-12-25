//
//  CTut.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTut__
#define __Cheetah__CTut__

#include <iostream>

enum CTutIds {
    CT_ID_NONE,
    CT_ID_SPEECH,
    CT_ID_FOCUS_BUILDING,
    CT_ID_POINT_BUILDING,
    CT_ID_POINT_BUTTON,
    CT_ID_POINT_VILLAGE,
    CT_ID_POINT_NIKBUTTON,
    CT_ID_POINT_POWERUPBUTTONS,
    CT_ID_DRAG_FOOD,
    CT_ID_DRAG_BUILDING,
    CT_ID_MANUAL_CONTROL
};

class CTut {
    
    void *m_onAfterPerform_sender;
    void (*m_onAfterPerform_callback)(void *sender);
    
protected:
    
    /** ID should be assigned automatically */
    CTutIds id;
    bool    active;
    void*   tutUnit;
    
public:
    
    /** Set callback when decision has been made! */
    void (*callback)(void* sender, std::string result);
    
    /** Set sender to send back with callback */
    void*           sender;
    
    CTut():
    id(CT_ID_NONE),
    active(true),
    tutUnit(NULL),
    callback(NULL),
    sender(NULL),
    m_onAfterPerform_callback(NULL),
    m_onAfterPerform_sender(NULL){}
    
    virtual ~CTut() {}
    virtual void perform(void* tutUnit) {}
    virtual void update(float delta) {}
    
    void setAfterPerformCallback(void *sender,
                                 void (*callback)(void*)) {
        m_onAfterPerform_sender = sender;
        m_onAfterPerform_callback = callback;
    }
    void unsetAfterPerformCallback() {
        setAfterPerformCallback(NULL, NULL);
    }
    void doAfterPerform() {
        if (!m_onAfterPerform_callback) return;
        m_onAfterPerform_callback(m_onAfterPerform_sender);
    }
    
};

#endif /* defined(__Cheetah__CTut__) */
