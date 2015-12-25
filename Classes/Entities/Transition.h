//
//  Transition.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/56 BE.
//
//

#ifndef __Cheetah__Transition__
#define __Cheetah__Transition__

#include "cocos2d.h"
#include "../Settings.h"
#include "../Extensions/CCLabelHelper.h"
#include "../Extensions/CCNikProgressBar.h"

#define FS_SPEED_DEFAULT    0.1f
#define TRANSITION_CLOUD_MAX    20

class TransitionCloud: public cocos2d::CCSprite {
    
//    cocos2d::CCSprite*          m_cloudSprite;
    cocos2d::CCPoint            m_targetPosition;
    float                       m_movingSpeed;
    
public:
    
    static TransitionCloud *create(void *preferenceRootParent_);
    virtual bool init();
    
    void updateMoving();
    void setPreferenceRootParent(void *preferenceRootParent);
    void setTargetPosition(cocos2d::CCPoint pos);
    void setMovingSpeed(float spd);
    
};

class Transition: public cocos2d::CCNode {
    
    cocos2d::CCLayerColor*      layerFade;
    
    TransitionCloud*            m_cloud[TRANSITION_CLOUD_MAX];
    bool                        m_didAssetLoad = false;
    bool                        m_isFinished;
    
    void _onSceneFadedIn(cocos2d::CCObject* sender);
    void _onSceneFadedOut(cocos2d::CCObject* sender);
    
public:
    
    void                        *sceneManager       = NULL;
    
    float                       fadeSpeed;
    
    cocos2d::CCLabelBMFont      *loadingMessage     = NULL;
    CCNikProgressBar            *progressBar        = NULL;
    CCScale9Sprite              *progressBarFrame   = NULL;
    
    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(Transition);
    
    bool loadAsset();
    bool didAssetLoad();
    bool isFinished() { return m_isFinished; }
    
    void setProgressBarEnabled(bool enabled_);
    void setProgressBarPercent(float percent_);
    
    void fadeSceneIn(cocos2d::CCObject* sender);
    void fadeSceneOut(cocos2d::CCObject* sender);
    
    void (*OnSceneFadedIn)(void* sender);
    void (*OnSceneFadedOut)(void* sender);
    
};

#endif /* defined(__Cheetah__Transition__) */
