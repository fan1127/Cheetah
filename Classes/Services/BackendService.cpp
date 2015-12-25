//
//  BackendService.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#include "BackendService.h"


#include <iostream>
using namespace std;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../Helpers/MD5.h"

extern string gcid, uuid, token, auuid, race_id;
extern string device_token;

#include "GameContentService.h"
extern GameContentService*  g_gameContent;
extern BackendService*      g_gameBackend;

#include "../Entities/PlayerState.h"
extern PlayerState* g_playerState;
extern PlayerState* g_opponentState;

#include "../Extensions/CCAlert/CCAlert.h"

#include "../SceneManager.h"
#include "../Scenes/SceneBoost.h"
#include "../Scenes/SceneVille.h"
#include "../Scenes/SceneMatch.h"
#include "../Scenes/SceneRace.h"
#include "../Scenes/SceneWorldMap.h"
#include "../UIs/ShopItemBuilding.h"
USING_NS_CC;
USING_NS_CC_EXT;

#include "../Entities/Ability.h"

extern SceneManager* g_sceneManager;
extern CCScene* g_currentScene;

#include "../Extensions/CCOpenURL/CCOpenURL.h"
#include "../Helpers/StringHelper.h"
#include "../Helpers/Versioning.h"
#include "../Helpers/PointerPool.h"

BackendService *g_gameBackend = NULL;
BackendService *BackendService::getInstance() {
    if (!g_gameBackend) g_gameBackend = new BackendService();
    return g_gameBackend;
}

bool BackendService::isCreated() {
    
    return g_gameBackend != NULL;
    
}

struct MyExeption: public std::exception {
    std::string s;
    MyExeption(std::string ss) : s(ss) {}
    ~MyExeption() throw() {}
    const char* what() const throw() { return s.c_str(); }
};

BackendService::BackendService() {
    
    m_url = DEFAULT_BACKEND_URL;
    m_port = DEFAULT_BACKEND_PORT;
    m_version = DEFAULT_BACKEND_VERSION;
    
    m_declaration_message = "";
    m_declaration_interval = 0;
    
    m_did_user_get = false;
    m_did_alert = false;
    m_did_public_internet_respond = false;
    
    ///
    m_requests_queue.clear();
    m_requests_sent.clear();
    
    unsetActiveCallback();
    
    cannot_connect_to_server = false;

    CCHttpClient::getInstance()->setTimeoutForConnect(20);
    CCHttpClient::getInstance()->setTimeoutForRead(20);
    
}

BackendService::~BackendService() {
    
#if kBackendConnection == BC_CCHTTP
    
#else
    curl_easy_cleanup(m_curl);
    m_curl = NULL;
#endif
    
}

#pragma mark - API framework
bool BackendService::send(APIRootClass *api_request) {
    
    if (api_request) {
        m_requests_queue.push_back(api_request);
    }
    
    if (m_requests_queue.size() <= 0 ||
        m_requests_sent.size() > 0) {
        _doActiveCallback();
        return false;
    }

    APIRootClass *_api_request = m_requests_queue[0];

    string _url = m_url + _api_request->name();
    string _param = _api_request->param();
    string _param2 = _api_request->param2();
    char _source[SOURCE_SIZE] = {0};
    StringHelper::genRandomString(_source, SOURCE_SIZE);

#if IS_RELEASE_BUILD || ZIP_RESPONSE
    
    string _param_debug = _api_request->param();
    string _passwd2 = md5(_param_debug + DEFAULT_BACKEND_PASSWD);
    _param_debug += "&passwd=" + _passwd2;
    _param_debug += _param2;
    _param_debug += "&source=";
    _param_debug += _source;
    _param_debug += "&apiVersion=" + m_version;
    CCLog("%s?%s", _url.c_str(), _param_debug.c_str());
    
    if (_param != "") {
        _param.append("&");
    }
    _param.append("zip=1");

#endif
    
    _param += "&passwd=" + md5(_param + DEFAULT_BACKEND_PASSWD);
    _param += _param2;
    _param += "&source=";
    _param += _source;
    _param += "&apiVersion=" + m_version ;
    //CCLog("%s?%s", _url.c_str(), _param.c_str());
    
    CCHttpRequest* request = new cocos2d::extension::CCHttpRequest();
    request->setUrl(_url.c_str());
    request->setRequestType(CCHttpRequest::kHttpPost);
    request->setResponseCallback((CCLayer*)this, httpresponse_selector(BackendService::onResponded));
    request->setRequestData(_param.c_str(), (int)_param.length());
    request->setTag(_api_request->name().c_str());
    //request->retries = _api_request->retries;
    CCHttpClient::getInstance()->send(request);
    request->release();
    _api_request->retries --;
    
    m_requests_sent.push_back(_api_request);
    m_requests_queue.erase(m_requests_queue.begin());
    
    _doActiveCallback();
    return true;

}

void BackendService::onResponded(CCHttpClient *sender,
                                 CCHttpResponse *response) {
    
    if (!response) {
        return;
    }
    
    if (0 != strlen(response->getHttpRequest()->getTag())) {
        didRespond = true;
        //CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    vector<char> *_buffer = response->getResponseData();
    string _resp_str = "";
    for (unsigned int i=0; i<_buffer->size(); i++) {
        _resp_str.push_back((*_buffer)[i]);
    }
    
    //CCLog_("%s", _resp_str.c_str());
    
    bool skip = false;
    
    switch (_parseErrorResult(_resp_str.c_str())) {
        case kBackendServiceErrorCodeSomeError: {
            skip = true;
        }break;
        case kBackendServiceErrorCodeNoResponse: {
            if (retryRequestOrCheckInternet()) {
                return;
            }
        }break;
        default:
            break;
    }
    
    //Json allocator;
    Json* root = Json_create(_resp_str.c_str());
    
    PlayerState::getInstance()->parseCurrentTime(root);
    Json_dispose(root);

    for (vector<APIRootClass*>::iterator it=m_requests_sent.begin(); it!=m_requests_sent.end(); ++it) {
        APIRootClass *_api_request = *it;
        if (_api_request->name() != response->getHttpRequest()->getTag())
            continue;
        _api_request->onResponded(_resp_str);
        m_requests_sent.erase(it);
        if (_api_request->releaseAfterResponded) {
            delete _api_request;
        }
        break;
    }
    
    if (!skip) send(NULL);
    
}

void BackendService::cancelAllRequestsInQueue() {
    
    while ((int)m_requests_queue.size() > 0) {
        APIRootClass *_req = m_requests_queue[(int)m_requests_queue.size()-1];
        delete _req;
        _req = NULL;
        m_requests_queue.pop_back();
    }
    
}

void BackendService::cancelAllRequests() {

    while ((int)m_requests_sent.size() > 0) {
        APIRootClass *_req = m_requests_sent[(int)m_requests_sent.size()-1];
        delete _req;
        _req = NULL;
        m_requests_sent.pop_back();
    }
    
}

void BackendService::clearAllRequests() {
    
    cannot_connect_to_server = false;
    didRespond = false;
    
    cancelAllRequestsInQueue();
    cancelAllRequests();
    
}

bool BackendService::isWaitingForResponse() {
    
    return m_requests_sent.size() > 0;
    
}

bool BackendService::isWaitingForQueue() {
    
    return m_requests_queue.size() > 0;
    
}

void BackendService::setActiveCallback(void *sender, void (*callback)(void *)) {
    
    m_activeSender = sender;
    m_activeCallback = callback;
    
}

void BackendService::unsetActiveCallback() {
    
    setActiveCallback(NULL, NULL);
    
}

void BackendService::_doActiveCallback() {
    
    if (!m_activeCallback) return;
    m_activeCallback(m_activeSender);
    
}

bool BackendService::retryRequestOrCheckInternet() {
    
    if (m_requests_sent.size() > 0) {
        APIRootClass* waitingRequest = m_requests_sent[0];
        m_requests_sent.erase(m_requests_sent.begin());
        if (waitingRequest->retries > 0) {
            CCLog_("retry: %s", waitingRequest->name().c_str());
            send(waitingRequest);
        }else {
            internetCheck();
        }
        return true;
    }
    
    return false;
    
}

void BackendService::internetCheck() {
    
    m_did_public_internet_respond = false;
    CCLog_("internet_check: %s", INTERNET_CHECKING_URL);
    
    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl(INTERNET_CHECKING_URL);
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback((CCLayer*)this, httpresponse_selector(BackendService::_callback_internet_check));
    request->setTag(INTERNET_CHECKING_URL);
    CCHttpClient::getInstance()->send(request);
    request->release();

}

void BackendService::_callback_internet_check(CCHttpClient *sender_, CCHttpResponse *response_) {
    
    if (0 != strlen(response_->getHttpRequest()->getTag())) {
        //CCLog_("%s completed", response_->getHttpRequest()->getTag());
    }
    
    vector<char> *_buffer = response_->getResponseData();
    string _json_str = "";
    for (unsigned int i=0; i<_buffer->size(); i++) {
        _json_str.push_back((*_buffer)[i]);
    }

    if (_json_str != "") {
        m_did_public_internet_respond = true;
    }
    
    if (m_did_public_internet_respond) {
        //slow internet connection
        CCAlert::Show("Connection failed!", "Could not connect to the server! Please try again.", "OK", NULL, (CCObject*)this, alert_selector(BackendService::kick_back_to_front));
    }else {
        //no internet connection
        CCAlert::Show("Connection failed!", "Could not connect to the internet! Please try again.", "OK", NULL, (CCObject*)this, alert_selector(BackendService::kick_back_to_front));
    }
    
}

kBackendServiceErrorCode BackendService::_parseErrorResult(const char *json_str_) {
    
    if (m_did_alert) {
        return kBackendServiceErrorCodeSomeError;
    }

    //JsonAllocator allocator;
    Json *_root = Json_create(json_str_);
    if (!_root) {
        Json_dispose(_root);
        return kBackendServiceErrorCodeNoResponse;
    }
    
    if (Json_getItem(_root, "error")) {
        string _err = Json_getString(_root, "error", "");
        if (_err != "") {
            CCAlert::Show("Error", _err.c_str(),
                          "OK",
                          NULL,
                          (CCObject*)this,
                          alert_selector(BackendService::kick_back_to_front));
            Json_dispose(_root);
            return kBackendServiceErrorCodeSomeError;
        }
    }
    
    if (Json_getItem(_root, "redirect")) {
        m_url = Json_getString(_root, "redirect", DEFAULT_BACKEND_URL);
        m_version = Json_getString(_root, "apiVersion", DEFAULT_BACKEND_VERSION);
        kick_back_to_front(true);
        Json_dispose(_root);
        return kBackendServiceErrorCodeSomeError;
    }
    
    if (Json_getItem(_root, "warning")) {
        string _warning = Json_getString(_root, "warning", "");
        if (_warning != "") {
            CCAlert::Show("warning", _warning.c_str(), "OK", NULL, NULL, NULL);
            Json_dispose(_root);
            return kBackendServiceErrorCodeSomeError;
        }
    }
    
    if (Json_getItem(_root, "force_update_version")) {
        m_link = Json_getString(_root, "link", "http://www.avalable.com");
        CCAlert::Show("Ranch Run:", Json_getString(_root, "force_update_version", "Please update the new version from the AppStore."),
                      "OK", NULL, (CCObject*)this, alert_selector(BackendService::bypass_message));
        Json_dispose(_root);
        return kBackendServiceErrorCodeSomeError;
    }
    
    if (Json* _declaration = Json_getItem(_root, "declaration")) {
        m_declaration_message = Json_getString(_declaration, "message", "");
        m_declaration_interval = Json_getInt(_declaration, "interval", 0);
    }

    if (Json_getItem(_root, "message")) {
        m_link = Json_getString(_root, "link", "http://www.avalable.com");
        CCAlert::Show("Ranch Run:", Json_getString(_root, "message", "Maintenance time! Please come back later."),
                      "OK", NULL, (CCObject*)this, alert_selector(BackendService::bypass_message));
        Json_dispose(_root);
        return kBackendServiceErrorCodeSomeError;
    }
    
    Json_dispose(_root);
    return kBackendServiceErrorCodeNoError;
    
}

void BackendService::kick_back_to_front(bool firstBtnPressed_) {
    
    clearAllRequests();

    if (typeid(*g_currentScene) == typeid(SceneBoost)) {
        if (SceneBoost *_sceneBoost = (SceneBoost*)g_currentScene) {
            _sceneBoost->reload();
            return;
        }
    }

    if (typeid(*g_currentScene) == typeid(SceneVille)) {
        if (SceneVille *_scene = (SceneVille*)g_currentScene) {
            _scene->fadeOutThisSceneAndChangeTo(SCENE_BOOST);
            return;
        }
    }
    
    if (typeid(*g_currentScene) == typeid(SceneWorldMap)) {
        if (SceneWorldMap* _sceneWorldMap = (SceneWorldMap*)g_currentScene) {
            _sceneWorldMap->fadeOutThisSceneAndChangeTo(SCENE_BOOST);
            return;
        }
    }
    
    if (typeid(*g_currentScene) == typeid(SceneMatch)) {
        if (SceneMatch* _sceneMatch = (SceneMatch*)g_currentScene) {
            _sceneMatch->fadeOutThisSceneAndChangeTo(SCENE_BOOST);
            return;
        }
    }
    
    if (typeid(*g_currentScene) == typeid(SceneRace)) {
        if (SceneRace *_sceneRace = (SceneRace*)g_currentScene) {
            _sceneRace->fadeOutThisSceneAndChangeTo(SCENE_BOOST);
            return;
        }
    }
}

void BackendService::bypass_message(bool firstBtnPressed_)
{
    if (g_gameBackend->m_link != "") {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        CCOpenURL::openAndroidMarket();
#else
        CCOpenURL::openURL(g_gameBackend->m_link);
#endif

        return;
    }

    kick_back_to_front(firstBtnPressed_);
}
