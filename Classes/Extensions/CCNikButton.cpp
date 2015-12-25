//
//  CCNikButton.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/28/2557 BE.
//
//

#include "CCNikButton.h"

CCNikButton* CCNikButton::create(cocos2d::CCSprite *normalSprite, cocos2d::CCSprite *selectedSprite)
{
    CCNikButton* pRet = new CCNikButton();
    pRet->init(normalSprite, selectedSprite);
    pRet->autorelease();
    return pRet;
}

bool CCNikButton::init(cocos2d::CCSprite *normalSprite, cocos2d::CCSprite *selectedSprite)
{
    if (!CCLayerRGBA::init())
    {
        return false;
    }

    m_enabled = true;
    m_event = CCNikButtonEventTouchUpOutside;
    
    onWillActive = NULL;
    onWillActiveSender = NULL;
    onWillReleased = NULL;
    onWillReleasedSender = NULL;
    
    m_activeScale = SCALE_ON_TOUCH;
    m_activeScaleEnabled = false;

    setTouchEnabled(true);
    setTouchPriority(kCCMenuHandlerPriority);
    setTouchMode(kCCTouchesOneByOne);
    
    this->ignoreAnchorPointForPosition(false);
    setAnchorPoint(ccp(0.5f, 0.5f));
    
    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    this->addChild(contentNode);
    
    m_normalSprite = normalSprite;
    m_selectedSprite = selectedSprite;
    
    CCSize size = CCSizeZero;
    
    if (m_normalSprite) {
        contentNode->addChild(m_normalSprite, -1);
        size = m_normalSprite->getContentSize();
    }
    if (m_selectedSprite) {
        m_selectedSprite->setVisible(false);
        contentNode->addChild(m_selectedSprite, -1);
    } else {
        setEnableActiveScale(true);
    }
    
    this->setContentSize(size);
    
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    
    return true;
}

CCNikButton::~CCNikButton() {
    
    if (onWillReleased) {
        onWillReleased(onWillReleasedSender);
    }
}

void CCNikButton::addChild(cocos2d::CCNode *child)
{
    addChild(child, child->getZOrder());
}

void CCNikButton::addChild(cocos2d::CCNode *child, int zOrder)
{
    addChild(child, zOrder, child->getTag());
}

void CCNikButton::addChild(cocos2d::CCNode *child, int zOrder, int tag)
{
    if (child == contentNode) {
        CCLayer::addChild(child, zOrder, tag);
    } else {
        contentNode->addChild(child, zOrder, tag);
    }
}

void CCNikButton::setEnabled(bool enabled)
{
    m_enabled = enabled;
    setState();
}

void CCNikButton::setDisableSprite(cocos2d::CCSprite *disabledSprite)
{
    if (disabledSprite) {
        m_disabledSprite = disabledSprite;
        this->addChild(m_disabledSprite, -1);
        this->setContentSize(this->getContentSize());
        
        setState();
    }
}

void CCNikButton::setState()
{
    if (m_enabled && m_disabledSprite) {
        m_normalSprite->setVisible(true);
        m_disabledSprite->setVisible(false);
        
    } else if (m_disabledSprite){
        m_normalSprite->setVisible(false);
        m_disabledSprite->setVisible(true);
    }
}

void CCNikButton::setContentSize(cocos2d::CCSize size)
{
    CCLayerRGBA::setContentSize(size);
    if (contentNode) {
        contentNode->setContentSize(size);
        contentNode->setPosition(size*0.5f);
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
    
//    CCLog_("%f, %f",contentNode->getContentSize().width, contentNode->getContentSize().height);
}

void CCNikButton::setTarget(cocos2d::CCObject *rec, SEL_MenuHandler selector, CCNikButtonEvent state)
{
    m_pListeners[state] = rec;
    m_pfnSelectors[state] = selector;
}


void CCNikButton::touchDown()
{
    m_event = CCNikButtonEventTouchDown;
    if (m_pListeners[CCNikButtonEventTouchDown] && m_pfnSelectors[CCNikButtonEventTouchDown])
    {
        if (onWillActive) {
            onWillActive(onWillActiveSender);
        }
        (m_pListeners[CCNikButtonEventTouchDown]->*m_pfnSelectors[CCNikButtonEventTouchDown])(this);
    }
}

void CCNikButton::touchUpInside()
{
    m_event = CCNikButtonEventTouchUpInside;
    if (m_pListeners[CCNikButtonEventTouchUpInside] && m_pfnSelectors[CCNikButtonEventTouchUpInside])
    {
        if (onWillActive) {
            onWillActive(onWillActiveSender);
        }
        (m_pListeners[CCNikButtonEventTouchUpInside]->*m_pfnSelectors[CCNikButtonEventTouchUpInside])(this);
    }
    
}

void CCNikButton::touchUpOutSide()
{
    m_event = CCNikButtonEventTouchUpOutside;
    if (m_pListeners[CCNikButtonEventTouchUpOutside] && m_pfnSelectors[CCNikButtonEventTouchUpOutside])
    {
        if (onWillActive) {
            onWillActive(onWillActiveSender);
        }
        (m_pListeners[CCNikButtonEventTouchUpOutside]->*m_pfnSelectors[CCNikButtonEventTouchUpOutside])(this);
    }
   
}

bool CCNikButton::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (!m_enabled) return false;
    
    isMoved = false;
    isDragToOutOfBoundTarget = false;
    
//    CCPoint location = touch->getLocation();
    CCPoint location = this->convertTouchToNodeSpaceAR(touch);
    CCRect rect = this->boundingBox();
    
//    CCLog_("================================");
//    CCLog_("CCNikButton::ccTouchBegan rect, then location");
//    DEBUG_RECT(rect);
//    DEBUG_POINT_XY(location);
//    CCLog_("================================");
    
    rect.origin = CCPointZero - ccp(getContentSize().width*getAnchorPoint().x, getContentSize().height*getAnchorPoint().y);
    float scaleX = getScaleX();
    float scaleY = getScaleY();
    if (scaleX == 0.0f || scaleY == 0.0f) return false;
    if (!isVisible()) return false;
    
    rect.size = CCSizeMake(rect.size.width / scaleX,
                           rect.size.height / scaleY);
//    rect.size = CCSizeMake(rect.size.width * scaleX,
//                           rect.size.height * scaleY);
//    CCLog_("locatio : %f, %f",location.x, location.y);
    if (rect.containsPoint(location)) {
        if (m_activeScaleEnabled) contentNode->setScale(m_activeScale);
        touchDown();
        
        if (onHit) {
            onHit(this, m_sender);
        }
        
        return true;
    }
    
    return false;
}

void CCNikButton::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    isMoved = true;
}

void CCNikButton::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{
    if (m_activeScaleEnabled) contentNode->setScale(1.0f);
    
//    CCPoint location = touch->getLocation();
    CCPoint location = this->convertTouchToNodeSpaceAR(touch);
    CCRect rect = this->boundingBox();
    rect.origin = CCPointZero - ccp(getContentSize().width*getAnchorPoint().x, getContentSize().height*getAnchorPoint().y);
    
    float scaleX = getScaleX();
    float scaleY = getScaleY();
    if (scaleX == 0.0f || scaleY == 0.0f) return;
    if (!isVisible()) return;
    
    rect.size = CCSizeMake(rect.size.width / scaleX,
                           rect.size.height / scaleY);
    if (rect.containsPoint(location)) {
        touchUpInside();
    } else {
        touchUpOutSide();
    }
}

void CCNikButton::ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
{

}
