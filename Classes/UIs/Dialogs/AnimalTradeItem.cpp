//
//  AnimalTradeItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/27/2557 BE.
//
//

#include "AnimalTradeItem.h"

USING_NS_CC;

#include "../../AppMacros.h"
extern float sizeMultiplier;

//extern CCPoint screenFactor;

//#include "../../Entities/PlayerState.h"
//extern PlayerState* g_playerState;
#include "../../Helpers/CCStringHelper.h"

USING_NS_CC;

AnimalTradeItem *AnimalTradeItem::create(GDItemVO itemVO_, IntKeyValue item_, cocos2d::CCSize size, PlayerState* playerState_) {
    
    AnimalTradeItem *pRet = new AnimalTradeItem();
    pRet->init(itemVO_, item_, size, playerState_);
    pRet->autorelease();
    return pRet;
    
}

bool AnimalTradeItem::init(GDItemVO itemVO_, IntKeyValue item_, cocos2d::CCSize size, PlayerState* playerState_) {
    
    if (!CCNikScrollViewButton::init(NULL, NULL)) {
        return false;
    }
    
    setContentSize(size);
    setAnchorPoint(ccp(0.5f, 0.5f));
    
    m_complete = false;
    itemVO = itemVO_;
    item = item_;
    m_playerState = playerState_;
    
    CCSprite *_itemSprite = CCSprite::createWithSpriteFrameName(itemVO.src.c_str());
    if (!_itemSprite) {
        _itemSprite = CCSprite::createWithSpriteFrameName("yarn.png");
    }
    _itemSprite->setPosition(ccp(getContentSize().width*0.5f, getContentSize().height*0.6f));
    addChild(_itemSprite);

    int _item_amount = m_playerState->getNItem(item.key);
    CCString *amountString = CCString::createWithFormat("%d/%d",_item_amount,  item.value );
    CCLabelBMFont *_amountLabel = CCLabelHelper::createBMFont(amountString->getCString(), kFontSizeMedium, kTextColorLightBrown);
    _amountLabel->setPosition(_itemSprite->getPosition()+ccp(0.0f,-(_itemSprite->getContentSize().height + _amountLabel->getContentSize().height)*0.5f));
    addChild(_amountLabel);
    
    CCLog_("%s:amount:%d,value:%d",itemVO.src.c_str(),_item_amount,item.value);
    if (_item_amount >= item.value) {
        CCLog_("COMPLETE-> %s:amount:%d,value:%d",itemVO.src.c_str(),_item_amount,item.value);
        
        CCSprite *_mark = UIHelper::getSprite(UIHelper::UISprites(UIHelper::HUDCheckmarkSmall));
        _mark->setPosition(_itemSprite->getPosition()+ccp(_itemSprite->getContentSize().width*0.25f,-_itemSprite->getContentSize().height*0.25f));
        this->addChild(_mark);
        m_complete = true;
    }

    return true;
    
}

bool AnimalTradeItem::isComplete() {
    
    return m_complete;
    
}
