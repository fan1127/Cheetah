//
//  AnalyticsClientWrapper.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 1/8/2558 BE.
//
//

#ifndef __Cheetah__AnalyticsClientWrapper__
#define __Cheetah__AnalyticsClientWrapper__

#include <stdio.h>
#include <vector>

#include "cocos2d.h"

#include "../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
//#include "reader.h"
#include "../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

#define CLIENT_LIBRARY_VERSION "0.1"
#define PLATFORM_IOS "ios"
#define PLATFORM_ANDROID "android"

#define EVENT_SESSION_START "session_start"
#define EVENT_SESSION_END "session_end"

enum StartSessionOptions {
    StartSessionOptionsUnknown = 0,
    StartSessionOptionsSceneBoost,
    StartSessionOptionsAppResume
};

namespace cocos2d { namespace extension { class CCHttpClient; class CCHttpResponse; } }

class AnalyticsClient {
    
    std::string m_sessionID;
    StartSessionOptions m_startSessionOptions;
    
    std::string m_userID;
    std::string m_userGender;
    std::string m_userAgeRangeMinimum;
    std::string m_userEmail;
    std::string m_facebookID;

    std::vector<CSJson::Value> m_queue;

    float timeSinceSessionStarted;

    void onResponded(int code, std::string data);
    std::string getCompressedData() const;
    void queueData(const CSJson::Value&);
    std::string compressData(std::string data) const;
    void sendData();

public:
    
    //Instance Handling
    static AnalyticsClient *getInstance();
    AnalyticsClient();
    ~AnalyticsClient();
    
    //Session Management
    void startSession(StartSessionOptions options);
    
    //Set Client Data Manually
    void setUserID(std::string userID);
    void setUserGender(std::string userGender);
    void setUserAgeRangeMinimum(std::string userAgeRangeMinimum);
    void setUserEmail(std::string userEmail);
    void setFacebookID(std::string facebookID);
    
    //Methods to Get Data by Library
    CSJson::Value getUserInfo();
    std::string getCurrentTime();
    std::string getDateFromTimestamp(time_t timestamp);
    std::string getAppVersion();
    std::string getSystemVersion();
    std::string getClientLibraryVersion();
    std::string getPlatform();
    std::string getDeviceModel();
    std::string getDeviceName();
    std::string getLanguage();
    bool usingWifi();
    std::string generateUUID();
    
    //Client Library Tools
    static std::string write(const CSJson::Value &value);
    void logEvent(std::string eventName);
    void logEvent(std::string eventName, const CSJson::Value& eventParameters);

    //Events
    void appDidEnterBackground();
    
};

#endif /* defined(__Cheetah__AnalyticsClientWrapper__) */
