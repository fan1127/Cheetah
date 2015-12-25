//
//  SceneManager.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/22/56 BE.
//
//

#ifndef __Cheetah__SceneManager__
#define __Cheetah__SceneManager__

#include "cocos2d.h"

enum SceneList {
    SCENE_BOOST,
    SCENE_VILLE,
    SCENE_VISIT,
    SCENE_VISIT2,
    SCENE_WORLDMAP,
    SCENE_MATCH,
    SCENE_MATCH2,
    SCENE_RACE,
    SCENE_RACE2,
};

#include "Entities/Transition.h"
#include "UIs/LoadingLayer.h"
#include "Extensions/CCShareSkeletonAnimation.h"
#include "UIs/MessageNotification.h"
#include "Extensions/Facebook/FacebookController.h"

#define kSceneManagerHintsPath        "text/"
#define kSceneManagerHintsFilename    "hints.txt"

#define IS_INSTANCE_OK(__INST__,__TYPE__) (typeid(*__INST__) == typeid(__TYPE__))

class BlockingLayer;
class SceneManager: public cocos2d::CCLayer {
    
    bool m_didLoad;
    BlockingLayer *m_blockingLayer;

public:

    void testSpineLoaderPerformance();

    //Common
    std::vector<std::string>                    Hints;
    
    //Scene
    static cocos2d::CCScene *scene();
    
    //Property
    Transition*             transition;
    LoadingLayer*           loadingLayer;
    MessageNotification*    notification;
    FacebookController*     facebook;
    cocos2d::CCArray*       pendingMsg;
    SceneList               sceneSelector, sceneSelected;
    
    cocos2d::extension::CCShareSkeletonAnimation *sharedSkeletonData;
    
    long int                enter_background_timestamp;
    
    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(SceneManager);
    virtual ~SceneManager();
    
    static void restart(void* sender);
    static void switchScene(void *sender);
    
    bool loadHintsFromFile(const char *path, const char *filename);
    
    void scheduleLocalNotifications();
    void unscheduleLocalNotification();
    
    void updateAtSecond();
    void updateForDeclaration();
    
    bool showLoading();
    void hideLoading();

    bool showBlockingLayer();
    void hideBlockingLayer();
    
    void dismissPlaceHolder();
    
    // Facebook
    static void onFBCreated(bool isLoggedIn);
    static void onFBLoggedIn(void* sceneManager, bool logged_in);
    static void onFBLoggedIn(void* sceneManager, bool logged_in, bool rewardGem);
    static void onFBLoggedIn(void* sceneManager, void* sender, bool logged_in, void (*callback)(void*, bool));
    static void onFBUserInfoReceived(void* sender, bool success, bool rewardGem);
    static void onUserFacebookTied(void* sender, std::string json_str);
    static void onFBInstalledFriendRetrieved(void* sender, bool success);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    static void fbSessionStateCallback(void* sender, bool logged_in);
#endif //End of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    void load_game_dialog();
    
    static void onGameCenterAuthenticated(void* sender, bool success);
    static void onMapVillagesRetrieved(void* sender, std::string resp_str);
    static void onPetRetrieved(void *sender, std::string resp_str);

    static void purgeCacheData();
    
    // tutorial cleanup - release unused spines and images
    void cleanAfterTutorial();
    
};

#endif /* defined(__Cheetah__SceneManager__) */
