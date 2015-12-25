//
//  MessageNotification.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/18/56 BE.
//
//

#include "MessageNotification.h"
#include <algorithm>
USING_NS_CC;
using namespace std;

extern float scaleFactor;
extern float retinaMultiplier;

MessageNotificationItem::MessageNotificationItem() {
    
    type = MSGN_TYPE_INFO;
    msg = "";
    life = 0.0f;

}

MessageNotificationItem::~MessageNotificationItem() {
    
}

bool MessageNotification::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    for (int i=0; i<MSGN_NUM; i++)
    {
        m_messageNode[i] = CCNode::create();
        this->addChild(m_messageNode[i]);
        
        CCLabelBMFont *messageLabel = CCLabelHelper::createBMFont(retinaMultiplier, "", MSGN_FONT_SIZE, true, MSGN_INFO_COLOR);
        m_originalLabelScale = messageLabel->getScale();
        messageLabel->setTag(MSGN_LABEL_TAG);
        m_messageNode[i]->addChild(messageLabel);
    }
    
    setMessagePosition(ccp(_screenSize.width*0.5f, _screenSize.height*0.8f));
    
    /* scheduleUpdate();
     * should be called manually
     */
    
    return true;
    
}

void MessageNotification::update(float delta) {

    int i = 0;
    std::reverse(messages.begin(), messages.end());
    
    std::vector<int> _remove_list;
    _remove_list.clear();
    
    for (vector<MessageNotificationItem*>::iterator it=messages.begin(); it!=messages.end(); ++it)
    {
        if (MessageNotificationItem* _messageItem = (MessageNotificationItem*)(*it))
        {
            if (_messageItem->life > 0.0f)
            {
                if (_messageItem->life == MSGN_LIFE)
                {
                    getLabel(m_messageNode[i])->stopAllActions();
                    getLabel(m_messageNode[i])->
                    runAction(CCSequence::create(CCScaleTo::create(0.12f, m_originalLabelScale * 1.1f),
                                                 CCScaleTo::create(0.12f, m_originalLabelScale * 1.0f),
                                                 NULL));
                }
                
                _messageItem->life -= delta;
            }
            else
            {
                _remove_list.push_back(i);
            }
        }
        i++;
    }
    
    for (vector<int>::reverse_iterator it = _remove_list.rbegin(); it != _remove_list.rend(); ++it)
    {
        int _index = (int)(*it);
        if (_index > 0)
        {
            MessageNotificationItem* _item = messages[_index];
            delete _item;
            _item = NULL;
            messages.erase(messages.begin()+_index);
        }
    }
    
    
    i = 0;
    for (vector<MessageNotificationItem*>::iterator it=messages.begin(); it!=messages.end(); ++it)
    {
        if (i < MSGN_NUM)
        {
            if (MessageNotificationItem* _messageItem = (MessageNotificationItem*)(*it))
            {
                if (!IS_STRING_EQUAL(_messageItem->msg.c_str(), ""))
                {
                    if (!IS_STRING_EQUAL(getLabel(m_messageNode[i])->getString(), _messageItem->msg.c_str()))
                    {
                        getLabel(m_messageNode[i])->setString(_messageItem->msg.c_str());
                    }
                    getLabel(m_messageNode[i])->setVisible(true);
                    
                    switch (_messageItem->type)
                    {
                        default:
                        case MSGN_TYPE_INFO:
                            getLabel(m_messageNode[i])->setColor(MSGN_INFO_COLOR);
                            break;
                        case MSGN_TYPE_WARNING:
                            getLabel(m_messageNode[i])->setColor(MSGN_WARNING_COLOR);
                            break;
                        case MSGN_TYPE_ERROR:
                            getLabel(m_messageNode[i])->setColor(MSGN_ERROR_COLOR);
                            break;
                    }
                    
                    float _opacity = 255;
                    if (_messageItem->life < 1.0f)
                    {
                        _opacity = (_messageItem->life)*255;
                        
                        if (_opacity < 0.0f)
                        {
                            _opacity = 0.0f;
                        }
                    }
                    
                    getLabel(m_messageNode[i])->setOpacity(_opacity);
                }
            }
            i++;
        }
    }
    std::reverse(messages.begin(), messages.end());
    
    while (i < MSGN_NUM) {
        getLabel(m_messageNode[i])->setString("");
        getLabel(m_messageNode[i])->setVisible(false);
        getLabel(m_messageNode[i])->stopAllActions();
        i++;
    }
    
}

CCLabelBMFont* MessageNotification::getLabel(cocos2d::CCNode* messageNode) {
    
    return (CCLabelBMFont*)messageNode->getChildByTag(MSGN_LABEL_TAG);
}

void MessageNotification::setMessagePosition(cocos2d::CCPoint position_) {
    
    m_messagePosition = position_;
    
    for (int i=0; i<MSGN_NUM; i++) {
        getLabel(m_messageNode[i])->setPosition(m_messagePosition + CCPoint(0,  i*(MSGN_FONT_SIZE*1.2f*retinaMultiplier)) );
    }
    
}

void MessageNotification::addMessage(MessageNotificationType type_, std::string msg_) {
    
    MessageNotification::addMessage(type_, msg_, true);
    
}

void MessageNotification::addMessage(MessageNotificationType type_, string msg_, bool force_) {
    
    if (!force_) {
        for (vector<MessageNotificationItem*>::iterator it=messages.begin(); it!=messages.end(); ++it) {
            if (MessageNotificationItem* _msg = (MessageNotificationItem*)(*it)) {
                if (_msg->msg == msg_) {
                    return;
                }
            }
        }
    }
    
    MessageNotificationItem* _newMsg = new MessageNotificationItem();
    _newMsg->type = type_;
    _newMsg->life = MSGN_LIFE;
    _newMsg->msg = msg_;
    
    messages.push_back(_newMsg);
    
}

//void MessageNotification::setFontSize(float fontSize)
//{
//    for (int i=0; i<MSGN_NUM; i++) {
//        //getLabel(m_messageNode[i])->setFontSize(fontSize*scaleFactor);
//        this->addChild(getLabel(m_messageNode[i]));
//    }
//}
