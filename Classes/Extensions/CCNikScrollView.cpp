//
//  CCNikScrollView.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/4/2557 BE.
//
//

#include "CCNikScrollView.h"
#include "../Settings.h"

bool CCNikScrollView::init() {
    
    if (!CCLayer::init()) {
        return false;
    }
    
    isBounce = false;
    isAlwaysBounceVertical = false;
    isAlwaysBounceHorizontal = false;
    
    isCameraMode = false;
    isStartCameraMode = false;
    
    beganMove = false;
    beganZoom = false;
    decelerate = false;
    
    zoomingEnabled = false;
    
    m_delegate = NULL;
    
    minimumScale = 1;
    maximumScale = 1;
    
    cameraPosition = CCPointZero;
    cameraScale = 1;
    
    allTouches = CCArray::create();
    allTouches->retain();
    
    contentNode = CCNode::create();
    this->addChild(contentNode);
    
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(size);

    setTouchEnabled(true);
    setScrollEnabled(true);

    return true;
}

CCNikScrollView::~CCNikScrollView()
{
    if (allTouches != NULL) {
        allTouches->release();
        allTouches = NULL;
    }
}

void CCNikScrollView::registerWithTouchDispatcher()
{
    CCLayer::registerWithTouchDispatcher();
}

void CCNikScrollView::addChild(cocos2d::CCNode *child, int zOrder, int tag)
{
    if (child == contentNode) {
        CCNode::addChild(child, zOrder, tag);
    } else {
        contentNode->addChild(child, zOrder, tag);
    }
}
void CCNikScrollView::addChild(cocos2d::CCNode *child, int zOrder)
{
    addChild(child, zOrder, child->getTag());
}

void CCNikScrollView::addChild(CCNode *child)
{
    addChild(child, child->getZOrder());
}

void CCNikScrollView::removeAllChildren()
{
    contentNode->removeAllChildren();
}

void CCNikScrollView::onEnter()
{
    scheduleUpdate();
    CCLayer::onEnter();
}
void CCNikScrollView::onExit()
{
    unscheduleUpdate();
    CCLayer::onExit();
}

void CCNikScrollView::setContentOffset(cocos2d::CCPoint offset)
{
    contentOffset = offset;
    contentNode->setPosition(-offset);
    CCAssert(zoomAnimated == false, "Zoom animated is true!");
}

CCPoint CCNikScrollView::getContentOffset()
{
    return -contentNode->getPosition();
}

void CCNikScrollView::setContentSize(cocos2d::CCSize size)
{
    CCLayer::setContentSize(size);
    setContentView(size);
}

void CCNikScrollView::setContentView(cocos2d::CCSize size)
{
    contentView = CCSizeMake(MAX(getContentSize().width, size.width), MAX(getContentSize().height, size.height));
    contentNode->setContentSize(contentView);
    
    CCPoint lastPosition = contentNode->getPosition();
    lastPosition.x = MIN(0, lastPosition.x);
    lastPosition.x = MAX(-contentView.width*maximumScale + getContentSize().width, lastPosition.x);
    lastPosition.y = MIN(0, lastPosition.y);
    lastPosition.y = MAX(-contentView.height*maximumScale + getContentSize().height, lastPosition.y);

    CCAssert(zoomAnimated == false, "Zoom animated is true!");
    contentNode->setPosition(lastPosition);
}

void CCNikScrollView::setScale(float scale)
{
    contentNode->setScale(scale);
}

void CCNikScrollView::setScrollEnabled(bool enabled)
{
    scrollEnabled = enabled;
    setTouchEnabled(enabled);
    
    if (!enabled) {
        allTouches->removeAllObjects();
        averageBeganPoint = CCPointZero;
        averageBeforeLastMovedPoint = CCPointZero;
        averageMovedPoint = CCPointZero;
    }
}

void CCNikScrollView::resetScrollEnabled(bool enabled) {
    
    setScrollEnabled(!enabled);
    setScrollEnabled(enabled);
    
}

void CCNikScrollView::setZoomingEnabled(bool enabled)
{
    zoomingEnabled = enabled;
}

void CCNikScrollView::setCameraMode(bool cameraMode)
{
    setScrollEnabled(!cameraMode);
    isCameraMode = cameraMode;
}

void CCNikScrollView::setMinimumScale(double scale)
{
    minimumScale = scale;
}

void CCNikScrollView::setMaximumScale(double scale)
{
    maximumScale = scale;
}

void CCNikScrollView::update(float delta) {
    
    countTime += delta;
    
    if (isCameraMode) {
        updateCamera(delta);
    } else {
        updateNormal(delta);
    }
}

void CCNikScrollView::updateNormal(float delta)
{
    if (!scrollEnabled) return;
    
    if (scrollViewEventState == ScrollViewEventStatePanning || scrollViewEventState == ScrollViewEventStateNormal)
    {
        if (scrollViewTouchState == ScrollViewTouchStateNormal)
        {
            if (decelerate && endTouchedTime - currentTouchedTime < 0.05f) {
                double aX = velocityX*2.f;
                double aY = velocityY*2.f;
                
                double sX = 0;
                double sY = 0;
                sX = MAX(0, velocityX*delta - aX*pow(delta, 2));
                sY = MAX(0, velocityY*delta - aY*pow(delta, 2));
                
                if (velocityX > 1 || velocityY > 1) {
                    CCPoint lastPosition = CCPointZero;
                    if (decelerate && isBounce) {
                        
                        CCPoint movedPosition = CCPointZero;
                        if (contentNode->getContentSize().width != getContentSize().width) {
                            if (contentNode->getPosition().x > 0 || contentNode->getPosition().x < -contentView.width*contentNode->getScale() + getContentSize().width) {
                                velocityX = MAX(0, 2*sX/(delta*2) - velocityX);
                                movedPosition.x = + ((averageMovedPoint.x - averageBeforeLastMovedPoint.x) > 0 ? sX : -sX)*0.05f;
                            } else {
                                velocityX = MAX(0, 2*sX/delta - velocityX);
                                movedPosition.x = (averageMovedPoint.x - averageBeforeLastMovedPoint.x) > 0 ? sX : -sX;
                            }
                        }
                        if (contentNode->getContentSize().height != getContentSize().height) {
                            if (contentNode->getPosition().y > 0 || contentNode->getPosition().y < -contentView.height*contentNode->getScale() + getContentSize().height) {
                                velocityY = MAX(0, 2*sY/(delta*2) - velocityY);
                                movedPosition.y = ((averageMovedPoint.y - averageBeforeLastMovedPoint.y) > 0 ? sY : -sY)*0.05f;
                            } else {
                                velocityY = MAX(0, 2*sY/delta - velocityY);
                                movedPosition.y = (averageMovedPoint.y - averageBeforeLastMovedPoint.y) > 0 ? sY : -sY;
                            }
                        }
                        lastPosition = contentNode->getPosition() + movedPosition;
                    } else {
                        velocityX = MAX(0, 2*sX/delta - velocityX);
                        velocityY = MAX(0, 2*sY/delta - velocityY);
                        
                        lastPosition = contentNode->getPosition() + ccp((averageMovedPoint.x - averageBeforeLastMovedPoint.x) > 0 ? sX : -sX, (averageMovedPoint.y - averageBeforeLastMovedPoint.y) > 0 ? sY : -sY);
                        lastPosition.x = MIN(0, lastPosition.x);
                        lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
                        lastPosition.y = MIN(0, lastPosition.y);
                        lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
                    }
                    
                    bool didScroll = false;
                    if ((int)lastPosition.x != (int)contentNode->getPositionX() || (int)lastPosition.y != (int)contentNode->getPositionY()) {
                        didScroll = true;
                    } else {
                        velocityX = 0;
                        velocityY = 0;
                    }
                    
                    contentNode->setPosition(lastPosition);

                    if (didScroll && m_delegate) m_delegate->scrollViewDidScroll(this);

                } else {
                    
                    scrollViewEventState = ScrollViewEventStateNormal;
                    
                    if (decelerate && isBounce && !isBounceAnimated) {
                        CCPoint lastPosition = contentNode->getPosition();
                        lastPosition.x = MIN(0, lastPosition.x);
                        lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
                        lastPosition.y = MIN(0, lastPosition.y);
                        lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
                        isBounceAnimated = true;
                        contentNode->stopAllActions();
                        contentNode->runAction(CCSequence::create(CCMoveTo::create(0.1f, lastPosition),
                                                                  CCCallFunc::create(this, callfunc_selector(CCNikScrollView::bounceAnimateDidFinished)),
                                                                  NULL));
                    }
                    
                    if (decelerate && m_delegate) m_delegate->scrollViewDidEndDecelerating(this);
                    decelerate = false;
                }
                
            } else {
                scrollViewEventState = ScrollViewEventStateNormal;
            }
        } else if (scrollViewTouchState == ScrollViewTouchStateBegan) {
            beganTouchedTime = countTime;
            currentTouchedTime = countTime;
            beforeTouchedTime = countTime;
        } else if (scrollViewTouchState == ScrollViewTouchStateMoved) {
            beforeTouchedTime = currentTouchedTime;
            currentTouchedTime = countTime;
        } else if (scrollViewTouchState == ScrollViewTouchStateEnded) {
            endTouchedTime = countTime;
            double deltaTime = currentTouchedTime - beforeTouchedTime;
            velocityX = fabs((averageMovedPoint.x - averageBeforeLastMovedPoint.x)/deltaTime);
            velocityY = fabs((averageMovedPoint.y - averageBeforeLastMovedPoint.y)/deltaTime);
            
            scrollViewTouchState = ScrollViewTouchStateNormal;
        }
    }
}
void CCNikScrollView::updateCamera(float delta)
{
    if (!isStartCameraMode && zoomAnimated) return;
    
    CCPoint currentPosition = contentNode->getPosition();
    float currentScale = contentNode->getScale();
    
    CCPoint updatePosition = currentPosition + cameraVelocity*delta;
    float updateScale = currentScale + cameraVelocityScaling*delta;
    
    updateScale = MAX(minimumScale, updateScale);
    updateScale = MIN(maximumScale, updateScale);

    updatePosition.x = MIN(0, updatePosition.x);
    updatePosition.x = MAX(-contentView.width*updateScale + getContentSize().width, updatePosition.x);
    updatePosition.y = MIN(0, updatePosition.y);
    updatePosition.y = MAX(-contentView.height*updateScale + getContentSize().height, updatePosition.y);
    
    
    CCPoint lastPosition = -cameraPosition + ccp(this->getContentSize().width*0.5f, this->getContentSize().height*0.5f);
    lastPosition = this->getContentSize()*0.5f - (this->getContentSize()*0.5f - lastPosition)*cameraScale;
    lastPosition.x = MIN(0, lastPosition.x);
    lastPosition.x = MAX(-contentView.width*cameraScale + getContentSize().width, lastPosition.x);
    lastPosition.y = MIN(0, lastPosition.y);
    lastPosition.y = MAX(-contentView.height*cameraScale + getContentSize().height, lastPosition.y);

    CCAssert(zoomAnimated == false, "Zoom animated is true!");

    contentNode->setScale(updateScale);
    contentNode->setPosition(updatePosition);
}

bool CCNikScrollView::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchBegan(touch, event);
    }
//    CCLog_("Nik Scroll view touch");
    if (!scrollEnabled && !isVisible()) return false;
    
    CCPoint location = this->convertTouchToNodeSpace(touch);
    if (!this->boundingBox().containsPoint(location)) {
        return false;
    }
    
    CCSet *set = CCSet::create();
    set->addObject(touch);
    this->ccTouchesBegan(set, event);
    
    if (m_touchDelegate) {
        m_touchDelegate->ccTouchBegan(touch, event);
    }
    
    return true;
}
void CCNikScrollView::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        float distance2 = touch->getStartLocationInView().getDistanceSq(touch->getLocationInView());
        CCLog_("CCNikScrollViewButton::ccTouchEnded %f", sqrtf(distance2));

        if (distance2 < ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            //! ignore small drags
            return;
        }
#endif

    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchMoved(touch, event);
    }
    
    CCSet *set = CCSet::create();
    set->addObject(touch);
    this->ccTouchesMoved(set, event);
    
    if (m_touchDelegate) {
        m_touchDelegate->ccTouchMoved(touch, event);
    }
}
void CCNikScrollView::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchEnded(touch, event);
    }
    
    CCSet *set = CCSet::create();
    set->addObject(touch);
    this->ccTouchesEnded(set, event);

    if (m_touchDelegate) {
        m_touchDelegate->ccTouchEnded(touch, event);
    }
}

void CCNikScrollView::ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchEnded(touch, event);
    }
    
    CCSet *set = CCSet::create();
    set->addObject(touch);
    this->ccTouchesCancelled(set, event);
    
    if (m_touchDelegate) {
        m_touchDelegate->ccTouchCancelled(touch, event);
    }
}


void CCNikScrollView::ccTouchesBegan(CCSet *touches, CCEvent *event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchesBegan(touches, event);
    }
    
    if (!scrollEnabled) return;
    
    //ignore touch for the second finger if multitouch is not enabled
    if (!zoomingEnabled
        && allTouches->count() >= 1)
    {
        return;
    }
    
    scrollViewTouchState = ScrollViewTouchStateBegan;
    
    if (contentNode->numberOfRunningActions() > 0) {
        contentNode->getActionManager()->pauseTarget(contentNode);
    }
    
    CCSetIterator it = touches->begin();
    for (int i=0; i<touches->count(); i++) {
        CCTouch* touch = (CCTouch*)(*it);
        CCPoint location = this->convertTouchToNodeSpace(touch);
        if (this->boundingBox().containsPoint(location)) {
            allTouches->addObject(touch);
        } else {
            return;
        }
        it++;
    }
    
    if (!beganTouch) {
        if (m_delegate) {
            CCNikScrollViewDelegate* _delegate = dynamic_cast<CCNikScrollViewDelegate*>(m_delegate);
            CCAssert(_delegate != NULL, "m_delegate must not be NULL");
            if (_delegate) {
                _delegate->scrollViewTouchBegan(this);
            }
        }
        beganTouch = true;
    }
    
    if (allTouches->count() == 0) {
        scrollViewEventState = ScrollViewEventStateNormal;
    } else if (allTouches->count() == 1) {
        scrollViewEventState = ScrollViewEventStatePanning;
    } else if (allTouches->count() == 2) {
        scrollViewEventState = ScrollViewEventStateZooming;
    }
    
    if (allTouches->count()) {
        CCPoint averagePoint = CCPointZero;
        for (int i=0; i < allTouches->count(); i++) {
            CCTouch* touch = (CCTouch *)allTouches->objectAtIndex(i);
            CCPoint location = this->convertTouchToNodeSpace(touch);
//            CCLog_("i = %d : %f, %f",i, location.x, location.y);
            averagePoint = averagePoint + location;
        }
        
        averageBeganPoint = averagePoint/allTouches->count();
        averageBeforeLastMovedPoint = averageBeganPoint;
        averageMovedPoint = averageBeganPoint;
    }
    
    
    if (scrollViewEventState == ScrollViewEventStateNormal) {
        
    } else if (scrollViewEventState == ScrollViewEventStatePanning) {
        
    } else if (scrollViewEventState == ScrollViewEventStateZooming) {
        beforeLastMovedFirstPoint = movedFirstPoint;
        beforeLastMovedSecondPoint = movedSecondPoint;
        for (int i=0; i<allTouches->count(); i++) {
            CCTouch* touch = (CCTouch *)allTouches->objectAtIndex(i);
            CCPoint location = this->convertTouchToNodeSpace(touch);
//            CCLog_("i = %d : %f, %f",i, location.x, location.y);
            if (i == 0) {
                movedFirstPoint = location;
            } else if (i == 1) {
                movedSecondPoint = location;
            }
        }
    }    
}
void CCNikScrollView::ccTouchesMoved(CCSet *touches, CCEvent *event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchesMoved(touches, event);
    }
    
    if (!scrollEnabled || !isVisible()) return;
    if (allTouches->count() == 0) return;
    
    isBounceAnimated = false;
    contentNode->stopAllActions();
    
    scrollViewTouchState = ScrollViewTouchStateMoved;

    averageBeforeLastMovedPoint = averageMovedPoint;
    
    CCPoint averagePoint = CCPointZero;
    for (int i=0; i < allTouches->count(); i++) {
        CCTouch* touch = (CCTouch *)allTouches->objectAtIndex(i);
        CCPoint location = this->convertTouchToNodeSpace(touch);
        averagePoint = averagePoint + location;
    }
    averageMovedPoint = averagePoint/allTouches->count();
    
    if (!beganMove) {
        if (m_delegate) m_delegate->scrollViewWillBeginDragging(this);
        beganMove = true;
    }
    
    if (scrollViewEventState == ScrollViewEventStateNormal) {
        
    } else if (scrollViewEventState == ScrollViewEventStatePanning) {
        
        CCPoint lastPosition = CCPointZero;
        if (isBounce) {
            CCPoint movedPosition = CCPointZero;
            if (contentNode->getContentSize().width != getContentSize().width || isAlwaysBounceHorizontal) {
                if (contentNode->getPosition().x > 0 || contentNode->getPosition().x < -contentView.width*contentNode->getScale() + getContentSize().width) {
                    movedPosition.x = (averageMovedPoint.x - averageBeforeLastMovedPoint.x)*0.2f;
                } else {
                    movedPosition.x = (averageMovedPoint.x - averageBeforeLastMovedPoint.x);
                }
            }
            if (contentNode->getContentSize().height != getContentSize().height || isAlwaysBounceVertical) {
                if (contentNode->getPosition().y > 0 || contentNode->getPosition().y < -contentView.height*contentNode->getScale() + getContentSize().height) {
                    movedPosition.y = (averageMovedPoint.y - averageBeforeLastMovedPoint.y)*0.2f;
                } else {
                    movedPosition.y = (averageMovedPoint.y - averageBeforeLastMovedPoint.y);
                }
            }
            lastPosition = contentNode->getPosition() + movedPosition;
        } else {
            lastPosition = contentNode->getPosition() + (averageMovedPoint - averageBeforeLastMovedPoint);
            lastPosition.x = MIN(0, lastPosition.x);
            lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
            lastPosition.y = MIN(0, lastPosition.y);
            lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
        }
        
        if (lastPosition.x != contentNode->getPositionX() || lastPosition.y != contentNode->getPositionY()) {
            if (m_delegate)
            {
                m_delegate->scrollViewDidScroll(this);
            }
        }

        //CCAssert(zoomAnimated == false, "Zoom animated is true!");
        contentNode->setPosition(lastPosition);

    } else if (scrollViewEventState == ScrollViewEventStateZooming) {
        
        if (!beganZoom) {
            if (m_delegate) m_delegate->scrollViewWillBeginZooming(this);
            beganZoom = true;
        }
        
        beforeLastMovedFirstPoint = movedFirstPoint;
        beforeLastMovedSecondPoint = movedSecondPoint;
        
        double lastValue = sqrt(pow(beforeLastMovedFirstPoint.x - beforeLastMovedSecondPoint.x, 2) + pow(beforeLastMovedFirstPoint.y - beforeLastMovedSecondPoint.y, 2));

        for (int i=0; i<allTouches->count(); i++) {
            CCTouch* touch = (CCTouch *)allTouches->objectAtIndex(i);
            CCPoint location = this->convertTouchToNodeSpace(touch);
            if (i == 0) {
                movedFirstPoint = location;
            } else if (i == 1) {
                movedSecondPoint = location;
            }
        }
        
        double value = sqrt(pow(movedFirstPoint.x - movedSecondPoint.x, 2) + pow(movedFirstPoint.y - movedSecondPoint.y, 2));
        double increaseScale = (value - lastValue)*0.00125f*maximumScale;
        double newScale = contentNode->getScale() + increaseScale;
        newScale = MAX(minimumScale, newScale);
        newScale = MIN(maximumScale + maximumScale*0.25f, newScale);
        double beforeScale = contentNode->getScale();
        increaseScale = newScale - beforeScale;
        contentNode->setScale(newScale);
        
        if (increaseScale > 0.001) {
            if (m_delegate != NULL)
            {
                m_delegate->scrollViewDidZoom(this);
            }
        }

        CCPoint lastPosition = averageMovedPoint - (averageMovedPoint - contentNode->getPosition())*newScale/beforeScale + (averageMovedPoint - averageBeforeLastMovedPoint);
//        CCPoint lastPosition = (contentNode->getPosition() + (averageMovedPoint - averageBeforeLastMovedPoint)*newScale/beforeScale);
        lastPosition.x = MIN(0, lastPosition.x);
        lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
        lastPosition.y = MIN(0, lastPosition.y);
        lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
        
        if (lastPosition.x != contentNode->getPositionX() || lastPosition.y != contentNode->getPositionY()) {
            if (m_delegate != NULL)
            {
                m_delegate->scrollViewDidScroll(this);
            }
        }

//        CCAssert(zoomAnimated == false, "Zoom animated is true!");
        contentNode->setPosition(lastPosition);
    }
}
void CCNikScrollView::ccTouchesEnded(CCSet *touches, CCEvent *event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchesEnded(touches, event);
    }
    
    if (contentNode->numberOfRunningActions() > 0) {
        contentNode->getActionManager()->resumeTarget(contentNode);
    }

    CCSetIterator it = touches->begin();
    for (int i=0; i<touches->count(); i++) {
        CCTouch* touch = (CCTouch*)(*it);
//        CCPoint location = this->convertTouchToNodeSpace(touch);
//        CCLog_("ended : %f, %f",location.x, location.y);

        if (allTouches->containsObject(touch)) {
            allTouches->removeObject(touch);
        }
        it++;
    }
    
    if (allTouches->count() == 0 && getScale() > maximumScale) {
        CCPoint lastPosition = getContentSize()*0.5f - (getContentSize()*0.5f - contentNode->getPosition())*maximumScale/getScale();
        float duration = (getScale() - maximumScale)*2;
        lastPosition.x = MIN(0, lastPosition.x);
        lastPosition.x = MAX(-contentView.width*maximumScale + getContentSize().width, lastPosition.x);
        lastPosition.y = MIN(0, lastPosition.y);
        lastPosition.y = MAX(-contentView.height*maximumScale + getContentSize().height, lastPosition.y);
        contentNode->stopAllActions();
        contentNode->runAction(CCEaseIn::create(CCScaleTo::create(duration, maximumScale), 0.5));
        contentNode->runAction(CCEaseIn::create(CCMoveTo::create(duration, lastPosition), 0.5));
    }
    
    if (allTouches->count() < 2) {
        if (beganZoom) {
            if (m_delegate) m_delegate->scrollViewDidEndZooming(this);
            beganZoom = false;
        }
    }
    
    if (allTouches->count() == 0) {
        
        //this is tapping
        if (beganTouch
            && !beganMove
            && (scrollViewEventState == ScrollViewEventStatePanning))
        {
//            CCLog_("the F'ing touch");
            
            
//            CCPoint lastPosition = contentNode->getPosition() + (averageMovedPoint - averageBeforeLastMovedPoint);
//            lastPosition.x = MIN(0, lastPosition.x);
//            lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
//            lastPosition.y = MIN(0, lastPosition.y);
//            lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
//            
//            averageMovedPoint = lastPosition;
//            averageBeforeLastMovedPoint = lastPosition;
//            
//            contentNode->setPosition(lastPosition);
        }
        
        if (beganTouch) {
            if (m_delegate) m_delegate->scrollViewTouchEnded(this);
            beganTouch = false;
        }
        
        if (beganMove) {
            
            double deltaTime = currentTouchedTime - beforeTouchedTime;
            double velocityX = fabs((averageMovedPoint.x - averageBeforeLastMovedPoint.x)/deltaTime);
            double velocityY = fabs((averageMovedPoint.y - averageBeforeLastMovedPoint.y)/deltaTime);

            if (deltaTime <= 0.05 && (velocityX > 1 || velocityY > 1) && (contentNode->getPosition().x <= 0 && contentNode->getPosition().x >= -contentView.width*contentNode->getScale() + getContentSize().width && contentNode->getPosition().y <= 0 && contentNode->getPosition().y >= -contentView.height*contentNode->getScale() + getContentSize().height)) {
                decelerate = true;
            } else {
                decelerate = false;
            }
            if (m_delegate) m_delegate->scrollViewDidEndDragging(this, decelerate);
            
            if (isBounce && !isBounceAnimated && !(contentNode->getPosition().x <= 0 && contentNode->getPosition().x >= -contentView.width*contentNode->getScale() + getContentSize().width && contentNode->getPosition().y <= 0 && contentNode->getPosition().y >= -contentView.height*contentNode->getScale() + getContentSize().height)) {
                CCPoint lastPosition = contentNode->getPosition();
                lastPosition.x = MIN(0, lastPosition.x);
                lastPosition.x = MAX(-contentView.width*contentNode->getScale() + getContentSize().width, lastPosition.x);
                lastPosition.y = MIN(0, lastPosition.y);
                lastPosition.y = MAX(-contentView.height*contentNode->getScale() + getContentSize().height, lastPosition.y);
                isBounceAnimated = true;
                contentNode->stopAllActions();
                contentNode->runAction(CCSequence::create(CCMoveTo::create(0.1f, lastPosition),
                                                          CCCallFunc::create(this, callfunc_selector(CCNikScrollView::bounceAnimateDidFinished)),
                                                          NULL));
            }
            
            if (decelerate && m_delegate) m_delegate->scrollViewWillBeginDecelerating(this);
            
            beganMove = false;
        }
        
        scrollViewTouchState = ScrollViewTouchStateEnded;
        scrollViewEventState = ScrollViewEventStateNormal;
    } else if (allTouches->count() == 1) {
        scrollViewEventState = ScrollViewEventStatePanning;
    } else if (allTouches->count() == 2) {
        scrollViewEventState = ScrollViewEventStateZooming;
    }
    
    if (allTouches->count()) {
        CCPoint averagePoint = CCPointZero;
        for (int i=0; i<allTouches->count(); i++) {
            CCTouch* touch = (CCTouch *)allTouches->objectAtIndex(i);
            CCPoint location = this->convertTouchToNodeSpace(touch);
//            CCLog_("i = %d : %f, %f",i, location.x, location.y);
            averagePoint = averagePoint + location;
        }
        
        averageBeganPoint = averagePoint/allTouches->count();
        averageBeforeLastMovedPoint = averageBeganPoint;
        averageMovedPoint = averageBeganPoint;
        
    }
}
void CCNikScrollView::ccTouchesCancelled(CCSet *touches, CCEvent *event)
{
    if (m_ccTouchDelegate) {
        m_ccTouchDelegate->ccTouchesCancelled(touches, event);
    }
    
    this->ccTouchesEnded(touches, event);
}


CCPoint CCNikScrollView::adjustZoomPosition(cocos2d::CCPoint fromPoint, cocos2d::CCPoint anchorPoint, float fromZoom, float toZoom)
{
    float newX, newY;
    newX = anchorPoint.x - ((anchorPoint.x-fromPoint.x)*toZoom/fromZoom);
    newY = anchorPoint.y - ((anchorPoint.y-fromPoint.y)*toZoom/fromZoom);
    return ccp(newX,newY);
}

void CCNikScrollView::zoomOnPosition(cocos2d::CCPoint position, double scale, double duration) {
    
    zoomOnPosition(position, scale, duration, false);
    
}

void CCNikScrollView::zoomOnPosition(cocos2d::CCPoint position, double scale, double duration, bool force) {
    
    if (zoomAnimated && !force) return;
    
    scale = MAX(minimumScale, scale);
    scale = MIN(maximumScale, scale);
    
    CCPoint lastPosition = -position + ccp(this->getContentSize().width*0.5f, this->getContentSize().height*0.5f);
    lastPosition = this->getContentSize()*0.5f - (this->getContentSize()*0.5f - lastPosition)*scale;
    lastPosition.x = MIN(0, lastPosition.x);
    lastPosition.x = MAX(-contentView.width*scale + getContentSize().width, lastPosition.x);
    lastPosition.y = MIN(0, lastPosition.y);
    lastPosition.y = MAX(-contentView.height*scale + getContentSize().height, lastPosition.y);
    
    if (duration < 0.001) {
        contentNode->setScale(scale);
        contentNode->setPosition(lastPosition);
    } else {
        
        zoomAnimated = true;
        
        contentNode->stopAllActions();
        contentNode->runAction(CCEaseOut::create(CCEaseExponentialOut::create(CCScaleTo::create(duration, scale)), 0.5));
        contentNode->runAction(CCEaseOut::create(CCEaseExponentialOut::create(CCMoveTo::create(duration, lastPosition)), 0.5));
        
        this->stopAllActions();
        this->runAction(CCSequence::create(CCDelayTime::create(duration),
                                           CCCallFunc::create(this, callfunc_selector(CCNikScrollView::zoomAnimateFinished)),
                                           NULL));
    }
}

void CCNikScrollView::zoomAnimateFinished()
{
    if (!zoomAnimated) return;
    
    if (m_delegate) {
        m_delegate->scrollViewDidEndZoomingAnimation(this);
    }
    
    zoomAnimated = false;

}

void CCNikScrollView::bounceAnimateDidFinished()
{
    isBounceAnimated = false;
}

void CCNikScrollView::setCameraOnPosition(cocos2d::CCPoint position, double scale, double duration)
{
    if (!isCameraMode) return;

    scale = MAX(minimumScale, scale);
    scale = MIN(maximumScale, scale);
    
    cameraPosition = position;
    cameraScale = scale;
    
    CCPoint lastPosition = -cameraPosition + ccp(this->getContentSize().width*0.5f, this->getContentSize().height*0.5f);
    lastPosition = this->getContentSize()*0.5f - (this->getContentSize()*0.5f - lastPosition)*scale;
    lastPosition.x = MIN(0, lastPosition.x);
    lastPosition.x = MAX(-contentView.width*scale + getContentSize().width, lastPosition.x);
    lastPosition.y = MIN(0, lastPosition.y);
    lastPosition.y = MAX(-contentView.height*scale + getContentSize().height, lastPosition.y);
    
    CCPoint currentPosition = contentNode->getPosition();
    float currentScale = contentNode->getScale();
    
    cameraVelocityScaling = (scale - currentScale)/duration;
    
    cameraVelocity = (lastPosition - currentPosition)/duration;
    
    isStartCameraMode = true;
}

bool CCNikScrollView::isInPosition(CCPoint position, float scale, float tolerance)
{
    scale = MAX(minimumScale, scale);
    scale = MIN(maximumScale, scale);

    CCPoint lastPosition = -position + ccp(this->getContentSize().width*0.5f, this->getContentSize().height*0.5f);
    lastPosition = this->getContentSize()*0.5f - (this->getContentSize()*0.5f - lastPosition)*scale;
    lastPosition.x = MIN(0, lastPosition.x);
    lastPosition.x = MAX(-contentView.width*scale + getContentSize().width, lastPosition.x);
    lastPosition.y = MIN(0, lastPosition.y);
    lastPosition.y = MAX(-contentView.height*scale + getContentSize().height, lastPosition.y);

    return contentNode->getPosition().getDistanceSq(lastPosition) < tolerance*tolerance;
}
