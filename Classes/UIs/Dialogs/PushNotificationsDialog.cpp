//
//  PushNotificationsDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

#include "PushNotificationsDialog.h"
#include "../../Settings.h"
#include "../../DialogSettings.h"

#include "../../Services/GameAudioService.h"
#include "../../Scenes/SceneVille.h"
#include "../../Services/GameContentService.h"
#include "../../Helpers/CCSpriteHelper.h"
#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/UIHelper.h"
#include "../../Helpers/CCLabelUpdater.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../AppMacros.h"
#include "../../Helpers/PushNotifications/PushNotifications.h"

extern float sizeMultiplier;
extern GameAudioService *g_gameAudio;
extern CCScene *g_currentScene;
extern GameContentService *g_gameContent;
extern PlayerState* g_playerState;
extern TargetDevice targetDevice;

PushNotificationsDialog::PushNotificationsDialog() :
        NotificationDialog(),
        m_notifications(NULL)
{}

PushNotificationsDialog::~PushNotificationsDialog()
{
}

PushNotificationsDialog* PushNotificationsDialog::create(CCModalController* parent)
{
    return PushNotificationsDialog::create(parent, kDialogNotificationSize);
}

PushNotificationsDialog *PushNotificationsDialog::create(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    PushNotificationsDialog *pRet = new PushNotificationsDialog();
    pRet->init(parent, dialogSize);
    pRet->autorelease();
    return pRet;
}

void PushNotificationsDialog::onButtonYes(CCObject *sender)
{
    NotificationDialog::onButtonYes(sender);
    m_notifications->onButtonYes();
}

void PushNotificationsDialog::onButtonNo(CCObject *sender)
{
    NotificationDialog::onButtonNo(sender);
    m_notifications->onButtonNo();
}
