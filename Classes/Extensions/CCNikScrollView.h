//
//  CCNikScrollView.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/4/2557 BE.
//
//

#ifndef __Cheetah__CCNikScrollView__
#define __Cheetah__CCNikScrollView__

#include "cocos2d.h"

#include "CCNikScrollViewButton.h"

USING_NS_CC;

class CCNikScrollView;

class CC_DLL CCNikScrollViewTouchDelegate
{
public:
    
    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) { return false; };
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {};
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {};
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {};

};

class CC_DLL CCNikScrollViewDelegate
{
public:
//    virtual ~CCNikScrollViewDelegate() {}
    virtual void scrollViewTouchBegan(CCNikScrollView* view) {};
    virtual void scrollViewTouchEnded(CCNikScrollView* view) {};
    
    virtual void scrollViewDidScroll(CCNikScrollView* view) {};
    virtual void scrollViewDidZoom(CCNikScrollView* view) {};
    virtual void scrollViewWillBeginDragging(CCNikScrollView* view) {};
    virtual void scrollViewDidEndDragging(CCNikScrollView* view, bool decelerate) {};
    virtual void scrollViewWillBeginDecelerating(CCNikScrollView* view) {};
    virtual void scrollViewDidEndDecelerating(CCNikScrollView* view) {};
    
    virtual void scrollViewWillBeginZooming(CCNikScrollView* view) {};
    virtual void scrollViewDidEndZooming(CCNikScrollView* view) {};

    virtual void scrollViewDidEndZoomingAnimation(CCNikScrollView *view) {};
};

typedef enum {
    ScrollViewTouchStateNormal,
    ScrollViewTouchStateBegan,
    ScrollViewTouchStateMoved,
    ScrollViewTouchStateEnded,
    ScrollViewTouchStateCanceled
}ScrollViewTouchState;

typedef enum {
    ScrollViewEventStateNormal,
    ScrollViewEventStatePanning,
    ScrollViewEventStateZooming
}ScrollViewEventState;

class CCNikScrollView: public cocos2d::CCLayer, public CCNikScrollViewButtonDelegate{
    
    ScrollViewTouchState scrollViewTouchState;
    ScrollViewEventState scrollViewEventState;
    
    double countTime;
    double beganTouchedTime;
    double beforeTouchedTime;
    double currentTouchedTime;
    double endTouchedTime;
    
    double velocityX;
    double velocityY;
    
    double minimumScale;
    double maximumScale;
    
    bool beganTouch;
    bool beganMove;
    bool beganZoom;
    bool decelerate;
    bool zoomAnimated;
    
    bool scrollEnabled;
    bool zoomingEnabled;
    bool isBounce;
    bool isAlwaysBounceVertical;
    bool isAlwaysBounceHorizontal;

    bool isBounceAnimated;
    
    CCPoint maxContentOffset;
    CCPoint contentOffset;
    
    CCSize contentView;
    
    
//    CCTouch *beganTouch;
    CCArray *allTouches;
    
    CCPoint averageBeganPoint;
    CCPoint averageBeforeLastMovedPoint;
    CCPoint averageMovedPoint;
    
    CCPoint beforeLastMovedFirstPoint;
    CCPoint beforeLastMovedSecondPoint;
    
    CCPoint movedFirstPoint;
    CCPoint movedSecondPoint;
    
    CCPoint scaleDefaultPoint;

    bool isStartCameraMode;
    
    bool isCameraMode;
    CCPoint cameraPosition;
    float cameraScale;
    
    float cameraVelocityScaling;
    CCPoint cameraVelocity;
    
    
protected:
   
    CCNikScrollViewDelegate *m_delegate;
    CCNikScrollViewTouchDelegate *m_touchDelegate;
    cocos2d::CCTouchDelegate *m_ccTouchDelegate;
    
public:
    
    CCNode *contentNode;
    
    virtual bool init();
    virtual void update(float delta);
    
    void updateNormal(float delta);
    void updateCamera(float delta);
    
    virtual ~CCNikScrollView();
    CREATE_FUNC(CCNikScrollView);
    
    virtual void registerWithTouchDispatcher(void);
    
    void addChild(CCNode *child, int zOrder, int tag);
    void addChild(CCNode *child, int zOrder);
    void addChild(CCNode *child);
    void removeAllChildren();
    
    virtual void onEnter();
    virtual void onExit();
    
    void setContentOffset(CCPoint offset);
    CCPoint getContentOffset();
    
    virtual void setContentSize(CCSize size);
    
    void setContentView(CCSize size);
    CCSize getContentView() { return contentView; };
    
    void setScale(float scale);
    float getScale() { return contentNode->getScale(); }
    bool getAnimated() { return zoomAnimated; }

    bool isInPosition(CCPoint point, float scale, float tolerance);

    void setCameraMode(bool cameraMode);
    void setScrollEnabled(bool enabled);
    void resetScrollEnabled(bool enabled);
    void setZoomingEnabled(bool enabled);
    
    void setMinimumScale(double scale);
    double getMinimumScale() { return minimumScale; }
    void setMaximumScale(double scale);
    double getMaximumScale() { return maximumScale; }
    
    void setBounce(bool bounce) { isBounce = bounce; };
    void setAlwaysBounceVertical(bool bounce) { isAlwaysBounceVertical = bounce; };
    void setAlwaysBounceHorizontal(bool bounce) { isAlwaysBounceHorizontal = bounce; };

    virtual bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    virtual void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void ccTouchesBegan(CCSet* touches, CCEvent* event);
    void ccTouchesMoved(CCSet* touches, CCEvent* event);
    void ccTouchesEnded(CCSet* touches, CCEvent* event);
    void ccTouchesCancelled(CCSet* touches, CCEvent* event);
    
    CCPoint adjustZoomPosition(CCPoint fromPoint, CCPoint anchorPoint,float fromZoom, float toZoom);
    CCPoint adjustPositionInbounds(CCPoint currentPoint);
    
    void zoomOnPosition(CCPoint position,double scale, double duration);
    void zoomOnPosition(CCPoint position,double scale, double duration, bool force);
    void zoomAnimateFinished();
    
    void setCameraOnPosition(CCPoint position,double scale, double duration);
    
    CCNikScrollViewDelegate* getDelegate() { return m_delegate; }
    void setDelegate(CCNikScrollViewDelegate* delegate) { m_delegate = delegate; }
    
    CCNikScrollViewTouchDelegate* getTouchDelegate() { return m_touchDelegate; }
    void setTouchDelegate(CCNikScrollViewTouchDelegate* delegate) { m_touchDelegate = delegate; }
    
    cocos2d::CCTouchDelegate* getCCTouchDelegate() { return m_ccTouchDelegate; };
    void setCCTouchDelegate(cocos2d::CCTouchDelegate* delegate) { m_ccTouchDelegate = delegate; }
    
    void bounceAnimateDidFinished();
    bool isMovingOrZooming() { return numberOfRunningActions() > 0; }
    
};


#endif /* defined(__Cheetah__CCNikScrollView__) */
