//
//  MessageNotification.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/18/56 BE.
//
//

#ifndef __Cheetah__MessageNotification__
#define __Cheetah__MessageNotification__

#include "../Settings.h"
#include "cocos2d.h"

#include "../Extensions/CCLabelHelper.h"

#define MSGN_NUM         5
#define MSGN_LIFE        6.0f

#define MSGN_LABEL_TAG   123

#define MSGN_FONT_SIZE 64.0f
#define MSGN_INFO_COLOR ccc3(255, 255, 255)
#define MSGN_WARNING_COLOR ccc3(252, 78, 9)
#define MSGN_ERROR_COLOR ccc3(252, 78, 9)

enum MessageNotificationType {
    MSGN_TYPE_INFO,
    MSGN_TYPE_WARNING,
    MSGN_TYPE_ERROR
};

struct MessageNotificationItem {
    
    MessageNotificationType type;
    std::string msg;
    float life;
    
    MessageNotificationItem();
    ~MessageNotificationItem();
    
};

class MessageNotification: public cocos2d::CCLayer {
  
    cocos2d::CCPoint        m_messagePosition;
    cocos2d::CCNode*        m_messageNode[MSGN_NUM];
    float                   m_originalLabelScale;
    
public:
    
    std::vector<MessageNotificationItem*> messages;
    
    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(MessageNotification);
    
    cocos2d::CCLabelBMFont* getLabel(cocos2d::CCNode* messageNode);
    void setMessagePosition(cocos2d::CCPoint position);
    void addMessage(MessageNotificationType type_, std::string msg_);
    void addMessage(MessageNotificationType type_, std::string msg_, bool force_);
    //void setFontSize(float fontSize);
};

#endif /* defined(__Cheetah__MessageNotification__) */
