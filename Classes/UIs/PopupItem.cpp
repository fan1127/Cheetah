//
//  PopupItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/22/2557 BE.
//
//

#include "PopupItem.h"

PopupItem* PopupItem::create(){
    
    PopupItem *pRet = new PopupItem();
    pRet->init();
    pRet->autorelease();
    return pRet;
}
bool PopupItem::init() {
    
    if (!CCNode::init()) {
        return false;
    }
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    CCSprite *_background = CCSprite::createWithSpriteFrameName("_bubble_1.png");
    this->setContentSize(_background->getContentSize());
    _background->setPosition(this->getContentSize()*0.5f);
    this->addChild(_background);
    
    
    return true;
}
