//
//  CCNikScrollViewButton.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/20/2557 BE.
//
//

#include "CCNikScrollViewButton.h"

#include "CCNikScrollView.h"
#include "Settings.h"

CCNikScrollViewButton* CCNikScrollViewButton::create()
{
    CCNikScrollViewButton* pRet = new CCNikScrollViewButton();
    pRet->init();
    pRet->autorelease();
    return pRet;
}

CCNikScrollViewButton* CCNikScrollViewButton::create(cocos2d::CCNode *normalSprite, cocos2d::CCNode *selectedSprite)
{
    CCNikScrollViewButton* pRet = new CCNikScrollViewButton();
    pRet->init(normalSprite, selectedSprite);
    pRet->autorelease();
    return pRet;
}

bool CCNikScrollViewButton::init()
{
    return CCNikScrollViewButton::init(NULL, NULL);
}

bool CCNikScrollViewButton::init(cocos2d::CCNode *normalSprite, cocos2d::CCNode *selectedSprite)
{
    if (!CCLayer::init()) {
        return false;
    }
    
    m_enabled = true;
    
    this->ignoreAnchorPointForPosition(false);

    this->setTouchEnabled(true);
    this->setTouchPriority(kCCMenuHandlerPriority - 1);
    this->setTouchMode(kCCTouchesOneByOne);
    this->setAnchorPoint(ccp(0.0f, 0.0f));
    
    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(contentNode);

    m_normalSprite = normalSprite;
    m_selectedSprite = selectedSprite;
    
    if (m_normalSprite) {
        contentNode->addChild(m_normalSprite);
        contentNode->setContentSize(m_normalSprite->getContentSize());
        m_normalSprite->setPosition(contentNode->getContentSize()*0.5f);
    }
    if (m_selectedSprite) {
        contentNode->addChild(m_selectedSprite);
        m_selectedSprite->setPosition(contentNode->getContentSize()*0.5f);
    }

    this->setContentSize(contentNode->getContentSize());
    
    return true;
}

void CCNikScrollViewButton::addChild(cocos2d::CCNode *child)
{
    addChild(child, child->getZOrder());
}

void CCNikScrollViewButton::addChild(cocos2d::CCNode *child, int zOrder)
{
    addChild(child, zOrder, child->getTag());
}

void CCNikScrollViewButton::addChild(cocos2d::CCNode *child, int zOrder, int tag)
{
    if (child == contentNode) {
        CCLayer::addChild(child, zOrder, tag);
    } else {
        contentNode->addChild(child, zOrder, tag);
    }
}

void CCNikScrollViewButton::setContentSize(cocos2d::CCSize size)
{
    CCLayer::setContentSize(size);
    if (contentNode) {
        contentNode->setContentSize(size);
        contentNode->setPosition(this->getContentSize()*0.5f);
        
        if (m_normalSprite) {
            m_normalSprite->setPosition(size*0.5f);
        }
        
        if (m_selectedSprite) {
            m_selectedSprite->setPosition(size*0.5f);
        }
        
        if (m_disabledSprite) {
            m_disabledSprite->setPosition(size*0.5f);
        }
    }
}

void CCNikScrollViewButton::setEnabled(bool enabled)
{
    m_enabled = enabled;
    setState();
}

void CCNikScrollViewButton::setDisableSprite(cocos2d::CCNode *disabledSprite)
{
    if (disabledSprite) {
        m_disabledSprite = disabledSprite;
        this->addChild(m_disabledSprite, -1);
        this->setContentSize(this->getContentSize());
        
        setState();
    }
}

void CCNikScrollViewButton::setState()
{
    if (m_enabled) {
        if (m_normalSprite) m_normalSprite->setVisible(true);
        if (m_disabledSprite) m_disabledSprite->setVisible(false);
    } else {
        if (m_normalSprite) m_normalSprite->setVisible(false);
        if (m_disabledSprite) m_disabledSprite->setVisible(true);
    }
}

void CCNikScrollViewButton::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikScrollViewButtonState state)
{
    m_pListeners[state] = rec;
    m_pfnSelectors[state] = selector;
}

void CCNikScrollViewButton::setDragToOutOfBoundTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector)
{
    m_pDragListener = rec;
    m_pfnDragSelector = selector;
}

bool CCNikScrollViewButton::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (!m_enabled) return false;
    
    isMoved = false;
    isDragToOutOfBoundTarget = false;
    
    CCNikScrollView *scrollView;
    
    if (m_delegate) {
        scrollView = (CCNikScrollView *)m_delegate;
    } else {
        scrollView = (CCNikScrollView *)this->getParent()->getParent();
    }
    
    CCPoint locationOnScrollView = scrollView->convertTouchToNodeSpace(touch);
    if (!scrollView->boundingBox().containsPoint(locationOnScrollView)) {
        return false;
    }
    
    CCPoint location = this->convertTouchToNodeSpace(touch);
    CCRect rect = this->boundingBox();
    rect.origin = CCPointZero;

    if (rect.containsPoint(location)) {
        contentNode->setScale(NIKBUTTON_SCALE_WHEN_TOUCH);
        
        if (m_pListeners[CCNikScrollViewButtonStateTouchDown] && m_pfnSelectors[CCNikScrollViewButtonStateTouchDown])
        {
            (m_pListeners[CCNikScrollViewButtonStateTouchDown]->*m_pfnSelectors[CCNikScrollViewButtonStateTouchDown])(this);
        }
        
        scrollView->ccTouchBegan(touch, event);
        return true;
    }
    
    return false;
}

void CCNikScrollViewButton::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    isMoved = true;

    CCNikScrollView *scrollView;
    
    if (m_delegate) {
        scrollView = (CCNikScrollView *)m_delegate;
    } else {
        scrollView = (CCNikScrollView *)this->getParent()->getParent();
    }
    
    CCPoint location = touch->getLocation();
    if (!isDragToOutOfBoundTarget && location.x > scrollView->boundingBox().origin.x + scrollView->boundingBox().size.width && m_pDragListener && m_pfnDragSelector)
    {
        isDragToOutOfBoundTarget = true;
        (m_pDragListener->*m_pfnDragSelector)(this);
    }
    
    scrollView->ccTouchMoved(touch, event);
}

void CCNikScrollViewButton::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    if (isMoved)
    {
        float distance2 = touch->getStartLocationInView().getDistanceSq(touch->getLocationInView());
        CCLog_("CCNikScrollViewButton::ccTouchEnded %f", sqrtf(distance2));

        if (distance2 < ANDROID_TOUCH_MOVE_THRESHOLD*ANDROID_TOUCH_MOVE_THRESHOLD)
        {
            isMoved = false;
        }
    }

#endif

    contentNode->setScale(1.0f);
    
    CCPoint location = this->convertTouchToNodeSpace(touch);
    CCRect rect = this->boundingBox();
    rect.origin = CCPointZero;
    if (rect.containsPoint(location)) {
        if (m_pListeners[CCNikScrollViewButtonStateTouchUp] && m_pfnSelectors[CCNikScrollViewButtonStateTouchUp] && !isMoved)
        {
            (m_pListeners[CCNikScrollViewButtonStateTouchUp]->*m_pfnSelectors[CCNikScrollViewButtonStateTouchUp])(this);
        }
    }
    
    CCNikScrollView *scrollView;
    
    if (m_delegate) {
        scrollView = (CCNikScrollView *)m_delegate;
    } else {
        scrollView = (CCNikScrollView *)this->getParent()->getParent();
    }
    scrollView->ccTouchEnded(touch, event);

}

void CCNikScrollViewButton::ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{
    CCNikScrollView *scrollView;
    
    if (m_delegate) {
        scrollView = (CCNikScrollView *)m_delegate;
    } else {
        scrollView = (CCNikScrollView *)this->getParent()->getParent();
    }
    scrollView->ccTouchCancelled(touch, event);
}
