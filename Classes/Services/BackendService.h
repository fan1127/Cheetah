//
//  BackendService.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__BackendService__
#define __Cheetah__BackendService__

#include <iostream>
#include "../Settings.h"

/* Backend Service Connection will use CCHttpRequest extension provided by cocos2d-x community
 * By commenting line below, Backend Service connection will use CURL instead */

enum BackendConnection {
    BC_CCHTTP,
    BC_CURL,
    BC_PS //un-implemented - socket connection
};

#include "Apis/APIRootClass.h"

#define kBackendConnection BC_CCHTTP

#if kBackendConnection==BC_CCHTTP
#include "cocos2d.h"
#include "cocos-ext.h"
#else
#include <curl/curl.h>
#endif

#include "../VOs/GDPetVO.h"

#define DEFAULT_TIMEOUT         2

#define PRO_SERVER              0
#define DEV_SERVER              1

#if IS_RELEASE_BUILD
#define SELECT_SERVER           PRO_SERVER          // RELEASE BUILD
#elif IS_ADHOC_BUILD
#define SELECT_SERVER           DEV_SERVER          // AD HOC BUILD
#else
#define SELECT_SERVER           DEV_SERVER          // DEBUG BUILD
#endif

#define INTERNET_CHECKING_URL   "http://graph.facebook.com/"

#if SELECT_SERVER == DEV_SERVER
#define DEFAULT_BACKEND_URL     "http://54.172.151.126/api/"
//#define DEFAULT_BACKEND_URL     "http://192.168.3.226:8080/api/"
//#define DEFAULT_BACKEND_URL     "http://api.hoofitgame.com:8081/api/"
#elif SELECT_SERVER == PRO_SERVER
#define DEFAULT_BACKEND_URL     "http://api.hoofitgame.com:8081/api/"
//#define DEFAULT_BACKEND_URL     "http://54.165.219.220:8083/api/"
#endif

#define ZIP_RESPONSE            true
#define SOURCE_SIZE             12

#define DEFAULT_BACKEND_PORT    "8080"
#define DEFAULT_BACKEND_PASSWD  "thisisabook"
#define DEFAULT_BACKEND_VERSION "1.0.23"

#define API_RESOURCE_TYPE_COIN   "coin"
#define API_RESOURCE_TYPE_FOOD   "food"

enum kBackendServiceErrorCode {
    kBackendServiceErrorCodeSomeError,
    kBackendServiceErrorCodeNoResponse,
    kBackendServiceErrorCodeNoError,
};

class BackendService {
    
    std::string     m_rawData;
    std::string     m_url;
    std::string     m_host;
    std::string     m_port;
    std::string     m_version;
    
    std::string     m_link;
    std::string     m_declaration_message;
    int             m_declaration_interval;
    
    bool            m_did_user_get;
    bool            m_did_alert;
    bool            m_did_public_internet_respond;
    
    std::vector<APIRootClass*>  m_requests_queue;
    std::vector<APIRootClass*>  m_requests_sent;
    void onResponded(cocos2d::extension::CCHttpClient *sender,
                     cocos2d::extension::CCHttpResponse *response);
    
    kBackendServiceErrorCode _parseErrorResult(const char *json_str_);
    
    void _callback_internet_check(cocos2d::extension::CCHttpClient *sender_,
                                  cocos2d::extension::CCHttpResponse *response_);

    void *m_activeSender;
    void (*m_activeCallback)(void*);
    void _doActiveCallback();
    
public:
    
    bool            didRespond = false;
    
    bool            cannot_connect_to_server;
    
    BackendService();
    ~BackendService();
    
    static BackendService *getInstance();
    static bool isCreated();
    bool send(APIRootClass*);
    void cancelAllRequestsInQueue();
    void cancelAllRequests();
    void clearAllRequests();
    bool isWaitingForResponse();
    bool isWaitingForQueue();
    void setActiveCallback(void* sender, void (*callback)(void*));
    void unsetActiveCallback();
    
    std::string getDeclarationMessage() { return m_declaration_message; }
    int getDeclarationInterval() { return m_declaration_interval; }
    
    bool retryRequestOrCheckInternet();
    void internetCheck();

    void kick_back_to_front(bool firstBtnPressed_);
    void bypass_message(bool firstBtnPressed_);
    
};

#endif /* defined(__Cheetah__BackendService__) */
