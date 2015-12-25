//
//  GachaItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#include "GachaItem.h"

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../Extensions/CCLabelHelper.h"

#include <spine/spine-cocos2dx.h>

USING_NS_CC;

using namespace extension;

GachaItem* GachaItem::create(GachaType type, GachaTypeNumber typeNumber, float height, int index) {
    
    GachaItem *obj = new GachaItem();
    if (!obj->init(type, typeNumber, height, index)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool GachaItem::init(GachaType type, GachaTypeNumber typeNumber, float height, int index)
{
    if (!CCNode::init()) {
        return false;
    }
    
    m_gachaType = type;
    
    m_index = index;
    
    this->setAnchorPoint(ccp(0.5f, 0));
    
    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    
    
    CCSprite *bg = CCSprite::createWithSpriteFrameName(type == GachaTypeFree ? "_gacha_pad_yellow.png" : "_gacha_pad_blue.png");
    
    contentNode->setContentSize(bg->getContentSize());
    
    if (typeNumber == GachaTypeNumberEven) {
        bg->setPosition(contentNode->getContentSize()*0.5f);
        contentNode->addChild(bg);
    }
    
    this->setContentSize(CCSizeMake(contentNode->getContentSize().width, height));
    
    contentNode->setPosition(ccp(this->getContentSize().width*0.5f, this->getContentSize().height - contentNode->getContentSize().height*0.5f));
    
    this->addChild(contentNode);
    
    itemContentNode = CCNode::create();
    itemContentNode->setPosition(contentNode->getContentSize()*0.5f);
    itemContentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    contentNode->addChild(itemContentNode);
    
//    CCSprite *test = CCSprite::createWithSpriteFrameName("_gacha_button_free.png");
//    test->setScale(0.5f);
//    test->setPosition(itemContentNode->getContentSize()*0.5f);
//    itemContentNode->addChild(test);
    
//    CCSkeletonAnimation *spine = g_sceneManager->sharedSkeletonData->getSkeleton("common/pet/", "dog2_3_2");
//    spine->setPosition(itemContentNode->getContentSize()*0.5f);
//    spine->setScale(0.5f);
//    itemContentNode->addChild(spine);
    
    petSprite = CCSprite::create();
    petSprite->setPosition(itemContentNode->getContentSize()*0.5f);
    itemContentNode->addChild(petSprite);
    
    if (m_gachaType == GachaTypeFree) {
        randomItemVO();
    } else {
        randomPetVO();
    }
    return true;
}

void GachaItem::updateRotation(float updateRotation, int selectedIndex, GDPetVO selectedPetVO, GDItemVO selectedItemVO, float remainingRotation)
{
    float rotation = this->getRotation();
    itemContentNode->setRotation(-updateRotation-rotation);
    
    int summaryRotation = (int)(updateRotation + rotation)%360;
    
    if (m_gachaType == GachaTypeFree) {
        if (summaryRotation > 0 && summaryRotation < 180) {
            if (!m_didChangeItem) {
                m_didChangeItem = true;
                if (m_index == selectedIndex) {
                    changeItemVO(selectedItemVO);
                } else {
                    randomItemVO();
                }
            }
        } else {
            m_didChangeItem = false;
        }
    } else {
        if (summaryRotation > 180 && summaryRotation < 360) {
            if (!m_didChangeItem) {
                m_didChangeItem = true;
                if (m_index == selectedIndex) {
                    changePetVO(selectedPetVO);
                } else {
                    randomPetVO();
                }
            }
        } else {
            m_didChangeItem = false;
        }
    }
}

void GachaItem::setRotation(float fRotation)
{
    CCNode::setRotation(fRotation);
    itemContentNode->setRotation(-fRotation);
}

void GachaItem::randomPetVO()
{
    int count = g_gameContent->Pet.size();
    GDPetVO _pet_vo;
    do {
        _pet_vo = g_gameContent->Pet[arc4random()%count];
    } while (_pet_vo.gacha_distribution == 0);
    changePetVO(_pet_vo);
}

void GachaItem::changePetVO(GDPetVO petVO)
{
    CCString *fileName = CCString::createWithFormat("%s.png",petVO.src.c_str());
    CCSpriteFrame *spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(fileName->getCString());
    if (spriteFrame) {
        petSprite->setVisible(true);
        petSprite->setDisplayFrame(spriteFrame);
    } else {
        petSprite->setVisible(false);
    }
}

void GachaItem::randomItemVO()
{
    int count = g_gameContent->Item.size();
    GDItemVO _item_vo = g_gameContent->Item[arc4random()%count];
    changeItemVO(_item_vo);
}

void GachaItem::changeItemVO(GDItemVO itemVO)
{
    CCString *fileName = CCString::createWithFormat("%s",itemVO.src.c_str());
    CCSpriteFrame *spriteFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(fileName->getCString());
    if (spriteFrame) {
        petSprite->setVisible(true);
        petSprite->setDisplayFrame(spriteFrame);
    } else {
        petSprite->setVisible(false);
    }
}