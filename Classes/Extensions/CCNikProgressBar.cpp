//
//  CCNikProgressBar.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/4/2557 BE.
//
//

#include "CCNikProgressBar.h"

CCNikProgressBar* CCNikProgressBar::create(cocos2d::CCSprite *leftSprite, CCScale9Sprite *progressBarSprite, cocos2d::CCSprite *rightSprite) {
    
    CCNikProgressBar *obj = new CCNikProgressBar();
    if (!obj->init(leftSprite, progressBarSprite, rightSprite)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool CCNikProgressBar::init(cocos2d::CCSprite *leftSprite, CCScale9Sprite *progressBarSprite, cocos2d::CCSprite *rightSprite) {
    
    if (!progressBarSprite) {
        return false;
    }
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_leftSprite = leftSprite;
    m_rightSprite = rightSprite;
    m_progressBarSprite = progressBarSprite;
    m_maskSprite = CCScale9SpriteMask::create();

    if (m_leftSprite)
    {
        m_leftSprite->setAnchorPoint(ccp(0, 0.5f));
        this->addChild(m_leftSprite);
    }

    if (m_rightSprite)
    {
        m_rightSprite->setAnchorPoint(ccp(0, 0.5f));
        this->addChild(m_rightSprite);
    }

    m_progressBarSprite->setAnchorPoint(ccp(0, 0.5f));
    m_maskSprite->setAnchorPoint(ccp(0, 0.5f));

    setAnchorPoint(ccp(0.5f, 0.5f));

    this->addChild(m_maskSprite);
    
    m_maskSprite->addChild(m_progressBarSprite);

    setPercentage(0);
    scheduleUpdate();
    
    return true;
}

void CCNikProgressBar::setPercentage(float percentage)
{
    m_velocity = 0;
    m_percentage = MIN(percentage, 100);
    m_curretPercentage = percentage;
    
}

void CCNikProgressBar::setPercentage(float percentage, float duration)
{
    m_percentage = percentage;
    m_velocity = (m_percentage - m_curretPercentage)/duration;
}

void CCNikProgressBar:: setContentSize(cocos2d::CCSize size)
{
    CCNode::setContentSize(size);

    if (m_rightSprite && m_leftSprite)
    {
        m_progressBarSprite->setContentSize(CCSize(size.width - m_leftSprite->getContentSize().width - m_rightSprite->getContentSize().width, /*m_leftSprite->getContentSize().height)*/size.height));
        m_leftSprite->setPosition(ccp(1.5f, this->getContentSize().height*0.5f));
        m_maskSprite->setPosition(ccp(m_leftSprite->getPositionX() + m_leftSprite->getContentSize().width - 1, this->getContentSize().height*0.5f));
        m_maskSprite->setContentSize(CCSize((getContentSize().width - m_leftSprite->getContentSize().width - m_rightSprite->getContentSize().width)*m_curretPercentage/100, m_leftSprite->getContentSize().height));
        m_progressBarSprite->setPosition(0, m_maskSprite->getContentSize().height*0.5f);
        m_rightSprite->setPosition(ccp(m_maskSprite->getPositionX() + m_maskSprite->getContentSize().width - 2, this->getContentSize().height*0.5f));
    }
    else
    {
        m_progressBarSprite->setContentSize(CCSize(size.width, size.height));
        m_maskSprite->setPosition(ccp(0, this->getContentSize().height*0.5f));
        m_maskSprite->setContentSize(CCSize((getContentSize().width)*m_curretPercentage/100, getContentSize().height));
        m_progressBarSprite->setPosition(0, m_maskSprite->getContentSize().height*0.5f);
    }
}

void CCNikProgressBar::update(float delta)
{

    m_curretPercentage += m_velocity*delta;
    m_curretPercentage = MAX(0, m_curretPercentage);
    m_curretPercentage = MIN(m_percentage, m_curretPercentage);


    if (m_rightSprite && m_leftSprite)
    {
        float width = (getContentSize().width - m_leftSprite->getContentSize().width - m_rightSprite->getContentSize().width)*m_curretPercentage/100;
        if (fabsf(width - m_maskSprite->getContentSize().width) > 0) {
            m_maskSprite->setContentSize(CCSize((getContentSize().width - m_leftSprite->getContentSize().width - m_rightSprite->getContentSize().width)*m_curretPercentage/100, m_leftSprite->getContentSize().height));
        }
        m_rightSprite->setPosition(ccp(m_maskSprite->getPositionX() + m_maskSprite->getContentSize().width - 2, this->getContentSize().height*0.5f));
    }
    else
    {
        float width = (getContentSize().width)*m_curretPercentage/100;
        if (fabsf(width - m_maskSprite->getContentSize().width) > 0) {
            m_maskSprite->setContentSize(CCSize((getContentSize().width)*m_curretPercentage/100, getContentSize().height));
        }
    }
}

void CCNikProgressBar::visit()
{
    CCNode::visit();
    
    return;
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRect _rect = CCRect(0,
                          0,
                          _screenSize.width,
                          _screenSize.height);
    
    CCPoint _pos = this->getPosition();
    CCPoint _scl = ccp(this->getScaleX(), this->getScaleY());
    CCNode *_node = this;
    int _count=0;
    
    while (CCNode *_parent = _node->getParent()) {
        if (_count==0) {
            _pos = _parent->convertToWorldSpace(_pos);
        }
        _count++;
        _scl = ccp(fabsf(_scl.x*_parent->getScaleX()), fabsf(_scl.y*_parent->getScaleY()));
        _node = _parent;
    }
    
    CCSize _siz = CCSize(getContentSize().width*_scl.x, getContentSize().height*_scl.y);
    CCPoint _ach = getAnchorPoint();
    
    _rect = CCRect(_pos.x-((_siz.width)*_ach.x),
                   _pos.y-((_siz.height)*_ach.y),
                   m_leftSprite->getContentSize().width + m_rightSprite->getContentSize().width + m_progressBarSprite->getContentSize().width*m_curretPercentage/100 + 5,
                   _siz.height);
    
    kmGLPushMatrix();
    glEnable(GL_SCISSOR_TEST);
    
    cocos2d::CCEGLView::sharedOpenGLView()->setScissorInPoints(_rect.origin.x,
                                                               _rect.origin.y,
                                                               _rect.size.width,
                                                               _rect.size.height);
    
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
    
    m_rightSprite->setPosition(ccp(m_leftSprite->getContentSize().width +  m_rightSprite->getContentSize().width*0.5f + m_progressBarSprite->getContentSize().width*m_curretPercentage/100, this->getContentSize().height*0.5f));
}

#pragma mark - CCScale9SpriteMask

CCScale9SpriteMask* CCScale9SpriteMask::create() {
    
    CCScale9SpriteMask *obj = new CCScale9SpriteMask();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool CCScale9SpriteMask::init() {
    
    if (!CCNode::init()) {
        return false;
    }

    return true;
}


void CCScale9SpriteMask::visit() {

    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCRect _rect = CCRect(0,
                          0,
                          _screenSize.width,
                          _screenSize.height);
    
    CCPoint _pos = this->getPosition();
    CCPoint _scl = ccp(this->getScaleX(), this->getScaleY());
    CCNode *_node = this;
    int _count=0;
    
    while (CCNode *_parent = _node->getParent()) {
        if (_count==0) {
            _pos = _parent->convertToWorldSpace(_pos);
        }
        _count++;
        _scl = ccp(fabsf(_scl.x*_parent->getScaleX()), fabsf(_scl.y*_parent->getScaleY()));
        _node = _parent;
    }
    
    CCSize _siz = CCSize(getContentSize().width*_scl.x, getContentSize().height*_scl.y);
    CCPoint _ach = getAnchorPoint();
    
    _rect = CCRect(_pos.x-((_siz.width)*_ach.x) - 1,
                   _pos.y-((_siz.height)*_ach.y) - 1,
                   _siz.width + 3,
                   _siz.height + 3);
    
    kmGLPushMatrix();
    glEnable(GL_SCISSOR_TEST);
    
    cocos2d::CCEGLView::sharedOpenGLView()->setScissorInPoints(_rect.origin.x,
                                                               _rect.origin.y,
                                                               _rect.size.width,
                                                               _rect.size.height);
    
    CCNode::visit();
    glDisable(GL_SCISSOR_TEST);
    kmGLPopMatrix();
}
