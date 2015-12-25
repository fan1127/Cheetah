//
//  CCTiledDialogBG.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 1/7/2557 BE.
//
//

#include "CCTiledDialogBG.h"
USING_NS_CC;
using namespace std;

//CCTiledDialogBG *CCTiledDialogBG::createWithSpriteFileNames(const char *sprite_head_filename_, const char *sprite_body_filename_, const char *sprite_foot_filename_) {
//    
//    CCTiledDialogBG *pRet = new CCTiledDialogBG();
//    pRet->initWithSpriteFileNames(sprite_head_filename_, sprite_body_filename_, sprite_foot_filename_);
//    pRet->autorelease();
//    return pRet;
//    
//}
//
//CCTiledDialogBG *CCTiledDialogBG::createWithSpriteFrameNames(const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_buttom_framename_) {
//    
//    CCTiledDialogBG *pRet = new CCTiledDialogBG();
//    pRet->initWithSpriteFrameNames(sprite_head_framename_, sprite_body_framename_, sprite_buttom_framename_);
//    pRet->autorelease();
//    return pRet;
//    
//}

CCTiledDialogBG *CCTiledDialogBG::createWithSpriteFrameNamesInBatch(const char *texture_filename_, const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_buttom_framename_) {
    
    CCTiledDialogBG *pRet = new CCTiledDialogBG();
    pRet->initWithSpriteFrameNamesInBatch(texture_filename_, sprite_head_framename_, sprite_body_framename_, sprite_buttom_framename_);
    pRet->autorelease();
    return pRet;
    
}

//bool CCTiledDialogBG::initWithSpriteFileNames(const char *sprite_head_filename_, const char *sprite_body_filename_, const char *sprite_buttom_filename_) {
//    
//    if (!CCNode::init()) {
//        return false;
//    }
//    
//    
//    
//    return true;
//    
//}
//
//bool CCTiledDialogBG::initWithSpriteFrameNames(const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_buttom_framename_) {
//    
//    if (!CCNode::init()) {
//        return false;
//    }
//    
//    
//    
//    return true;
//
//}

bool CCTiledDialogBG::initWithSpriteFrameNamesInBatch(const char *texture_filename_, const char *sprite_head_framename_, const char *sprite_body_framename_, const char *sprite_foot_framename_) {
    
    if (!CCNode::init()) {
        return false;
    }
    
    m_batchNode     = CCSpriteBatchNode::create(texture_filename_, 3);
    m_head          = CCSprite::createWithSpriteFrameName(sprite_head_framename_);
    m_body          = CCSprite::createWithSpriteFrameName(sprite_body_framename_);
    m_body_glue[0]  = CCSprite::createWithSpriteFrameName(sprite_body_framename_);
    m_body_glue[1]  = CCSprite::createWithSpriteFrameName(sprite_body_framename_);
    m_body          = CCSprite::createWithSpriteFrameName(sprite_body_framename_);
    m_foot          = CCSprite::createWithSpriteFrameName(sprite_foot_framename_);
    
    this->addChild(m_batchNode);
    setSize(CCSize((m_head->getContentSize().width+m_body->getContentSize().width+m_foot->getContentSize().width)/3,
                   (m_head->getContentSize().height+m_body->getContentSize().height+m_foot->getContentSize().height)));
    
    return true;

}

void CCTiledDialogBG::setSize(CCSize size_) {
    
    CCSize _body_expected_size = CCSize(size_.width,
                                        size_.height-m_head->getContentSize().height-m_foot->getContentSize().height);
    CCPoint _body_expected_scale = ccp(_body_expected_size.width/m_body->getContentSize().width,
                                       _body_expected_size.height/m_body->getContentSize().height);
    m_body->setScaleX(_body_expected_scale.x);
    m_body->setScaleY(_body_expected_scale.y);

    m_batchNode->removeAllChildren();
    
    m_body_glue[0]->setPositionY( (_body_expected_size.height-m_body_glue[0]->getContentSize().height)*0.55f);
    m_batchNode->addChild(m_body_glue[0]);
    
    m_body_glue[1]->setPositionY(-(_body_expected_size.height-m_body_glue[1]->getContentSize().height)*0.55f);
    m_batchNode->addChild(m_body_glue[1]);
    
    m_body->setPosition(CCPointZero);
    m_batchNode->addChild(m_body);
    
    m_head->setPositionY((size_.height-m_head->getContentSize().height)*0.5f);
    m_batchNode->addChild(m_head);
    
    m_foot->setPositionY(-(size_.height-m_foot->getContentSize().height)*0.5f);
    m_batchNode->addChild(m_foot);
    
    this->setContentSize(size_);
    
}

void CCTiledDialogBG::setHeight(float height_) {
    
    setSize(CCSize(this->getContentSize().width, height_));
    
}

void CCTiledDialogBG::setWidth(float width_) {
    
    setSize(CCSize(width_, this->getContentSize().height));
    
}




