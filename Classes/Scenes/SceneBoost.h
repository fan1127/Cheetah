//
//  SceneBoost.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/22/56 BE.
//
//

#ifndef __Cheetah__SceneBoost__
#define __Cheetah__SceneBoost__

#include "CScene.h"

#define TAG_BG_DEFAULT 1
#define TAG_BG_SPLASH_1 2
#define TAG_BG_SPLASH_2 3

#define LOADING_BAR_SPEED 20.0f //percent per second
#define PROGRESS_BAR_SPEED 550.6f //second to specified percent

#define DELAY_TIME_FOR_RELOAD   2.0f

class SceneBoost: public CScene {

    std::string m_visit_uuid;
    bool m_reboost;
    void delayForFadeSound();
    
    int loadImageDoneCount;
    float loadingPercentComplete;
    float loadingPercentCompleteTarget;
    bool animationComplete;
    bool enableProgressBar;
    bool pauseProgressBar;
    
    bool allowLoadingInUpdate;
    float loadingTime;
    
protected:
    
    int boost_step;
    
    void fadeInSplash1();
    void afterSplash1();
    void afterSplash2();
    void playSplashSound1();
    void playSplashSound2();
    void preloadImages();
    void loadImageDone();
    
public:
    
    cocos2d::CCLabelBMFont *m_message;
    
    virtual bool init();
    virtual void update(float);
    CREATE_FUNC(SceneBoost);
    virtual ~SceneBoost();
    
    void reload();
    void shuffleHint();
    void do_a_real_reload(CCObject* obj);
    void fadeOutButWaitForOpponent(int sceneIndex, std::string oppo_uuid);
    static void fire_opponent_get(void *sender);
    
    static void onUserDataRetrieved(void *sender, std::string respn_json);
    static void onPurchaseStatReceived(void *sender, std::string respn_json);
    static void onOpponenentDataRetrieved(void* sender, std::string resp_json);
    
    static void onCountDownDone(void *sender);
    
};

#endif /* defined(__Cheetah__SceneBoost__) */
