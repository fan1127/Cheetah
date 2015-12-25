//
//  AppRatingsDialog.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/7/2557 BE.
//
//

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
#include "../../Helpers/AppRating/AppRating.h"

#include "AppMacros.h"
#include "AppRatingDialog.h"

extern float sizeMultiplier;
extern GameAudioService *g_gameAudio;
extern CCScene *g_currentScene;
extern GameContentService *g_gameContent;
extern PlayerState* g_playerState;
extern TargetDevice targetDevice;

AppRatingsDialog::AppRatingsDialog() :
        NotificationDialog(),
        m_ratings(NULL)
{}

AppRatingsDialog::~AppRatingsDialog()
{
}

AppRatingsDialog* AppRatingsDialog::create(CCModalController* parent)
{
    return AppRatingsDialog::create(parent, kDialogRatingSize);
}

AppRatingsDialog *AppRatingsDialog::create(CCModalController* parent, cocos2d::CCSize dialogSize)
{
    AppRatingsDialog *pRet = new AppRatingsDialog();
    pRet->init(parent, dialogSize);
    pRet->autorelease();
    return pRet;
}

void AppRatingsDialog::onButtonYes(CCObject *sender)
{
    NotificationDialog::onButtonYes(sender);

    if (m_buttonNo)
    {
        m_ratings->onRateButtonYes();
    }
    else
    {
        m_ratings->onRateDislikeOk();
    }
}

void AppRatingsDialog::onButtonNo(CCObject *sender)
{
    NotificationDialog::onButtonNo(sender);
    m_ratings->onRateButtonNo();
}
