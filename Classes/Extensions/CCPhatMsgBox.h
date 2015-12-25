//
//  CCPhatMsgBox.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/21/56 BE.
//
//

#ifndef __Cheetah__CCPhatMsgBox__
#define __Cheetah__CCPhatMsgBox__

#include "CCPhatButton.h"
#include "../Settings.h"

class CCPhatMsgBox: public cocos2d::CCNode {
    
    cocos2d::CCSprite*      m_boxSprite     = NULL;
    
public:
    
    CCPhatButton*           btnOK           = NULL;
    CCPhatButton*           btnCancel       = NULL;
    CCPhatButton*           btnClose        = NULL;
    
    cocos2d::CCLabelTTF*    title           = NULL;
    cocos2d::CCLabelTTF*    msg             = NULL;
    
    static CCPhatMsgBox* createWithAllFrameNames(const char* bodyFrameName_, const char* btnOkFrameName_, const char* btnCancelFrameName_, const char* btnCloseFrameName_, std::string title_, std::string msg_);
    
    bool initWithAllFrameNames(const char* bodyFrameName_, const char* btnOkFrameName_, const char* btnCancelFrameName_, const char* btnCloseFrameName_, std::string title_, std::string msg_);
    CREATE_FUNC(CCPhatMsgBox);
    
    void open(std::string title_, std::string msg_);
    void open(std::string title_, std::string msg_, void* sender);
    void close();
    
    

};

#endif /* defined(__Cheetah__CCPhatMsgBox__) */
