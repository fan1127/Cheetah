//
//  ModalDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#ifndef __Cheetah__PushNotificationDialog__
#define __Cheetah__PushNotificationDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Helpers/UIHelper.h"
#include "NotificationDialog.h"


class PushNotifications;

class PushNotificationsDialog : public NotificationDialog
{
protected:

        PushNotifications*                  m_notifications;

public:

    PushNotificationsDialog();

    virtual ~PushNotificationsDialog();

    static PushNotificationsDialog* create(CCModalController* parent);

    static PushNotificationsDialog *create(CCModalController* parent, cocos2d::CCSize dialogSize);

    void setNotifications(PushNotifications* notes) { m_notifications = notes; }

    virtual void onButtonYes(cocos2d::CCObject *sender);

    virtual void onButtonNo(cocos2d::CCObject *sender);
};

#endif /* defined(__Cheetah__StandardDialog__) */
