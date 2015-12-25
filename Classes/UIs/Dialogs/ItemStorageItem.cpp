//
//  ItemStorageItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#include "ItemStorageItem.h"

#include <spine/spine-cocos2dx.h>

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../AppMacros.h"
extern float sizeMultiplier;

#include "../../Helpers/CCStringHelper.h"

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

ItemStorageItem *ItemStorageItem::create(IntKeyValue *item_, GDItemVO itvo_, CCSize size_) {
    
    ItemStorageItem *pRet = new ItemStorageItem();
    pRet->init(item_, itvo_, size_);
    pRet->autorelease();
    return pRet;
    
}

bool ItemStorageItem::init(IntKeyValue *item_, GDItemVO itvo_, CCSize size_) {
    if (!CCNikScrollViewButton::init()) {
        return false;
    }
    
    setContentSize(size_);
    
    item = item_;
    itvo = itvo_;
    
    CCSprite *_itemSprite = CCSprite::createWithSpriteFrameName(itvo.src.c_str());
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, "%d", item->value);
    
    CCLabelBMFont *_amountLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeMedium, kTextColorLightBrown);

    CCPoint point = CCPointZero;
    point.x = getContentSize().width*0.5f;
    point.y = (this->getContentSize().height - (_itemSprite->getContentSize().height + _amountLabel->getContentSize().height))/2.f + _amountLabel->getContentSize().height*0.5f;
    _amountLabel->setPosition(point);
    _itemSprite->setPosition(_amountLabel->getPosition() + ccp(0, _amountLabel->getContentSize().height*0.4f + _itemSprite->getContentSize().height*0.5f));
    
    this->addChild(_itemSprite);
    this->addChild(_amountLabel);
    
    return true;
}