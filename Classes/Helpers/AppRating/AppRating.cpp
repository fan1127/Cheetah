//
//  AppRating.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "AppRating.h"
#include "RatingWrapper.h"
#include "../../UIs/Dialogs/AppRatingDialog.h"
#include "../../Scenes/CScene.h"
#include "../../Entities/PlayerState.h"
#include "../../Extensions/CCOpenURL/CCOpenURL.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

const char* AppRating::KEY_APP_RATING_LEVEL = "AR_LEVEL";

AppRating& AppRating::Instance()
{
    static AppRating instance;
    return instance;
}

AppRating::AppRating() : CCNode()
{
    m_rating = new RatingWrapper();

    loadData();
}

void AppRating::initialize()
{
    m_rating->init();
}

void AppRating::onEnterForeground()
{
    m_rating->onEnterForeground();
}

bool AppRating::canRate()
{
    return m_levelUp && m_rating->canRate();
}

void AppRating::showRateDialog()
{
    if (canRate())
    {
        auto dialog = createNotificationDialog();

        if (dialog)
        {
            dialog->createTitle("Are you enjoying Ranch Run?");
            dialog->createDescription("Please rate 5 stars");
            dialog->createButtons("Yes", "No");
        }
    }
}

void AppRating::showRateDialogDislike()
{
    auto dialog = createNotificationDialog();

    if (dialog)
    {
        dialog->createTitle("Rate the game");
        dialog->createDescription("Don't like our game? Please tell us why?");
        dialog->createOkButton("Ok");
    }
}

void AppRating::onRateButtonNo()
{
//    setupUpdate();
//    m_dialogTimeout = 0.5f;
    m_rating->dontRateApp();
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(ccs("No"), "Result");
    FlurryCpp::logEvent(EVENT_RATE_US, params);
    
    CSJson::Value eventParameters;
    eventParameters["result"] = false;
    AnalyticsClient::getInstance()->logEvent(EVENT_RATE_US, eventParameters);
}

void AppRating::onRateButtonYes()
{
    CCLog_("Rate 5 stars ... ");
    m_rating->rateApp();
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(ccs("Yes"), "Result");
    FlurryCpp::logEvent(EVENT_RATE_US, params);
    
    CSJson::Value eventParameters;
    eventParameters["result"] = true;
    AnalyticsClient::getInstance()->logEvent(EVENT_RATE_US, eventParameters);
}

void AppRating::onRateDislikeOk()
{
    extern PlayerState *g_playerState;

    CREATE_CHAR_BUFFER(_buffer, 128);
    snprintf(_buffer, _buffer_size, "mailto:%s?subject=%s&body=my_uuid_is=%s", kEmailAddress, kEmailSubject, g_playerState->player.uuid.c_str());

    CCOpenURL::openURL(_buffer);
}

AppRatingsDialog* AppRating::createNotificationDialog()
{
    extern CCScene* g_currentScene;

    auto currScene = static_cast<CScene*>(g_currentScene);

    if (currScene)
    {
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        auto note = AppRatingsDialog::create(currScene->modalController);
        note->setPosition(_screenSize*0.5f);
        g_currentScene->addChild(currScene->modalController, kModalControllerZOrder);
        currScene->modalController->pushView(note);
        note->setRatings(this);

        return note;
    }

    return NULL;
}

void AppRating::update(float dt)
{
    m_dialogTimeout -= dt;

    if (m_dialogTimeout < 0.0f)
    {
        showRateDialogDislike();
        stopUpdate();
        m_dialogTimeout = FLT_MAX;
    }
}

void AppRating::setupUpdate()
{
    extern CCScene* g_currentScene;

    if (getParent() != g_currentScene)
    {
        g_currentScene->addChild(this);
    }

    scheduleUpdate();
}

void AppRating::stopUpdate()
{
    unscheduleUpdate();
}

void AppRating::loadData()
{
    m_levelUp = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_APP_RATING_LEVEL, false);
}

void AppRating::saveData()
{
    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_APP_RATING_LEVEL, m_levelUp);
}

void AppRating::onLevelUp(int level)
{
    if (!m_levelUp)
    {
        if (level >= 5)
        {
            CCLog_("AppRating::onLevelUp %d", level);
            m_levelUp = true;
            saveData();
        }
    }
}
