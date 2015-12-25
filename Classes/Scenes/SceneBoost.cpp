//
//  SceneBoost.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/22/56 BE.
//
//

#include "SceneBoost.h"
USING_NS_CC;
using namespace std;

#include "../AppMacros.h"

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;
extern CCScene* g_currentScene;

#include "../Entities/PlayerState.h"
#include "../Services/GameContentService.h"
#include "../Services/BackendService.h"
#include "../Services/APIs/APIGameDataGet.h"
#include "../Services/APIs/APIUserGet.h"
#include "../Services/APIs/APIUserStatSet.h"
#include "../Services/APIs/APIOpponentGet.h"

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

extern string gcid, uuid, token;
extern unsigned int currentTime;
extern TargetDevice targetDevice;
extern float sizeMultiplier;
extern bool isPhone;

#include "../Entities/Tutorials/TutFirstRace.h"

#include "../Helpers/FlurryCpp.h"
#include "../Helpers/DeviceInfo.h"
#include "../Helpers/Versioning.h"

#include "../UIs/Dialogs/WaitForRaceToEndDialog.h"
#include "../user.h"
#include "../Helpers/AnalyticsClientWrapper.h"
#include "../Extensions/APKExpansion.h"

/* LOAD_CUSTOM_UUID if defined, defined in Settings.h */

bool SceneBoost::init() {
    
    if (!CCScene::init()) {
        return false;
    }

    modalController = CCModalController::create();
    modalController->setSender(this);
    modalController->retain();
    
    FlurryCpp::logEvent(EVENT_LOADING_GAME_LAUNCH, true);
    
    loadingTime = 0.0f;
    allowLoadingInUpdate = false;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    uuid = CCUserDefault::sharedUserDefault()->getStringForKey("uuid");
    token = CCUserDefault::sharedUserDefault()->getStringForKey("token");

    m_visit_uuid = "";
    
#ifdef LOAD_CUSTOM_UUID
    uuid = LOAD_CUSTOM_UUID;
#endif
    
#ifdef IS_DEBUG_BUILD
//    uuid = "";
#endif
    
    AnalyticsClient::getInstance()->setUserID(uuid);

    CCUserDefault::sharedUserDefault()->setIntegerForKey("override_explore_button_badge_number",
                                                         HUD_OVERRIDE_EXPLORE_BUTTON_BADGE_NUMBER_DISABLE);
    currentTime = 0;

    std::string bgKey = "";

    CCSprite *_bg;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    switch (targetDevice)
    {
        case TD_IPHONE:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            _bg = CCSprite::create("splash/Default.png");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            _bg = CCSprite::create("Default.png");
#endif
            _bg->setRotation(270.0-180);
            break;
        case TD_IPHONE_RETINA:
        {
            if(_screenSize.width==960)
            {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                _bg = CCSprite::create("splash/Default@2x.png");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                _bg = CCSprite::create("Default@2x.png");
#endif
            }
            else
            {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                _bg = CCSprite::create("splash/Default-568h@2x.png");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                _bg = CCSprite::create("Default-568h@2x.png");
#endif
            }
            _bg->setRotation(270.0-180);
            break;
        }
        case TD_IPAD:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            _bg = CCSprite::create("splash/Default-Landscape~ipad.png");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            _bg = CCSprite::create("Default-Landscape~ipad.png");
#endif
            break;
        case TD_IPAD_RETINA:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            _bg = CCSprite::create("splash/Default-Landscape@2x~ipad.png");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            _bg = CCSprite::create("Default-Landscape@2x~ipad.png");
#endif
            break;
    }

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    //!
    //! if you are adding new splash images in the future, just add the image name
    //! to splashes[] with appropriate ratio, the game will pick the best fit
    //!

    struct SplashImage
    {
        SplashImage(float r, std::string s) : ratio(r), name(s) {}
        float ratio;
        std::string name;
    };

    CCSize frameSize = CCSizeMake(APKExpansion::getScreenWidth(), APKExpansion::getScreenHeight());
    const float ratio = (float)APKExpansion::getScreenWidth() / (float)APKExpansion::getScreenHeight();
    const SplashImage splashes[] = { SplashImage(fabsf((4.0f / 3.0f) - ratio), "splash/splash4_3.png"),
                                     SplashImage(fabsf((16.0f / 9.0f) - ratio), "splash/splash16_9.png"),
                                     SplashImage(fabsf((16.0f / 10.0f) - ratio), "splash/splash16_10.png"),
                                   };

#define splash_min(s0, s1) (s0.ratio<s1.ratio?s0:s1)
    const SplashImage& result = splash_min(splashes[0], splash_min(splashes[1], splashes[2]));
#undef splash_min

      CCLog_("Ratio: %f %f %f", ratio, frameSize.width, frameSize.height);

//    std::sort(splashes, splashes + sizeof splashes / sizeof splashes[0], [](SplashImage& a, SplashImage& b)
//    {
//        return a.ratio < b.ratio;
//    });

    CCLog_("Splash screen: %s", result.name.c_str());
    _bg = CCSprite::create(result.name.c_str());
    _bg->setScale(frameSize.height / _bg->getContentSize().height);

#endif

    _bg->setPosition(_screenSize/2);
    addChild(_bg,0,TAG_BG_DEFAULT);

    m_reboost = false;
    boost_step = 0;
    
    loadImageDoneCount = 0;
    loadingPercentComplete = 0;
    animationComplete = false;
    enableProgressBar = false;
    pauseProgressBar = false;
    
#ifdef DEBUG_LOADING
    m_message = CCLabelHelper::createBMFont("init ..", 100.0f, true);
    m_message->setPosition(CCPoint(_screenSize.width*0.5f, _screenSize.height*0.2f));
    this->addChild(m_message, 2);
#endif
    
    fadeInSplash1();
    
    scheduleUpdate();
    
    return true;
    
}

SceneBoost::~SceneBoost() {

    if (modalController) {
        modalController->release();
        modalController = NULL;
    }
    
}

void SceneBoost::fadeInSplash1() {

    //preload any effect sound
    if (!g_gameAudio) {
        g_gameAudio = new GameAudioService();
    }

    //! asynchronous image preload caused a crash on android platform
#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
    preloadImages();
#endif

#ifdef DEBUG_LOADING
    m_message->setString("just preloadImages() ..");
#endif
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("common/transition/cloud.plist");
    
#ifdef DEBUG_LOADING
    m_message->setString("just loaded loadingBar&cloud frames");
#endif
    
    g_gameAudio->preloadEffects(GameAudioService::Generic);
    
    allowLoadingInUpdate = true;
    
    playSplashSound1();
    
    scheduleOnce(schedule_selector(SceneBoost::afterSplash1), 1.5f); //display splash2 for at least 2 seconds

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        //
        // this will hide the native splash screen and replace it with the same one from cocos2dx
        // the native splash screen is necesary for removing the black screen during startup
        //
        APKExpansion::hideSplashscreen();
#endif
}

void SceneBoost::playSplashSound1() {
    
    //stopping all sounds first, in case of being kicked from server
    g_gameAudio->stopAllEffect();
    g_gameAudio->stopMusic();
    
    if (g_gameAudio->isMusicOn()) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(SPLASH_SOUND_2);
    }

}

void SceneBoost::playSplashSound2() {
    

}

void SceneBoost::shuffleHint(){
    
    if (!m_message || g_sceneManager->Hints.size() <= 0) return;
    
    int randomNum = rand()%g_sceneManager->Hints.size();
    m_message->setString(g_sceneManager->Hints[randomNum].c_str());

}

void SceneBoost::afterSplash1() {
    
    enableProgressBar = (modalController->getNStackView() == 0);
    loadingPercentComplete = 0;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
#ifndef DEBUG_LOADING
    m_message = CCLabelHelper::createBMFont("", 70.0f, true);
    m_message->setPosition(CCPoint(_screenSize.width*0.5f,
                                   g_sceneManager->transition->progressBarFrame->getPositionY()
                                   + 70.0f*sizeMultiplier));
    m_message->setAnchorPoint(ccp(0.5f,0.0f)) ;
    m_message->setWidth(_screenSize.width
                        - 160.0f*sizeMultiplier);
    addChild(m_message, 2 );
    shuffleHint();
    schedule(schedule_selector(SceneBoost::shuffleHint), 8.0f);
#endif

    scheduleOnce(schedule_selector(SceneBoost::afterSplash2), 2.0f); //display splash2 for at least 2 seconds
}

void SceneBoost::afterSplash2() {
    
    animationComplete = true;
    
}

void SceneBoost::preloadImages() {
    
    //must use real paths
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    
    //================================================================
    //NOTE:
    //addImageAsync in version 2.1.5 does NOT yet support PVR textures
    //================================================================

    CCTextureCache::sharedTextureCache()->addImageAsync("common/ui/shift_building.png",
                                                        this, callfuncO_selector(SceneBoost::loadImageDone));
    CCTextureCache::sharedTextureCache()->addImageAsync("common/particle_effect/effect_glowing.png",
                                                        this, callfuncO_selector(SceneBoost::loadImageDone));

}

void SceneBoost::loadImageDone() {
    
    loadImageDoneCount++;
    
#ifdef DEBUG_LOADING
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "loadImageDoneCount:%d",loadImageDoneCount);
    m_message->setString(_buffer);
#endif
    
}

void SceneBoost::update(float delta) {
    
    loadingTime += delta;
    
    if (!allowLoadingInUpdate)
        return;
    
    SceneManager *_sceneManager = (SceneManager*)g_sceneManager;
    if (!_sceneManager) {
        CCLog_("SceneBoost::update - SceneManager is uninitialized!");
#ifdef DEBUG_LOADING
        if (m_message)
            m_message->setString("SceneBoost::update - SceneManager is uninitialized!");
#endif
        return;
    }
    
    if (BackendService::isCreated()) {
        if (BackendService::getInstance()->cannot_connect_to_server) {
#ifdef DEBUG_LOADING
            if (m_message)
                m_message->setString("cannot_connect_to_server ..");
#endif
            pauseProgressBar = true;
        }
    } else {
        pauseProgressBar = false;
    }
    
    if (enableProgressBar && !pauseProgressBar) {
        _sceneManager->transition->setProgressBarEnabled(true);
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        
        if (loadingPercentComplete < loadingPercentCompleteTarget) {
            loadingPercentComplete += (LOADING_BAR_SPEED * delta);
        } else {
            loadingPercentComplete = loadingPercentCompleteTarget;
        }
        
        snprintf(_buffer, _buffer_size, "%d%%", (int)loadingPercentComplete);
        _sceneManager->transition->setProgressBarPercent(loadingPercentComplete);
    }
    
    switch (boost_step) {
        case -1: {
            
        }break;
        case 0: {
            
            if (_sceneManager->transition) {
#ifdef DEBUG_LOADING
                m_message->setString("loading contents ..");
#endif
                
                loadingPercentCompleteTarget += 3.0f;
                _sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
            }
            
//            if (_sceneManager->phatTouches) {
//                _sceneManager->phatTouches->enable();
//            }
            
            boost_step = 1;
        
        }break;
        case 1: {
            
            loadingPercentCompleteTarget += 1.0f;
            
            GameContentService::getInstance();
            
#ifdef DEBUG_LOADING
            m_message->setString("initiating audio ..");
#endif
            
            loadingPercentCompleteTarget += 10.0f;
            boost_step = 20;
            
        }break;
        case 20: {

#ifdef DEBUG_LOADING
            m_message->setString("initiating components ..");
#endif
            
            loadingPercentCompleteTarget += 1.0f;
            boost_step = 2;
            
        }break;
        case 2: {
            
            _sceneManager->transition->loadAsset();
            
#ifdef DEBUG_LOADING
            m_message->setString("initiating player state ..");
#endif
            
            loadingPercentCompleteTarget += 1.0f;
            boost_step = 3;
            
        }break;
        case 3: {
            
            boost_step = 5;
            
            if (!PlayerState::isCreated()) {
                PlayerState::getInstance();
            }else {
                PlayerState::getInstance()->clear();
                PlayerState::getInstance()->rawData = "";
                if (gcid != "" &&
                    PlayerState::getInstance()->player.gcid == "") {
                    PlayerState::getInstance()->player.gcid = gcid;
                }
            }

            PlayerState::getOpponent()->clear();
            
#ifdef DEBUG_LOADING
            m_message->setString("logging in to GameCenter ..");
#endif
            
            loadingPercentCompleteTarget += 15.0f;
            
        }break;
        case 4: {
            
            if (m_reboost) {
                m_reboost = false;
                boost_step = 5;
            }
            
        }break;
        case 5: {
            
            BackendService::getInstance()->clearAllRequests();
            BackendService::getInstance()->send(APIUserGet::create(this,
                                                                   SceneBoost::onUserDataRetrieved));

#ifdef DEBUG_LOADING
            m_message->setString("retrieving data ..");
#endif

            boost_step = 60;
            
        }break;
        case 60: {
            
            loadingPercentCompleteTarget = 31.0f + (GameContentService::getInstance()->getPercentageFinished() * 49.0f);
            
        }break;
        case 6: {
            
#ifdef DEBUG_LOADING
            if (g_gameBackend)
                m_message->setString("g_gameBackend not null ..");
            if (g_playerState)
                m_message->setString("g_playerState not null ..");
#endif

            if (!BackendService::isCreated() ||
                !PlayerState::isCreated()) return;
                
            if (BackendService::getInstance()->didRespond)
                loadingPercentCompleteTarget = 80.0f;
                
#ifdef DEBUG_LOADING
            if (BackendService::getInstance()->didRespond)
                m_message->setString("g_gameBackend->didRespond ..");
            if (g_playerState->didParseData())
                m_message->setString("g_playerState->didParseData() ..");
#endif
                
            if (BackendService::getInstance()->didRespond &&
                PlayerState::getInstance()->didParseData()) {
                
                loadingPercentCompleteTarget = 95.0f;
                
                //delay for fade sound must change to 0 when test game
                scheduleOnce(schedule_selector(SceneBoost::delayForFadeSound), 1.5f);
                g_gameAudio->clearEffectFadeOut(1.0);
                boost_step = 7;
            }
            
        }break;
        case 7: {

        }break;
        case 8: {

            g_gameAudio->updateAudioSetting();
            boost_step = 9;
            
        }break;
            
        case 9: {


#ifdef DEBUG_LOADING
            m_message->setString("wait 4 animComplete ..");
#endif
            
            loadingPercentComplete = 100.0f;
            loadingPercentCompleteTarget = 100.0f;
            
            m_message->setVisible(false);
        
            if (SceneManager* _sceneManager = (SceneManager*)g_sceneManager) {
                
                _sceneManager->facebook->tryLoginIfThereIsTokenLoaded();
                
                FlurryCpp::endTimedEvent(EVENT_LOADING_GAME_LAUNCH, NULL);
                
#ifdef DEBUG_SKIP_FIRST_RACE
                g_playerState->checklist(TutFirstRace::key());
#endif
                
#ifdef DEBUG_SKIP_SECOND_RACE
                g_playerState->checklist(TutSecondRacePart1::key());
#endif
                
                CCLog_("loading time:%.3f",loadingTime);
                
                CCDictionary *ccDict = CCDictionary::create();
                ccDict->setObject(CCString::createWithFormat("%.3f",loadingTime), "Loading Time");
                FlurryCpp::logEvent(EVENT_LOADING_FINISH_GAME_LAUNCH, ccDict);
                
                if (!PlayerState::getInstance()->Checklist[TutFirstRace::key()]) {
                    fadeOutButWaitForOpponent(SCENE_RACE, "8b9fea10-7ce2-462d-bca1-f4d54b5f8128");
                    if (_sceneManager->transition) {
                        _sceneManager->transition->fadeSceneOut(_sceneManager);
                    }
                } else {
                    _sceneManager->sceneSelector = SCENE_VILLE;
                    if (_sceneManager->transition) {
                        _sceneManager->transition->fadeSceneOut(_sceneManager);
                    }
                }
                
            }
            
            boost_step = 10;

        }break;
            
        default: {
         
            _sceneManager->transition->setProgressBarEnabled(false);
        
        }break;
    }
    
}

void SceneBoost::delayForFadeSound() {
    
    boost_step = 8;
    
}

void SceneBoost::reload() {
    
    boost_step = -1;
    scheduleOnce(schedule_selector(SceneBoost::do_a_real_reload), DELAY_TIME_FOR_RELOAD);
    
    BackendService::getInstance()->clearAllRequests();
    
#ifdef DEBUG_LOADING
    m_message->setString("reload() wait 2 seconds");
#endif
    
}

void SceneBoost::do_a_real_reload(cocos2d::CCObject *obj) {
    
    m_reboost = true;
    boost_step = 3;
    
#ifdef DEBUG_LOADING
    m_message->setString("reloading - doarealreload");
#endif
    
}

void SceneBoost::fadeOutButWaitForOpponent(int sceneIndex, string oppo_uuid) {
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_opponent_get;
    }
    
    m_visit_uuid = oppo_uuid;
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

#pragma mark - Helper callback
void SceneBoost::fire_opponent_get(void *sender_) {
    
    SceneBoost* _this = (SceneBoost*)g_currentScene;
    if (!_this || !BackendService::isCreated()) return;

    BackendService::getInstance()->cancelAllRequestsInQueue();
    BackendService::getInstance()->send(APIOpponentGet::create(_this->m_visit_uuid,
                                                               _this,
                                                               SceneBoost::onOpponenentDataRetrieved));

}

void SceneBoost::onUserDataRetrieved(void *sender, std::string respn_json) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    if (!_playerState->parseUserData(respn_json.c_str())) return;

    SceneBoost* _this = (SceneBoost*)sender;
    if (!_this) return;

    CREATE_CHAR_BUFFER(_buffer, 256);
    snprintf(_buffer, _buffer_size, "location=getbyip&language=%s&platform=%s %s&device=%s(%s)&version=%s&level=%d&uuid=%s&token=%s",

            DeviceInfo::getLanguage().c_str(), //en
            DeviceInfo::getSystemName().c_str(), //iPhone OS
            DeviceInfo::getSystemVersion().c_str(), //8.1
            DeviceInfo::getModel().c_str(), //iPad Simulator
            DeviceInfo::getName().c_str(), //x86_64
            Versioning::GetBundleVersionShort().c_str(), //1.0.17
            _playerState->player.racing.level, _playerState->player.uuid.c_str() ,_playerState->player.token.c_str());

    BackendService::getInstance()->send(APIUserStatSet::create(_buffer,
                                                               true,
                                                               _this,
                                                               SceneBoost::onPurchaseStatReceived));
    
    
    if (_playerState->isBeingRaced()) {
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        WaitForRaceToEndDialog *_dialog = WaitForRaceToEndDialog::create(_this->modalController,
                                                                         _playerState->getSecondsLeftToPlayAgain());
        _dialog->setPosition(_screenSize*0.5f);
        _this->addChild(_this->modalController, 100);
        _this->modalController->onCloseAll = SceneBoost::onCountDownDone;
        _this->modalController->pushView(_dialog);
        _this->enableProgressBar = false;
        g_sceneManager->transition->setProgressBarEnabled(false);
        return;
    }
    
    _this->boost_step = 6;
    
    AnalyticsClient::getInstance()->startSession(StartSessionOptionsSceneBoost);
    
}

void SceneBoost::onPurchaseStatReceived(void *sender, std::string respn_json) {
    
    PlayerState *_playerState = PlayerState::getInstance();
    
    //JsonAllocator _allocator;
    Json* _root = Json_create(respn_json.c_str());
    _playerState->PurchaseStat.parseJSON(_root);
    Json_dispose(_root);
    
    _playerState->PurchaseStat.print();
    
    GameContentService::getInstance()->pullLiveGameConfig();
    
}

void SceneBoost::onOpponenentDataRetrieved(void *sender, string resp_str) {
    
    SceneBoost *_this = (SceneBoost*)g_currentScene;
    if (!_this || typeid(*_this) != typeid(SceneBoost)) return;
    
    PlayerState::getOpponent()->parseOppoData(resp_str.c_str());
    
    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);
    
}

void SceneBoost::onCountDownDone(void *sender) {

    SceneBoost *_this = (SceneBoost*)sender;
    if (!_this) return;
    
    _this->modalController->onCloseAll = NULL;
    _this->boost_step = 3;
    
    _this->enableProgressBar = true;
    g_sceneManager->transition->setProgressBarEnabled(true);
    
}
