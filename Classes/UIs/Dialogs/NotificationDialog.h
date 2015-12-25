//
//  ModalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__NotificationDialog__
#define __Cheetah__NotificationDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/PushNotifications/PushNotifications.h"


namespace cocos2d { namespace extension { class CCScale9Sprite; } }
class CCPhatTouchableSprite;
class PushNotifications;

class NotificationDialog : public CCModalView
{
protected:

    CCPhatTouchableSprite*              m_block_sprite;
    cocos2d::extension::CCScale9Sprite* m_background;
    cocos2d::CCLabelBMFont*             m_title;
    cocos2d::CCLabelBMFont*             m_description;
    cocos2d::CCMenu*                    m_buttonYes;
    cocos2d::CCMenu*                    m_buttonNo;

    cocos2d::CCMenu* createButton(bool yesButton, const char* title, void (CCObject::*SEL_MenuHandler)(CCObject*));

public:

    NotificationDialog();

    virtual ~NotificationDialog();

    static NotificationDialog* create(CCModalController* parent);

    static NotificationDialog *create(CCModalController* parent, cocos2d::CCSize dialogSize);

    virtual bool init(CCModalController* parent, cocos2d::CCSize dialogSize);

    virtual void createTitle(const char *name);

    virtual void setTitle(const char *name);

    virtual void createDescription(const char *description);

    virtual void setDescription(const char* desc);

    void createButtons(const char* yesTitle, const char* noTitle);

    virtual void onButtonYes(cocos2d::CCObject *sender);

    virtual void onButtonNo(cocos2d::CCObject *sender);

    void createOkButton(const char* title);
};

#endif /* defined(__Cheetah__StandardDialog__) */
