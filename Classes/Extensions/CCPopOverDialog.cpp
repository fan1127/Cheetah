//
//  CCPopOverDialog.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#include "CCPopOverDialog.h"

//extern CCPoint screenFactor;
extern float sizeMultiplier;

#include "../Extensions/Modal/CCModalController.h"

CCPopOverDialog* CCPopOverDialog::create(cocos2d::CCSize contentSize)
{
    CCPopOverDialog *obj = new CCPopOverDialog();
    if (!obj->init(contentSize, CCPopoverArrowDirectionAny)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
}

CCPopOverDialog* CCPopOverDialog::create(cocos2d::CCSize contentSize, CCPopoverArrowDirection direction)
{
    CCPopOverDialog *obj = new CCPopOverDialog();
    if (!obj->init(contentSize, direction)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
}


bool CCPopOverDialog::init(cocos2d::CCSize contentSize, CCPopoverArrowDirection direction)
{
    if (!CCLayerRGBA::init()) {
        return false;
    }
    
    setTouchEnabled(true);
    setTouchPriority(-2000);
    setTouchMode(kCCTouchesOneByOne);
    ignoreAnchorPointForPosition(false);

    m_autoBoundCalcuration = true;
    m_autoDismiss = true;
    m_direction = direction;
    m_positionAdjustment = CCPointZero;
    
    
    m_bg = extension::CCScale9Sprite::createWithSpriteFrameName("oldversion/_ground_9sprite_dialog.png");
    m_bg->setAnchorPoint(ccp(0.5f, 0.5f));
    m_bg->setContentSize(contentSize);
    m_bg->setPosition(contentSize*0.5f);
    this->addChild(m_bg, 0);

    m_arrowSprite = CCSprite::createWithSpriteFrameName("oldversion/_mini_ground_arrow_white.png");
    this->addChild(m_arrowSprite, 1);
    
    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    contentNode->setContentSize(contentSize);
    contentNode->setPosition(contentSize*0.5f);
    this->addChild(contentNode, 2);
    this->setContentSize(contentSize);
    
    return true;
}

void CCPopOverDialog::addChild(cocos2d::CCNode *child)
{
    addChild(child, child->getZOrder());
}

void CCPopOverDialog::addChild(cocos2d::CCNode *child, int zOrder)
{
    addChild(child, zOrder, child->getTag());
}

void CCPopOverDialog::addChild(cocos2d::CCNode *child, int zOrder, int tag)
{
    if (child == contentNode || child == m_bg || child == m_arrowSprite) {
        CCLayer::addChild(child, zOrder, tag);
    } else {
        contentNode->addChild(child, zOrder, tag);
    }
}

void CCPopOverDialog::presentPopover(cocos2d::CCNode *onNode, bool animated)
{
    presentPopover(CCDirector::sharedDirector()->getRunningScene(), onNode, animated);
}

void CCPopOverDialog::presentPopover(cocos2d::CCNode *parentNode, cocos2d::CCNode *onNode, bool animated)
{
    if (!onNode || !parentNode) return;

    CCScene *scene = CCDirector::sharedDirector()->getRunningScene();
    CCPoint pointOnScene = onNode->convertToWorldSpace(CCPointZero);

    if (!onNode->isIgnoreAnchorPointForPosition()) {
        pointOnScene.x += onNode->getContentSize().width*0.5f;
        pointOnScene.y += onNode->getContentSize().height*0.5f;
    }
    
    CCPopoverArrowDirection _direction  = m_direction;
    if (m_direction == CCPopoverArrowDirectionAny) {
        _direction = this->findDirection(pointOnScene,
                                         CCDirector::sharedDirector()->getRunningScene(),
                                         onNode,
                                         m_direction);
    }

    CCPoint point = onNode->convertToWorldSpace(CCPointZero);
    point = parentNode->convertToNodeSpace(point);
    
    if (!onNode->isIgnoreAnchorPointForPosition()) {
        point.x += onNode->getContentSize().width*0.5f;
        point.y += onNode->getContentSize().height*0.5f;
    }
    
    if (_direction == CCPopoverArrowDirectionDown ||
        _direction == CCPopoverArrowDirectionUp) {
        this->setContentSize(getContentSize()
                             + CCSize(0, m_arrowSprite->getContentSize().width));
    } else {
        this->setContentSize(getContentSize()
                             + CCSize(m_arrowSprite->getContentSize().width, 0));
    }
    
    CCSize contentSize = CCLayerRGBA::getContentSize();
    
    float shadowMargin = 10*sizeMultiplier;
    
    if (_direction == CCPopoverArrowDirectionUp) {
        
        point.y += onNode->getContentSize().height*0.5f;
        setAnchorPoint(ccp(0.5, 0));
        m_arrowSprite->setRotation(-90);
        m_arrowSprite->setPosition(ccp(contentSize.width*0.5f,
                                       m_arrowSprite->getContentSize().width*0.5f));
        
        CCPoint setPoint = m_arrowSprite->getPosition() + ccp(0,
                                                              m_arrowSprite->getContentSize().width*0.5f
                                                              + m_bg->getContentSize().height*0.5f
                                                              - shadowMargin
                                                              - EXTRA_ARROW_OFFSET);
        
        if (m_autoBoundCalcuration) {
            if (scene->getContentSize().width < (pointOnScene.x + m_bg->getContentSize().width*0.5f)) {
                setPoint.x -= (pointOnScene.x + m_bg->getContentSize().width*0.5f - scene->getContentSize().width);
            }
            
            if (0 > (pointOnScene.x - m_bg->getContentSize().width*0.5f)) {
                setPoint.x -= (pointOnScene.x - m_bg->getContentSize().width*0.5f);
            }
        }
        
        m_bg->setPosition(setPoint);
        
    } else if (_direction == CCPopoverArrowDirectionDown) {
        
        point.y -= onNode->getContentSize().height*0.5f;
        setAnchorPoint(ccp(0.5, 1));
        m_arrowSprite->setRotation(90);
        m_arrowSprite->setPosition(ccp(contentSize.width*0.5f,
                                       contentSize.height - m_arrowSprite->getContentSize().width*0.5f));
        
        CCPoint setPoint = m_arrowSprite->getPosition() - ccp(0,
                                                              m_arrowSprite->getContentSize().width*0.5f
                                                              + m_bg->getContentSize().height*0.5f
                                                              - shadowMargin
                                                              - EXTRA_ARROW_OFFSET);
        
        if (m_autoBoundCalcuration) {
            if (scene->getContentSize().width < (pointOnScene.x + m_bg->getContentSize().width*0.5f)) {
                setPoint.x -= (pointOnScene.x + m_bg->getContentSize().width*0.5f - scene->getContentSize().width);
            }
            
            if (0 > (pointOnScene.x - m_bg->getContentSize().width*0.5f)) {
                setPoint.x -= (pointOnScene.x - m_bg->getContentSize().width*0.5f);
            }
        }
        
        m_bg->setPosition(setPoint);
        
    } else if (_direction == CCPopoverArrowDirectionLeft) {
        
        point.x -= onNode->getContentSize().width*0.5f;
        setAnchorPoint(ccp(1, 0.5));
        m_arrowSprite->setFlipX(true);
        m_arrowSprite->setPosition(ccp(contentSize.width - m_arrowSprite->getContentSize().width*0.5f, contentSize.height*0.5f));
        
        CCPoint setPoint = m_arrowSprite->getPosition() - ccp(m_arrowSprite->getContentSize().width*0.5f
                                                              + m_bg->getContentSize().width*0.5f
                                                              - shadowMargin
                                                              - EXTRA_ARROW_OFFSET,
                                                              0);
        
        if (m_autoBoundCalcuration) {
            if (scene->getContentSize().height < (pointOnScene.y + m_bg->getContentSize().height*0.5f)) {
                setPoint.y -= (pointOnScene.y + m_bg->getContentSize().height*0.5f - scene->getContentSize().height);
            }
            
            if (0 > (pointOnScene.y - m_bg->getContentSize().height*0.5f)) {
                setPoint.y -= (pointOnScene.y - m_bg->getContentSize().height*0.5f);
            }
        }
        
        m_bg->setPosition(setPoint);
        
    } else if (_direction == CCPopoverArrowDirectionRight) {
        
        point.x += onNode->getContentSize().width*0.5f;
        setAnchorPoint(ccp(0, 0.5));
        m_arrowSprite->setRotation(0);
        m_arrowSprite->setPosition(ccp(m_arrowSprite->getContentSize().width*0.5f, contentSize.height*0.5f));
        
        CCPoint setPoint = m_arrowSprite->getPosition() + ccp(m_arrowSprite->getContentSize().width*0.5f
                                                              + m_bg->getContentSize().width*0.5f
                                                              - shadowMargin
                                                              - EXTRA_ARROW_OFFSET,
                                                              0);
        
        if (m_autoBoundCalcuration) {
            if (scene->getContentSize().height < (pointOnScene.y + m_bg->getContentSize().height*0.5f)) {
                setPoint.y -= (pointOnScene.y + m_bg->getContentSize().height*0.5f - scene->getContentSize().height);
            }
            
            if (0 > (pointOnScene.y - m_bg->getContentSize().height*0.5f)) {
                setPoint.y -= (pointOnScene.y - m_bg->getContentSize().height*0.5f);
            }
        }
        
        m_bg->setPosition(setPoint);
        
    }

    contentNode->setPosition(m_bg->getPosition());
    this->setPosition(point + m_positionAdjustment);
    if (!this->getParent()) {
        //fix zlayer by add
        parentNode->addChild(this, kCCPopOverDialogZOrder);
    }
    
    if (animated) {
        setScale(0.5f);
        runAction(CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f), 0.5f));
        CCModalController::showAllChildrensAnimated(this, 0.1f);
    } else {
        setScale(1);
    }
}

void CCPopOverDialog::dismissPopover(bool animated)
{
    this->dismissPopover(animated, true);
}

void CCPopOverDialog::dismissPopover(bool animated, bool removeSelf)
{
    if (animated) {
        this->setScale(1.0f);
        if (removeSelf) {
            this->runAction(CCSequence::create(CCScaleTo::create(0.1f, 0.5f),
                                               CCRemoveSelf::create(),
                                               NULL));
        } else {
            this->runAction(CCSequence::create(CCScaleTo::create(0.1f, 0.5f),
                                               CCHide::create(),
                                               NULL));
        }
        CCModalController::hideAllChildrensAnimated(this, 0.1f);
    } else {
        if (removeSelf) {
            this->removeFromParent();
        } else {
            this->setVisible(false);
        }
    }
}

void CCPopOverDialog::dismissAllPopovers() {
    
    CCScene* _scene = CCDirector::sharedDirector()->getRunningScene();
    if (!_scene) return;
    
    int n = _scene->getChildrenCount();
    for (int i=0; i<n; i++) {
        CCNode *_node = (CCNode*)_scene->getChildren()->objectAtIndex(i);
        if (_node->getZOrder() != kCCPopOverDialogZOrder) continue;
        CCPopOverDialog *_popOver = (CCPopOverDialog*)_node;
        if (typeid(*_popOver) != typeid(CCPopOverDialog)) continue;
        _popOver->dismissPopover(true);
    }
    
}

CCPopoverArrowDirection CCPopOverDialog::findDirection(cocos2d::CCPoint point, CCNode *parentNode, cocos2d::CCNode *onNode, CCPopoverArrowDirection direction)
{
    if (m_autoBoundCalcuration) {
        return findDirection(point, parentNode, onNode, direction, 0);
    } else {
        return direction == CCPopoverArrowDirectionAny ? CCPopoverArrowDirectionUp : direction;
    }
}


CCPopoverArrowDirection CCPopOverDialog::findDirection(cocos2d::CCPoint point, cocos2d::CCNode *parentNode, cocos2d::CCNode *onNode, CCPopoverArrowDirection direction, int count)
{
    if (count > 4) {
        return m_direction;
    }
    
//    CCLog_("point : %f,%f",point.x,point.y);

    
    
    CCRect boundingBox = CCRectZero;
//    boundingBox = parentNode->boundingBox();
//    boundingBox.size = parentNode->getContentSize()*parentNode->getScale();
//    CCLog_("boundingbox : %f,%f,%f,%f",boundingBox.origin.x,boundingBox.origin.y,boundingBox.size.width,boundingBox.size.height);
    boundingBox.size = parentNode->getContentSize();
    
    if (direction == CCPopoverArrowDirectionAny) {
        return findDirection(point, parentNode, onNode, CCPopoverArrowDirectionUp, ++count);
    } else if (direction == CCPopoverArrowDirectionUp) {
        
        CCPoint maxPoint = point + ccp(onNode->getContentSize().width*0.5f, onNode->getContentSize().height*0.5f + m_arrowSprite->getContentSize().width + m_bg->getContentSize().height);
        
        CCPoint maxPoint2 = point + ccp(-onNode->getContentSize().width*0.5f, onNode->getContentSize().height*0.5f + m_arrowSprite->getContentSize().width + m_bg->getContentSize().height);
        
        if (boundingBox.containsPoint(maxPoint) && boundingBox.containsPoint(maxPoint2)) {
            return direction;
        } else {
            return findDirection(point, parentNode, onNode, CCPopoverArrowDirectionDown, ++count);
        }
    } else if (direction == CCPopoverArrowDirectionDown) {
        
        CCPoint maxPoint = point + ccp(onNode->getContentSize().width*0.5f, -onNode->getContentSize().height*0.5f - m_arrowSprite->getContentSize().width - m_bg->getContentSize().height);
        
        CCPoint maxPoint2 = point + ccp(-onNode->getContentSize().width*0.5f, -onNode->getContentSize().height*0.5f - m_arrowSprite->getContentSize().width - m_bg->getContentSize().height);
        
        if (boundingBox.containsPoint(maxPoint) && boundingBox.containsPoint(maxPoint2)) {
            return direction;
        } else {
            return findDirection(point, parentNode, onNode, CCPopoverArrowDirectionLeft, ++count);
        }
    } else if (direction == CCPopoverArrowDirectionLeft) {
        
        CCPoint maxPoint = point + ccp(-onNode->getContentSize().width*0.5f - m_arrowSprite->getContentSize().width - m_bg->getContentSize().width, onNode->getContentSize().height*0.5f);
        
        CCPoint maxPoint2 = point + ccp(-onNode->getContentSize().width*0.5f - m_arrowSprite->getContentSize().width - m_bg->getContentSize().width, -onNode->getContentSize().height*0.5f);
        
        if (boundingBox.containsPoint(maxPoint) && boundingBox.containsPoint(maxPoint2)) {
            return direction;
        } else {
            return findDirection(point, parentNode, onNode, CCPopoverArrowDirectionRight, ++count);
        }
    } else if (direction == CCPopoverArrowDirectionRight) {
        CCPoint maxPoint = point + ccp(onNode->getContentSize().width*0.5f + m_arrowSprite->getContentSize().width + m_bg->getContentSize().width, onNode->getContentSize().height*0.5f);
        
        CCPoint maxPoint2 = point + ccp(onNode->getContentSize().width*0.5f + m_arrowSprite->getContentSize().width + m_bg->getContentSize().width, -onNode->getContentSize().height*0.5f);
        
        if (boundingBox.containsPoint(maxPoint) && boundingBox.containsPoint(maxPoint2)) {
            return direction;
        } else {
            return findDirection(point, parentNode, onNode, CCPopoverArrowDirectionUp, ++count);
        }
    } else {
        return m_direction;
    }
}

CCPoint CCPopOverDialog::findPosition(cocos2d::CCPoint point)
{
    return point;
}

bool CCPopOverDialog::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event)
{

    CCPoint location = this->convertTouchToNodeSpaceAR(touch);
    CCRect rect = this->boundingBox();
    rect.origin = CCPointZero - ccp(getContentSize().width*getAnchorPoint().x, getContentSize().height*getAnchorPoint().y);
    
//    if (m_autoDismiss) {
//        if (!rect.containsPoint(location)) {
//            this->dismissPopover(true);
//        }
//        return true;
//    } else {
    if (m_autoDismiss) {
        if (rect.containsPoint(location)) {
            return true;
        } else {
            this->setTouchEnabled(false);
            this->dismissPopover(true);
            return false;
        }
    } else {
        return false;
    }
//    }
}
