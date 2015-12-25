//
// Created by Vaclav Samec on 11/10/14 AD.
//

#include "PushNotifications.h"
#include "PushNotificationsWrapper.h"
#include <cocos2d.h>
#include "../../UIs/Dialogs/PushNotificationsDialog.h"
#include "../../Scenes/CScene.h"
#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/FlurryCpp.h"

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIUserUpdate.h"
#include "../../Scenes/SceneVille.h"


using namespace cocos2d;


const char* PushNotifications::KEY_LAST_NOTIFICATION =              "PN_LAST_NOTIFICATION";
const char* PushNotifications::KEY_NOTIFICATION_EVENT_NUMBER =      "PN_EVENT_NUMBER";
const char* PushNotifications::KEY_NOTIFICATION_ALLOW =             "PN_ALLOW";
const char* PushNotifications::KEY_NOTIFICATION_PET_WAKE_UP =       "PN_WAKE_UP";
const char* PushNotifications::KEY_NOTIFICATION_BUILDING_UPGRADE =  "PN_UPGRADE_BUILDING";
const char* PushNotifications::KEY_NOTIFICATION_SCENE_VILLE =       "PN_SCENE_VILLE";

// at least 5 minutes
const int PushNotifications::upgradeCountdownMin = 5*60;

// first timer is set for 30 minutes
const float PushNotifications::firstTimerMin = 30*60.0f;

// second timer is set for 3 hours
const float PushNotifications::secondTimerMin = 3*60*60.0f;


PushNotifications& PushNotifications::Instance()
{
    static PushNotifications instance;
    return instance;
}

PushNotifications::PushNotifications() :
        m_lastNotification(0),
        m_notificationEventNumber(0),
        m_wakeUpEvent(false),
        m_upgradeEvent(false),
        m_sceneVilleEvent(false),
        m_currEvent(PushNotifications::None)
{
//#ifdef IS_DEBUG_BUILD
//    CCUserDefault::purgeSharedUserDefault();
//    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_LAST_NOTIFICATION, 0);
//    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_NOTIFICATION_EVENT_NUMBER, 0);
//    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_ALLOW, false);
//    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_PET_WAKE_UP, false);
//    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_BUILDING_UPGRADE, false);
//    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_SCENE_VILLE, false);
//    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_ALLOW, false);
//#endif

    m_wrapper = new PushNotificationsWrapper();
    loadData();
}

PushNotifications::~PushNotifications()
{
    CC_SAFE_DELETE(m_wrapper);
}

void PushNotifications::loadData()
{
    // read timestamp from user data
    m_lastNotification = (unsigned int)CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_LAST_NOTIFICATION, 0);

    // read last number of notifications
    m_notificationEventNumber = CCUserDefault::sharedUserDefault()->getIntegerForKey(KEY_NOTIFICATION_EVENT_NUMBER, 0);

    // read allowness
    m_allowNotifications = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_NOTIFICATION_ALLOW, false);

    m_wakeUpEvent = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_NOTIFICATION_PET_WAKE_UP, false);
    m_upgradeEvent = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_NOTIFICATION_BUILDING_UPGRADE, false);
    m_sceneVilleEvent = CCUserDefault::sharedUserDefault()->getBoolForKey(KEY_NOTIFICATION_SCENE_VILLE, false);
}

void PushNotifications::saveData()
{
    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_PET_WAKE_UP, m_wakeUpEvent);
    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_BUILDING_UPGRADE, m_upgradeEvent);
    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_SCENE_VILLE, m_sceneVilleEvent);

    // write timestamp from user data
    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_LAST_NOTIFICATION, m_lastNotification);

    // write last number of notifications
    CCUserDefault::sharedUserDefault()->setIntegerForKey(KEY_NOTIFICATION_EVENT_NUMBER, m_notificationEventNumber);

    // write allowness
    CCUserDefault::sharedUserDefault()->setBoolForKey(KEY_NOTIFICATION_ALLOW, m_allowNotifications);
}

bool PushNotifications::notify(NotificationEvents event, std::string param0)
{
    extern unsigned int currentTime;

    CCLog_("PushNotifications::notify %d %d %d %d %d %u", m_notificationEventNumber, m_allowNotifications, m_wakeUpEvent, m_upgradeEvent, m_sceneVilleEvent, currentTime - m_lastNotification);

    // if allowed don't show the dialog again
    if (m_allowNotifications)
    {
        return false;
    }

    // disable notification dialog after 3 failed attempts
    if (!m_allowNotifications && m_notificationEventNumber >= 3)
    {
        return false;
    }

    // wait 3 hours for second notifications
    if (m_notificationEventNumber == 1)
    {
        if (currentTime - m_lastNotification < getFirstTimeout())
        {
            return false;
        }
    }

    if (m_notificationEventNumber == 2)
    {
        if (currentTime - m_lastNotification < getSecondTimeout())
        {
            return false;
        }
    }

    switch (event)
    {
        case PushNotifications::FirstTimeInVille:
        {
            if (!m_sceneVilleEvent)
            {
                auto dialog = createNotificationDialog();

                if (dialog)
                {
                    m_currEvent = event;
                    dialog->createTitle("Notifications");
                    dialog->createDescription("Please accept notifications so you don't miss out on any rewards!");
                    dialog->createButtons("Accept", "No Thanks");
//                CCAssert(m_notificationEventNumber == 0, "First notification should be first time in ville!");
                    m_notificationEventNumber = 1;
                    m_sceneVilleEvent = true;
                }
            }
        }
        break;

        case PushNotifications::FirstTimeAnimalWakeUp:
        {
            if (!m_wakeUpEvent)
            {
                auto dialog = createNotificationDialog();

                if (dialog)
                {
                    m_currEvent = event;
                    dialog->createTitle("All your pets are sleeping");
                    dialog->createDescription("Would you like to be alerted when your pets wake up?");
                    dialog->createButtons("Notify Me", "No Thanks");
//                CCAssert(m_notificationEventNumber > 0, "First notification should be first time in ville!");
                    m_notificationEventNumber ++;
                    m_wakeUpEvent = true;
                }
            }
        }
        break;

        case PushNotifications::FirstTimeBuildingUpgrade:
        {
            if (!m_upgradeEvent)
            {
                auto dialog = createNotificationDialog();

                if (dialog)
                {
                    m_currEvent = event;
                    dialog->createTitle("Your building is upgrading");
                    std::string desc = "Would you like to be notified when your " + param0 + " is finished?";
                    dialog->createDescription(desc.c_str());
                    dialog->createButtons("Notify Me", "No Thanks");
//                CCAssert(m_notificationEventNumber > 0, "First notification should be first time in ville!");
                    m_notificationEventNumber ++;
                    m_upgradeEvent = true;
                }
            }
        }
        break;

        default:
        {
            CCAssert(false, "Invalid notification event!");
        }
        break;
    }

    return true;
}

void PushNotifications::onButtonYes()
{
    extern unsigned int currentTime;

    m_allowNotifications = true;
    m_lastNotification = currentTime;
    saveData();
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(ccs("Yes"), "Result");
    params->setObject(ccs(getStringFromEvent(m_currEvent)), "Event");
    FlurryCpp::logEvent(EVENT_PUSH_NOTIFICATION_DIALOG, params);
    
    CSJson::Value eventParameters;
    eventParameters["result"] = "Yes";
    eventParameters["event"] = getStringFromEvent(m_currEvent);
    AnalyticsClient::getInstance()->logEvent(EVENT_PUSH_NOTIFICATION_DIALOG, eventParameters);

    // show the native dialog
    invokeNativeConfirmationDialog();
}

void PushNotifications::onButtonNo()
{
    extern unsigned int currentTime;

    m_allowNotifications = false;
    m_lastNotification = currentTime;
    saveData();
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(ccs("No"), "Result");
    params->setObject(ccs(getStringFromEvent(m_currEvent)), "Event");
    FlurryCpp::logEvent(EVENT_PUSH_NOTIFICATION_DIALOG, params);
    
    CSJson::Value eventParameters;
    eventParameters["result"] = "No";
    eventParameters["event"] = getStringFromEvent(m_currEvent);
    AnalyticsClient::getInstance()->logEvent(EVENT_PUSH_NOTIFICATION_DIALOG, eventParameters);
}

std::string PushNotifications::getStringFromEvent(PushNotifications::NotificationEvents event)
{
    std::string eventString = "";
    
    if (event == PushNotifications::FirstTimeInVille) {
        eventString = "FirstTimeInVille";
    } else if (event == PushNotifications::FirstTimeAnimalWakeUp) {
        eventString = "FirstTimeAnimalWakeUp";
    } else if (event == PushNotifications::FirstTimeAnimalWakeUp) {
        eventString = "FirstTimeAnimalWakeUp";
    }
    
    return eventString;
}

void PushNotifications::onRegisteredRemoteNotifications(std::string token)
{
    CCLog_("onRegisteredRemoteNotifications %s", (std::string("ios_token=")+token).c_str());
    BackendService::getInstance()->send(APIUserUpdate::create(std::string("ios_token=")+token, NULL, NULL));
}

void PushNotifications::onRegisteredRemoteNotificationsAndroid(std::string regID)
{
	 CCLog_("onRegisteredRemoteNotifications %s", (std::string("android_token=")+regID).c_str());
	 BackendService::getInstance()->send(APIUserUpdate::create(std::string("android_token=")+regID, NULL, NULL));
}

void PushNotifications::invokeNativeConfirmationDialog()
{
    m_wrapper->RegisterRemoteNotifications();
}

float PushNotifications::getFirstTimeout() const
{
    return firstTimerMin;
}

float PushNotifications::getSecondTimeout() const
{
    return secondTimerMin;
}

PushNotificationsDialog* PushNotifications::createNotificationDialog()
{
    extern CCScene* g_currentScene;

    auto currScene = dynamic_cast<SceneVille*>(g_currentScene);
    
    if (currScene == NULL)
        return NULL;
    
    if (currScene->modalController->getParent() != NULL)
        return NULL;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    auto note = PushNotificationsDialog::create(currScene->modalController);
    note->setPosition(_screenSize*0.5f);
    g_currentScene->addChild(currScene->modalController, kModalControllerZOrder);
    currScene->modalController->pushView(note);
    note->setNotifications(this);
    
    return note;
}
