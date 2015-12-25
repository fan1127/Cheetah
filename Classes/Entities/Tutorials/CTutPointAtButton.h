//
//  CTutPointAtButton.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutPointAtButton__
#define __Cheetah__CTutPointAtButton__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "CTut.h"

#include "../../Extensions/CCNikButton.h"

#define CTutPointAtButtonFailed     "failed"

class CTutPointAtButton: public CTut {
    
    bool                m_effectEnabled;
    bool                m_sensitiveEnabled;
    
    //retain sender & callback
    void                *m_sender;
    void                (*m_callback)(void*,std::string);
    
public:
    
    /*****************************************************
     Required instances, in order to perform the tutorial,
     these instances are required
     *****************************************************/
    cocos2d::CCMenu     *menuButton;
    cocos2d::CCMenuItem *menuItemButton;
    cocos2d::CCSprite*  pointer;
    cocos2d::CCMenuItem*    pointerGroup;
    cocos2d::CCMenuItem*    effectGroup;
    cocos2d::extension::CCSkeletonAnimation *effect;
    cocos2d::CCLabelBMFont* text;
    std::string         message;
    
    cocos2d::CCPoint    shift;
    float               angle;
private:
    
    void _default() {
        id = CT_ID_POINT_BUTTON;
        m_effectEnabled = true;
        m_sensitiveEnabled = false;
        
        menuButton = NULL;
        menuItemButton = NULL;
        
        pointer = NULL;
        pointerGroup = NULL;
        effect = NULL;
        effectGroup = NULL;
        text = NULL;
        message = "";
        
        shift = cocos2d::CCPointZero;
        angle = 0.0f;
        
        
        
        m_sender = NULL;
        m_callback = NULL;
        sender = NULL;
        callback = NULL;
    }
    
public:
    
    CTutPointAtButton() {
        
        CTut();
        _default();
        
    }
    
    CTutPointAtButton(cocos2d::CCMenu* menuButton,
                      void* sender,
                      void (*callback)(void* sender, std::string result)) {
        
        CTut();
        _default();
        
        this->menuButton = menuButton;
        
        m_sender = sender;
        m_callback = callback;
        
    }
    
    CTutPointAtButton(cocos2d::CCMenu* menuButton,
                      bool effectEnabled,
                      void* sender,
                      void (*callback)(void* sender, std::string result)) {
        
        CTut();
        _default();
        
        m_effectEnabled = effectEnabled;
        this->menuButton = menuButton;
        
        m_sender = sender;
        m_callback = callback;
        
    }
    
    CTutPointAtButton(cocos2d::CCMenu* menuButton,
                      bool effectEnabled,
                      cocos2d::CCPoint shift,
                      float angle,
                      void* sender, void (*callback)(void* sender, std::string result)) {
        
        CTut();
        _default();
        
        m_effectEnabled = effectEnabled;
        this->menuButton = menuButton;
        
        this->shift = shift;
        this->angle = angle;
        
        m_sender = sender;
        m_callback = callback;
        
    }
    
    CTutPointAtButton(cocos2d::CCMenu* menuButton,
                      cocos2d::CCMenuItem *menuItemButton,
                      bool effectEnabled,
                      cocos2d::CCPoint shift,
                      float angle,
                      void* sender,
                      void (*callback)(void* sender, std::string result)) {
        
        CTut();
        _default();
        
        m_effectEnabled = effectEnabled;
        
        this->menuButton = menuButton;
        this->menuItemButton = menuItemButton;
        
        this->shift = shift;
        this->angle = angle;
        
        m_sender = sender;
        m_callback = callback;
        
    }
    
    CTutPointAtButton(cocos2d::CCMenu* menuButton,
                      cocos2d::CCMenuItem *menuItemButton,
                      bool effectEnabled,
                      bool sensitiveEnabled,
                      cocos2d::CCPoint shift,
                      float angle,
                      void* sender,
                      void (*callback)(void* sender, std::string result)) {
        
        CTut();
        _default();
        
        m_effectEnabled = effectEnabled;
        m_sensitiveEnabled = sensitiveEnabled;
        
        this->menuButton = menuButton;
        this->menuItemButton = menuItemButton;
        
        this->shift = shift;
        this->angle = angle;
        
        m_sender = sender;
        m_callback = callback;
        
    }
    
    virtual ~CTutPointAtButton();
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
    void setEffectEnabled(bool enabled) { m_effectEnabled = enabled; }
    void clearGraphics();
    
    static void onButtonSelected(void* sender, bool select);
    static void onButtonWillActive(void* sender);
    static void onButtonWillReleased(void* sender);
    
};


#endif /* defined(__Cheetah__CTutPointAtButton__) */
