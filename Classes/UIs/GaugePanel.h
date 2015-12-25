//
//  GaugePanel.h
//  Cheetah
//
//  Created by Kaweerut on 2/6/57 BE.
//
//

#ifndef __Cheetah__GaugePanel__
#define __Cheetah__GaugePanel__

#include "cocos2d.h"
USING_NS_CC;

#define     GaugePanel_SPEED            700.0f;


enum GaugeRESULT {
    GaugeRESULT_NONE,
    GaugeRESULT_NORMAL,
    GaugeRESULT_GOOD
};


class GaugePanel: public cocos2d::CCNode {

    CCRenderTexture* createPatternSprite(int width, int height) const;

    
public:
    
    float                               _scaleFactor;
    
    CCArray                             *_gameDatas;
    CCDictionary                        *_barData;
    
    float                               _currentSpeed;
    bool                                _isIncrease;
    
    CCSprite                            *_gaugeBarBG;
    CCLayerColor                        *_gaugeBarFrame;
    CCSprite                            *_pinSprite;
    
    CCSprite                            *cursorSprite;
    CCNode*                             m_frame;
    
    float                               _frameWidth;
    
    
    
    static GaugePanel* create(std::string preselectedData);
    virtual bool init(std::string preselectedData);
    void adjustScaleSize();
    virtual void update(float delta);
    ~GaugePanel();
//    virtual void cleanup();
    
    
    CCDictionary* createGameData(std::string preselectedData);
    void show();
    void hide();
    
    void start();
    void stop();
    bool isReady();
    void resetData();
    GaugeRESULT getCurrentPinResult();
    void restart();
    
    void animateCursor();
    void stopAnimatedCursor();
    int x;
    int y;
    
};

#endif /* defined(__Cheetah__GaugePanel__) */
