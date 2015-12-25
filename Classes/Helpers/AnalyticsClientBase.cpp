//
//  AnalyticsClientWrapper.mm
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 1/8/2558 BE.
//
//

#include "cocos-ext.h"
#include "AnalyticsClientWrapper.h"
#include "AnalyticsHelper.h"
#include "DeviceInfo.h"
#include "../Settings.h"
#include "../Helpers/Versioning.h"
#include "../Entities/PlayerState.h"
//#include "support/zip_support/Zipper.h"

USING_NS_CC;

#pragma mark - Instance Handling


#define ANALYTICS_URL "http://analytics.pocketplaylab.com/events"
#define ANALYTICS_URL_DEBUG "http://analytics.pocketplaylab.com/events?debug=true"
#define ANALYTICS_HTTP_RETRIES (1)


extern double currentTimeClient;

AnalyticsClient *__analyticsClient = NULL;
AnalyticsClient *AnalyticsClient::getInstance()
{
    if (!__analyticsClient) {
        __analyticsClient = new AnalyticsClient();
    }

    return __analyticsClient;
}

AnalyticsClient::AnalyticsClient()
{
    m_userID = "";
    m_userGender = "";
    m_userEmail = "";
    m_facebookID = "";
    timeSinceSessionStarted = 0.0f;
}

AnalyticsClient::~AnalyticsClient()
{
}

#pragma mark - Session Management

void AnalyticsClient::startSession(StartSessionOptions options)
{
    m_sessionID = generateUUID();
    m_startSessionOptions = options;

    CSJson::Value eventParameters;
    if (options == StartSessionOptionsSceneBoost) {
        eventParameters["options"] = "SceneBoost";
    } else if (options == StartSessionOptionsAppResume) {
        eventParameters["options"] = "AppResume";
    }
    logEvent(EVENT_SESSION_START, eventParameters);
}

#pragma mark - Update

#pragma mark - Set Client Data Manually

void AnalyticsClient::setUserID(std::string userID)
{
    m_userID = userID;
}

void AnalyticsClient::setUserGender(std::string userGender)
{
    m_userGender = userGender;
}

void AnalyticsClient::setUserAgeRangeMinimum(std::string userAgeRangeMinimum)
{
    m_userAgeRangeMinimum = userAgeRangeMinimum;
}

void AnalyticsClient::setUserEmail(std::string userEmail)
{
    m_userEmail = userEmail;
}

void AnalyticsClient::setFacebookID(std::string facebookID)
{
    m_facebookID = facebookID;
}

#pragma mark - Methods to Get Data by Library

std::string AnalyticsClient::getCurrentTime()
{
    return getDateFromTimestamp(currentTimeClient);
}

std::string AnalyticsClient::getDateFromTimestamp(time_t timestamp)
{
    time_t t = timestamp;
    struct tm *tm = localtime(&t);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", tm); //format is 2014-12-19 2:55:17
    return date;
}

std::string AnalyticsClient::getAppVersion()
{
    return Versioning::GetBundleVersionShort();
}

std::string AnalyticsClient::getSystemVersion()
{
    return DeviceInfo::getSystemVersion();
}

std::string AnalyticsClient::getClientLibraryVersion()
{
    return CLIENT_LIBRARY_VERSION;
}

std::string AnalyticsClient::getPlatform()
{
    return PLATFORM_IOS;
}

std::string AnalyticsClient::getDeviceModel()
{
    return DeviceInfo::getModel();
}

std::string AnalyticsClient::getDeviceName()
{
    return DeviceInfo::getName();
}

std::string AnalyticsClient::getLanguage()
{
    return DeviceInfo::getLanguage();
}

CSJson::Value AnalyticsClient::getUserInfo()
{
    CSJson::Value userInfo;

    userInfo["appVersion"] = getAppVersion();
    userInfo["systemVersion"] = getSystemVersion();
    userInfo["clientLibVersion"] = getClientLibraryVersion();
    userInfo["platform"] = getPlatform();
    userInfo["deviceModel"] = getDeviceModel();
    userInfo["deviceName"] = getDeviceName();
    userInfo["language"] = getLanguage();
    userInfo["usingWifi"] = usingWifi();
    userInfo["userID"] = m_userID;
    userInfo["userGender"] = m_userGender;
    userInfo["userAgeRangeMinimum"] = m_userAgeRangeMinimum;
    userInfo["userEmail"] = m_userEmail;
    userInfo["facebookID"] = m_facebookID;

    PlayerState *_playerState = PlayerState::getInstance();

    userInfo["accountCreated"] = _playerState->PurchaseStat.account_created;
    userInfo["purchaseQuantity"] = _playerState->PurchaseStat.total_purchase;
    userInfo["firstPurchaseDate"] = _playerState->PurchaseStat.first_purchase_datetime;
    userInfo["lastPurchaseDate"] = _playerState->PurchaseStat.last_purchase_datetime;
    userInfo["totalPurchaseValue"] = CCString::createWithFormat("%.2f",_playerState->PurchaseStat.total_purchase_value)->getCString();
    userInfo["highestPackagePurchase"] = _playerState->PurchaseStat.highest_package_purchased;

    return userInfo;
}

#pragma mark - Client Library Tools

std::string AnalyticsClient::write(const CSJson::Value &value)
{
    CSJson::FastWriter writer;

    std::string output = writer.write(value);

//    CCLog_("......... AnalyticsClient - JSON Result .........\n%s", output.c_str());

    return output;
}

void AnalyticsClient::logEvent(std::string eventName)
{
    CSJson::Value emptyEventParameters;
    AnalyticsClient::logEvent(eventName, emptyEventParameters);
}

void AnalyticsClient::logEvent(std::string eventName, const CSJson::Value& eventParameters)
{
    CSJson::Value root;

    root["event_uuid"] = generateUUID();
    root["install_uuid"] = m_userID;
    root["app_id"] = kAnalyticsClientAppID;
    root["app_env"] = kAnalyticsClientAppEnvironment;
    root["session_uuid"] = m_sessionID;
    root["timestamp"] = (unsigned int)time(0);
    root["event_name"] = eventName;

    CSJson::Value extra;

    extra["eventParameters"] = eventParameters;

    CSJson::Value metadata;

    metadata["userInfo"] = getUserInfo();

    metadata["playerInfo"] = AnalyticsHelper::getPlayerInfo();
    metadata["petsInfo"] = AnalyticsHelper::getPetsInfo();
    metadata["buildingsInfo"] = AnalyticsHelper::getBuildingsInfo();
    metadata["abilitiesInfo"] = AnalyticsHelper::getAbilitiesInfo();
    metadata["sessionPurchasedInfo"] = AnalyticsHelper::getSessionPurchasedInfo();

    metadata["timestampFromServer"] = currentTimeClient;

    extra["metadata"] = metadata;

    root["extra"] = extra;

    queueData(root);
}

void AnalyticsClient::queueData(const CSJson::Value& obj)
{
    CCLog_("queuingData:%s", write(obj).c_str());
    m_queue.push_back(obj);
}

std::string AnalyticsClient::compressData(std::string data) const
{
    //return Zipper::compress_string(data);
}

void AnalyticsClient::onResponded(int code, std::string data)
{
    if (code >= 400)
    {
        CCLog_("AnalyticsClient request code error: %d %s", code, data.c_str());
    }
    else
    {
        CCLog_("AnalyticsClient request code success: %d %s", code, data.c_str());
    }

    m_queue.clear();
}

std::string AnalyticsClient::getCompressedData() const
{
    if (m_queue.empty())
    {
        return std::string();
    }

    //This is called automatically once the app goes to background
    //You can also call this manually

    CSJson::Value root;

    //! batch json events
    for (auto &item : m_queue)
    {
        root["events"].append(item);
    }

    std::string batchJson = write(root);
    CCLog_("BatchedJSON: %s", batchJson.c_str());

    std::string compressedJson = compressData(batchJson);

    CCLog_("Uncompressed size: %d", batchJson.length());
    CCLog_("Compressed size: %d", compressedJson.length());

    return compressedJson;
}
