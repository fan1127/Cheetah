//
//  SceneManager.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/22/56 BE.
//
//

#include "SceneManager.h"
USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

#include <spine/Json.h>

#include "Extensions/CCLocalNotification/CCLocalNotification.h"
#include "Helpers/PushNotifications/PushNotifications.h"
#include "AppMacros.h"

SceneManager *g_sceneManager = NULL;
CCScene *g_currentScene = NULL;

#include "Scenes/SceneBoost.h"
#include "Scenes/SceneVille.h"
#include "Scenes/SceneVisit.h"
#include "Scenes/SceneMatch.h"
#include "Scenes/SceneRace.h"
#include "Scenes/SceneWorldMap.h"

#include "Services/GameAudioService.h"
GameAudioService *g_gameAudio = NULL;

#include "Services/GameContentService.h"

#include "Entities/PlayerState.h"
#include "Helpers/SocialPlatform/SocialPlatform.h"

#include "Services/BackendService.h"
#include "Services/APIs/APIUserFacebookTie.h"

#include "Settings.h"

#include "Helpers/FlurryCpp.h"
#include "Helpers/GaiCpp.h"

#include "Helpers/StringHelper.h"
#include "Services/APIs/APIUserStatSet.h"
#include "UIs/BlockingLayer.h"

#include "Helpers/AnalyticsClientWrapper.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <android/log.h>
#endif

//CCPoint screenFactor;
string gcid, uuid, token, auuid, race_id;
string gcDisplayAlias;
unsigned int currentTime = 0;
double currentTimeClient = 0;
float margin;

CCScene *SceneManager::scene() {
    
    CCScene *scene = CCScene::create();
    g_sceneManager = SceneManager::create();
    scene->addChild(g_sceneManager);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    g_sceneManager->facebook->assignSessionStateCallback(g_sceneManager, SceneManager::fbSessionStateCallback);
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    return scene;
    
}

bool SceneManager::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    sceneSelector = SCENE_BOOST;
    sceneSelected = (SceneList)-1;

    margin = _screenSize.height*0.01f;
    
    transition = Transition::create();
    transition->fadeSpeed = 0.25f;
    transition->OnSceneFadedOut = SceneManager::switchScene;
    transition->sceneManager = this;
    this->addChild(transition, 999);
    
    loadingLayer = NULL;
    m_blockingLayer = NULL;

    notification = MessageNotification::create();
    this->addChild(notification, 1000);

    facebook = new FacebookController();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    facebook->assignSharedInstance(facebook);
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    
    pendingMsg = CCArray::create();
    pendingMsg->retain();
    
    sharedSkeletonData = CCShareSkeletonAnimation::sharedSkeletonAnimation();
    
    enter_background_timestamp = 0;
    
    loadHintsFromFile(kSceneManagerHintsPath, kSceneManagerHintsFilename);
    
    switchScene(this);
    
    schedule(schedule_selector(SceneManager::updateAtSecond), 1.0f);
    schedule(schedule_selector(SceneManager::updateForDeclaration), 30);
    scheduleUpdate();

    return true;
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void SceneManager::fbSessionStateCallback(void* sender, bool result)
{
	g_sceneManager->onFBLoggedIn(g_sceneManager, result);
}
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

void SceneManager::testSpineLoaderPerformance()
{
    sharedSkeletonData->clear();
    
#if DEBUG_ENABLE_PROFILERS
    CCProfiler::sharedProfiler()->createAndAddTimerWithName("Profiler_loadPets");
    CCProfilingBeginTimingBlock("Profiler_loadPets");
#endif

    sharedSkeletonData->getSkeleton("common/pet/", "bear1");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bear1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "bull1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_6_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_6_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_7_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cat1_7_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_1_3");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "cow1_2_3");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_10_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_10_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_6_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_6_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_7_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_7_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_8_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_8_2");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_9_1");
    sharedSkeletonData->getSkeleton("common/pet/", "deer1_9_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_1_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_2_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_3_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_4_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_5_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_6_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_6_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_6_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_7_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_7_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_7_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_8_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_8_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog1_8_3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog2_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dog3_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "dragon1_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "fox1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_6_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_6_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_7_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse1_7_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_3_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_4_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_4_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_5_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse2_5_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse3");
    sharedSkeletonData->getSkeleton("common/pet/", "horse3_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse3_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse3_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse3_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse4");
    sharedSkeletonData->getSkeleton("common/pet/", "horse4_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse4_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "horse4_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "horse4_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_1_3");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "pig1_2_3");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_1_3");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "sheep1_2_3");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_1_1");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_1_2");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_2_1");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_2_2");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_3_1");
    sharedSkeletonData->getSkeleton("common/pet/", "wolf1_3_2");
    
#if DEBUG_ENABLE_PROFILERS
    CCProfilingEndTimingBlock("Profiler_loadPets");
    CCProfiler::sharedProfiler()->displayTimers();
#endif
}

SceneManager::~SceneManager() {
    
    pendingMsg->release();
    pendingMsg = NULL;
    
    unschedule(schedule_selector(SceneManager::updateAtSecond));
    
    if (sharedSkeletonData) {
        sharedSkeletonData = NULL;
    }
    
    if (notification) {
        notification->unscheduleAllSelectors();
        notification->removeFromParentAndCleanup(true);
        notification = NULL;
    }
    
    if (facebook) {
        delete facebook;
        facebook = NULL;
    }
    
    if (transition) {
        transition->unscheduleAllSelectors();
        transition->removeFromParentAndCleanup(true);
        transition = NULL;
    }
    
}

bool SceneManager::loadHintsFromFile(const char *path, const char *filename) {
    
    string _hintsString = CCUserDefault::sharedUserDefault()->getStringForKey("hints");
    if (_hintsString != "") {
        //JsonAllocator allocator;
        if (Json *_root = Json_create(_hintsString.c_str())) {
            if (Json *_hints_json = Json_getItem(_root, "hints")) {
                Hints.clear();
                int n = Json_getSize(_hints_json);
                for (int i=0; i<n; i++) {
                    Json *_hint_json = Json_getItemAt(_hints_json, i);
                    Hints.push_back(Json_getString(_hint_json, "title", ""));
                }
            }
        }
    }
    
    if (Hints.size() > 0)  return true;
    
    unsigned long _fileSize = 0;
    unsigned char *_fileContents = NULL;
    
    string _fullPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(filename, path);
    _fileContents = CCFileUtils::sharedFileUtils()->getFileData(_fullPath.c_str(), "r", &_fileSize);
    
    string _stringContent;
    for (int i=0; i<_fileSize; i++) {
        _stringContent.push_back(_fileContents[i]);
    }

    Hints = StringHelper::split((const char*)_stringContent.c_str(), '\n');
    
    CCLog_("#######################");
    CCLog_("%s",_stringContent.c_str());
    CCLog_("#######################");
    CCLog_("HintsHintsHints:%lu",Hints.size());
    for (vector<string>::iterator it=Hints.begin(); it!=Hints.end(); ++it) {
        CCLog_("%s", (*it).c_str());
    }
    
    return true;
    
}

void SceneManager::update(float delta) {
    
#if IS_DEBUG_BUILD
    //CCLogMemUsage();
#endif
    
    currentTimeClient += (double)delta;
    
    if (transition) {
        transition->update(delta);
    }
    
    if (notification) {
        notification->update(delta);
    }
    
    if (!m_didLoad) {
        m_didLoad = true;
        switchScene(this);
    }
    
    if (g_gameAudio) {
        g_gameAudio->update();
    }
    
}



void SceneManager::restart(void *sender) {
    
    SceneManager *_this = (SceneManager*)sender;
    if (!_this) {
        CCLog_("Error #1 - SceneManager is unitialized!\n");
        return;
    }

    BackendService::getInstance()->clearAllRequests();
    CCPopOverDialog::dismissAllPopovers();
    
    _this->sceneSelector = SCENE_BOOST;
    _this->sceneSelected = (SceneList)-1;
    _this->switchScene(_this);
    
}

void SceneManager::switchScene(void *sender) {
    
    SceneManager *_this = (SceneManager*)sender;
    if (!_this) {
        CCLog_("Error #1 - SceneManager is unitialized!\n");
        return;
    }
    
    if (_this->sceneSelector != _this->sceneSelected) {
    
        if (_this->loadingLayer) {
            _this->loadingLayer->hide();
            _this->loadingLayer = NULL;
        }

        if (_this->m_blockingLayer) {
            _this->m_blockingLayer->hide();
            _this->m_blockingLayer = NULL;
        }

        if (g_currentScene) {
            g_currentScene->removeFromParent();
            g_currentScene = NULL;
        }
        
        if (g_gameAudio) {
            g_gameAudio->stopAllEffect();
            g_gameAudio->stopMusic();
        }
        
        GameContentService::getInstance()->removeAssetContent(_this->sceneSelected);
        GameContentService::getInstance()->loadAssetContent(_this->sceneSelector);
        
        SocialPlatform::getInstance()->setAuthenticationCallback(_this, SceneManager::onGameCenterAuthenticated);
        SocialPlatform::getInstance()->authenticate();
        
        switch (_this->sceneSelector) {
            case SCENE_BOOST: {
                g_currentScene = SceneBoost::create();
                if (_this->sceneSelected != (SceneList)-1) {
                    if (SceneBoost *_currentScene = (SceneBoost*)g_currentScene) {
                        _currentScene->reload();
                    }
                }
            }break;
            case SCENE_VILLE:
            {
                g_currentScene = SceneVille::create();
            }
                break;
            case SCENE_VISIT:
            case SCENE_VISIT2:
                g_currentScene = SceneVisit::create();
                break;
            case SCENE_WORLDMAP:
                if (PlayerState::isCreated()) {
                    PlayerState::getInstance()->cleanAllEffects();
                }
                g_currentScene = SceneWorldMap::create();
                break;
            case SCENE_MATCH:
            case SCENE_MATCH2:
                g_currentScene = SceneMatch::create();
                break;
            case SCENE_RACE:
            case SCENE_RACE2:
                g_currentScene = SceneRace::create();
                break;
        }
        
        if (g_currentScene) {
            _this->addChild(g_currentScene, 0);
            _this->sceneSelected = _this->sceneSelector;
            
            
            CCDictionary *ccDict = CCDictionary::create();
            switch (_this->sceneSelected) {
                case SCENE_VILLE:
                {
                    ccDict->setObject(CCString::create(EVENT_PARAM_VALUE_SCENE_VILLE), "Scene");
                    GaiCpp::reportScreenView(EVENT_PARAM_VALUE_SCENE_VILLE);
                }
                    break;
                    
                case SCENE_VISIT:
                {
                    ccDict->setObject(CCString::create(EVENT_PARAM_VALUE_SCENE_VISIT), "Scene");
                    GaiCpp::reportScreenView(EVENT_PARAM_VALUE_SCENE_VISIT);
                }
                    break;
                    
                case SCENE_WORLDMAP:
                {
                    ccDict->setObject(CCString::create(EVENT_PARAM_VALUE_SCENE_WORLDMAP), "Scene");
                    GaiCpp::reportScreenView(EVENT_PARAM_VALUE_SCENE_WORLDMAP);
                }
                    break;
                    
                case SCENE_MATCH:
                {
                    ccDict->setObject(CCString::create(EVENT_PARAM_VALUE_SCENE_MATCH), "Scene");
                    GaiCpp::reportScreenView(EVENT_PARAM_VALUE_SCENE_MATCH);
                }
                    break;
                    
                case SCENE_RACE:
                {
                    ccDict->setObject(CCString::create(EVENT_PARAM_VALUE_SCENE_RACE), "Scene");
                    GaiCpp::reportScreenView(EVENT_PARAM_VALUE_SCENE_RACE);
                }
                    break;
                    
                default:
                    break;
            }
            FlurryCpp::logEvent(EVENT_SCENE_CHANGE, ccDict);
            
            if (_this->transition->didAssetLoad()) {
                _this->transition->fadeSceneIn(_this);
            }
        }
        
    }

}

#pragma mark - Local Notification Functions

void SceneManager::scheduleLocalNotifications() {
    
    //if g_playerState is not created, SceneBoost has not finished running completely yet
    if (!PlayerState::isCreated()) return;
    PlayerState *_playerState = PlayerState::getInstance();

    //! notifications are disabled
//    if (!PushNotifications::Instance().isEnabled())
//    {
//        return;
//    }

    //CREATE_CHAR_BUFFER(_buffer, 128);
    CREATE_CHAR_BUFFER(_buffer, 128);
    int count = 0;
    
    CCLocalNotification::cancelAll();
    
    // schedule notification for buildings
    for (vector<Building*>::iterator it=_playerState->Buildings.begin(); it!=_playerState->Buildings.end(); ++it) {
        Building* _building = (Building*)(*it);
        if (_building->pbVo.status == BUILDING_STATUS_CONSTRUCT)
        {
            snprintf(_buffer, _buffer_size, GameContentService::getInstance()->Word["local_notification_building_build_finish"].c_str(),
                    _building->bVo.title.c_str());

            CCLocalNotification::show(_buffer, _building->getSecondsLeftToFinish(), count);
            count++;
        }
        else if (_building->pbVo.status == BUILDING_STATUS_UPGRADE)
        {
            snprintf(_buffer, _buffer_size, GameContentService::getInstance()->Word["local_notification_building_upgrade_finish"].c_str(),
                    _building->bVo.title.c_str(),
                    _building->pbVo.level+1);

            CCLocalNotification::show(_buffer, _building->getSecondsLeftToFinish(), count);
            count++;
        }
    }
    
    // schedule notification for abilities
    for (map<string, Ability*>::iterator it=_playerState->Abilities.begin(); it!=_playerState->Abilities.end(); ++it) {
        Ability* _ability = it->second;
        if (!_ability || _ability->paVo.status != kAbilityIsUpgrading) continue;
        snprintf(_buffer, _buffer_size, GameContentService::getInstance()->Word["local_notification_ability_upgrade_finish"].c_str(),
                _ability->aVo.title.c_str(),
                _ability->paVo.level+1);
        CCLocalNotification::show(_buffer, _ability->getSecondsLeftToFinish(), count);
        count++;
    }
    
    // schedule notification for pets
    int _seconds_to_full_energy = 0;
    int _seconds_to_fully_hungry = 0;
    for (vector<Pet*>::iterator it=_playerState->Pets.begin(); it!=_playerState->Pets.end(); ++it) {
        Pet* _pet = (Pet*)(*it);
        if (_pet->ppVo.located_in == "") continue;
        int _to_full_energy = _pet->getSecondsLeftToFullEnergy();
        if (_to_full_energy > 0) {
            if (_seconds_to_full_energy < _to_full_energy) {
                _seconds_to_full_energy = _to_full_energy;
            }
        }else {
            int _to_fully_hungry  = _pet->getSecondsLeftToHungerDrain();
            if (_to_fully_hungry  > 0) {
                if (_seconds_to_fully_hungry < _to_fully_hungry) {
                    _seconds_to_fully_hungry = _to_fully_hungry;
                }
            }
        }
    }
    if (_seconds_to_full_energy > 0) {
        CCLocalNotification::show(GameContentService::getInstance()->Word["local_notification_all_pets_are_ready"].c_str(),
                                  _seconds_to_full_energy, count);
        count++;
    }
    if (_seconds_to_fully_hungry > 0) {
        CCLocalNotification::show(GameContentService::getInstance()->Word["local_notification_all_pets_are_hungry"].c_str(),
                                  _seconds_to_fully_hungry, count);
        count++;
    }
    
    // scehdule notification for merchant
    if (Merchant::isIntroduced()) {
        CCLocalNotification::show(GameContentService::getInstance()->Word["local_notification_merchat_is_here"].c_str(),
                                  Merchant::getTimeMerchantWillAppear(), count);
        count++;
    }
}

void SceneManager::unscheduleLocalNotification() {
    
    CCLocalNotification::cancelAll();
    
}

#pragma mark - Update

void SceneManager::updateAtSecond() {
    
    if (sceneSelector == SCENE_WORLDMAP ||
        sceneSelector == SCENE_MATCH ||
        sceneSelector == SCENE_RACE) {
        if (g_currentScene && PlayerState::isCreated()) {
            PlayerState::getInstance()->updateBuildingsTimer(1.0f);
        }
    }
    
}

void SceneManager::updateForDeclaration() {
    
    std::string _msg = BackendService::getInstance()->getDeclarationMessage();
    if (_msg == "") return;
    
    notification->addMessage(MSGN_TYPE_WARNING, _msg);
    if (BackendService::getInstance()->getDeclarationInterval() <= 0) return;
    
    unschedule(schedule_selector(SceneManager::updateForDeclaration));
    schedule(schedule_selector(SceneManager::updateForDeclaration),
             BackendService::getInstance()->getDeclarationInterval());
    
}

#pragma mark - LoadingLayer

bool SceneManager::showLoading() {
    
    if (loadingLayer) return false;
    
    loadingLayer = LoadingLayer::create();
    loadingLayer->show();
    
    return true;
}

void SceneManager::hideLoading() {
    
    if (!loadingLayer) return;

    loadingLayer->hide();
    loadingLayer = NULL;
    
}

bool SceneManager::showBlockingLayer()
{
    if (m_blockingLayer) return false;

    m_blockingLayer = BlockingLayer::create();
    m_blockingLayer->show();

    return true;
}

void SceneManager::hideBlockingLayer()
{
    if (!m_blockingLayer) return;

    m_blockingLayer->hide();
    m_blockingLayer = NULL;
}

#pragma mark - PlaceHolder

void SceneManager::dismissPlaceHolder() {
    
    SceneVille *_scene = (SceneVille*)g_currentScene;
    if (!_scene || typeid(*_scene) != typeid(SceneVille)) return;
    
    IsoMapBuilding *_isoMapBuilding = (IsoMapBuilding*)_scene->isoMap;
    if (!_isoMapBuilding || typeid(*_isoMapBuilding) != typeid(IsoMapBuilding)) return;
    
    _isoMapBuilding->placingStuffFromPlaceHolder(true, (int)BuildingBuildTypeNormal);
    
}

#pragma mark - Facebook

void SceneManager::onFBLoggedIn(void* sceneManager, bool logged_in) {
    
    SceneManager* _this = (SceneManager*)sceneManager;
    if (!_this || !_this->facebook || !logged_in) return;
  
    CCLog_("logged in = %d", (int)logged_in);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->loginSuccessful();
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->fetchUserDetails(false, NULL, SceneManager::onFBUserInfoReceived);
    _this->facebook->getInstalledFriends(NULL, SceneManager::onFBInstalledFriendRetrieved);
}

void SceneManager::onFBLoggedIn(void* sceneManager, bool logged_in, bool rewardGem) {
    
    SceneManager* _this = (SceneManager*)sceneManager;
    if (!_this || !_this->facebook || !logged_in) return;
    
    CCLog_("logged in = %d", (int)logged_in);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->loginSuccessful();
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->fetchUserDetails(rewardGem, NULL, SceneManager::onFBUserInfoReceived);
    _this->facebook->getInstalledFriends(NULL, SceneManager::onFBInstalledFriendRetrieved);
}

void SceneManager::onFBLoggedIn(void* sceneManager, void* sender, bool logged_in, void (*callback)(void*, bool)) {
    
    SceneManager* _this = (SceneManager*)sceneManager;
    if (!_this || !_this->facebook || !logged_in) return;
    
    CCLog_("logged in = %d", (int)logged_in);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->loginSuccessful();
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _this->facebook->fetchUserDetails(false, NULL, SceneManager::onFBUserInfoReceived);
    _this->facebook->getInstalledFriends(sender, callback);

}

void SceneManager::onFBUserInfoReceived(void* sender, bool success, bool rewardGem)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	__android_log_print(ANDROID_LOG_INFO, "CUSTOM_TAG", "Fetch user callback");
#endif

    if (success)
    {
        CCLog_("-------------------------------------------");
        CCLog_("fbid=%s", g_sceneManager->facebook->user.fbid.c_str());
        CCLog_("name=%s", g_sceneManager->facebook->user.name.c_str());
        CCLog_("email=%s", g_sceneManager->facebook->user.email.c_str());
        CCLog_("gender=%s", g_sceneManager->facebook->user.gender.c_str());
        CCLog_("age_range=%s", g_sceneManager->facebook->user.age_range_min.c_str());
        CCLog_("rewardGem_=%d", rewardGem);
        if (g_sceneManager->sceneSelector != SCENE_BOOST) {
            BackendService::getInstance()->send(APIUserFacebookTie::create(g_sceneManager->facebook->user.fbid,
                                                                           g_sceneManager->facebook->user.email,
                                                                           g_sceneManager->facebook->user.gender,
                                                                           g_sceneManager->facebook->user.age_range_min,
                                                                           rewardGem,
                                                                           sender,
                                                                           SceneManager::onUserFacebookTied));
        }
        
        CREATE_CHAR_BUFFER(_buffer, 128);
        snprintf(_buffer, _buffer_size, "fbid=%s&gender=%s&age=%s",
                g_sceneManager->facebook->user.fbid.c_str(),
                g_sceneManager->facebook->user.gender_full.c_str(),
                g_sceneManager->facebook->user.age_range_min.c_str());
        

        
        BackendService::getInstance()->send(APIUserStatSet::create(_buffer));
        
        CScene *_scene = (CScene*)g_currentScene;
        if (!_scene || !_scene->hudPanel) return;
        _scene->hudPanel->updateBadgeAchievement();
        
    }
    
}

void SceneManager::onUserFacebookTied(void* sender, string json_str) {
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    PlayerState::getInstance()->parseUserBalance(_root); //user can earn gem from FBLogin only once
    PlayerState::getInstance()->parseCurrentTime(_root);
    PlayerState::getInstance()->parseUserInfo(_root);
    PlayerState::getInstance()->parseUserProfile(_root);
    Json_dispose(_root);
    
}

void SceneManager::onFBInstalledFriendRetrieved(void *sender, bool success) {
 
    PlayerState *_playerState = PlayerState::getInstance();
    
    string _param = "[";
    string _connect = "";
    for (int i=0; i<g_sceneManager->facebook->installed_friends.size(); i++) {
        FacebookUser _friend = g_sceneManager->facebook->installed_friends[i];
        if (_friend.fbid != "") {
            _param.append(_connect);
            _param.append("\"");
            _param.append(_friend.fbid);
            _param.append("\"");
            _connect = ",";
        }
    }
    _connect = "]";
    _param.append(_connect);
    _playerState->fbFriendsRawData.clear();
    _playerState->fbFriendsRawData.append(_param);
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    snprintf(_buffer, _buffer_size, "total_facebook_friend_in_game=%d&total_facebook_friend=%d",
            (int)g_sceneManager->facebook->installed_friends.size(),
            (int)g_sceneManager->facebook->total_friends);
    
    BackendService::getInstance()->send(APIUserStatSet::create(_buffer));
    
}

#pragma mark - GameCenterManagerDelegate

void SceneManager::onGameCenterAuthenticated(void *sender, bool success) {
    
    if (!success) {
        gcid = "";
        gcDisplayAlias = "";
        return;
    }
    
    gcid = SocialPlatform::getInstance()->getPlayerID();
    gcDisplayAlias = SocialPlatform::getInstance()->getDisplayAlias();
    SceneVille* _sceneVille = (SceneVille*)g_currentScene;

    if (!_sceneVille || typeid(*g_currentScene) != typeid(SceneVille)) return;
    _sceneVille->processGameCenter();
    
}

#pragma mark - Game Manager Callback

void SceneManager::onMapVillagesRetrieved(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();
    
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseCurrentTime(_root);
    _playerState->parseChecklistData(_root);
    _playerState->parseUserState(_root);
    _playerState->parseUserBalance(_root);
    _playerState->parseItemData(_root);
    _playerState->parsePetData(_root);
    _playerState->parseWorldMapVillages(_root);
    Json_dispose(_root);
    
    CScene* _scene = (CScene*)g_currentScene;
    if (!_scene) return;
    
    if (_scene->hudPanel) {
        _scene->hudPanel->updateHUD();
        _scene->hudPanel->updateAllResourceBars();
    }
    
    if (typeid(*g_currentScene) == typeid(SceneWorldMap)) {
        SceneWorldMap* _sceneWorldMap = (SceneWorldMap*)g_currentScene;
        _sceneWorldMap->worldMap->updateNewVillage(resp_str);
        _sceneWorldMap->worldMap->showPriceUnlockMap();
    }
}

void SceneManager::onPetRetrieved(void *sender, std::string resp_str) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseChecklistData(_root);
    _playerState->parseUserRaceData(_root);
    _playerState->parsePetData(_root);
    _playerState->parseBuildingData(_root);
    Json_dispose(_root);
}

void SceneManager::purgeCacheData()
{
    CCLabelBMFont::purgeCachedData();
    if (CCDirector::sharedDirector()->getOpenGLView()){
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
    }
    CCFileUtils::sharedFileUtils()->purgeCachedEntries();
}

void SceneManager::cleanAfterTutorial()
{
    CCLog_("cleanTutorial");
    g_gameAudio->unloadAllEffect();
    sharedSkeletonData->clear();
    SceneManager::purgeCacheData();
}


