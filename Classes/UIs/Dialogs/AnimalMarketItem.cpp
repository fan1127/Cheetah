//
//  AnimalMarketItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 2/26/2557 BE.
//
//

#include "AnimalMarketItem.h"

#include <spine/spine-cocos2dx.h>

#include "../../Helpers/CreateSpriteHelper.h"

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
#include "../../Helpers/CustomEventsHelper.h"
#include "../../Helpers/UIHelper.h"

extern PlayerState* g_playerState;

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

//extern CCPoint screenFactor;
extern float sizeMultiplier;

AnimalMarketItem *AnimalMarketItem::create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool isSpine_) {
    
    AnimalMarketItem *pRet = new AnimalMarketItem();
    pRet->init(playerState_, vo_, pet_, false, isSpine_, false);
    pRet->autorelease();
    return pRet;
    
}

AnimalMarketItem *AnimalMarketItem::create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_) {
    
    AnimalMarketItem *pRet = new AnimalMarketItem();
    pRet->init(playerState_, vo_, pet_, mask_, isSpine_, false);
    pRet->autorelease();
    return pRet;
    
}

AnimalMarketItem *AnimalMarketItem::create(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_, bool questionMark_) {
    
    AnimalMarketItem *pRet = new AnimalMarketItem();
    pRet->init(playerState_, vo_, pet_, mask_, isSpine_, questionMark_);
    pRet->autorelease();
    return pRet;
    
}

bool AnimalMarketItem::init(PlayerState* playerState_, GDPetVO vo_, Pet *pet_, bool mask_, bool isSpine_, bool questionMark_) {
    
    m_playerState = playerState_;
    
    if (questionMark_) {
        if (!CCNikScrollViewButton::init(UIHelper::getSprite(UIHelper::PetShop_uiHabitatQuestionmark), NULL)) {
            return false;
        }
    } else {
        if (pet_) {
            if (!CCNikScrollViewButton::init(UIHelper::getSprite(UIHelper::PetShop_uiHabitat), NULL)) {
                return false;
            }
        }else {
            if (!CCNikScrollViewButton::init(UIHelper::getSprite(UIHelper::PetShop_uiHabitat), NULL)) {
                return false;
            }
        }
    }

    vo = vo_;
    pet = pet_;
    double _petScale = sizeMultiplier * 0.712251282f;
    
    m_questionMark = questionMark_;
    scheduleUpdate();
    
    m_group = CCNode::create();
    m_group->setPosition(getContentSize()*0.5f);
    addChild(m_group);
    
    
    if (!questionMark_) {
        if (isSpine_) {
            CCSkeletonAnimation *_petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, vo.src.c_str());
            _petSpine->setScale(_petScale);
            _petSpine->setAnimation("habitat_idle_stand", true);
            _petSpine->timeScale = 1.0f;
            if (mask_) _petSpine->setColor(ccBLACK);
            m_group->addChild(_petSpine);
        } else {
            CCString *fileName = CCString::createWithFormat("%s.png",vo.src.c_str());
            CCLog_("animalpic:%s",vo.src.c_str());
            CCSprite *_animal_pic = CCSprite::createWithSpriteFrameName(fileName->getCString());
            _animal_pic->setPosition(ccp(-12.0f * sizeMultiplier,
                                          41.0f * sizeMultiplier));
            _animal_pic->setScale(0.8f);
            if (mask_) _animal_pic->setColor(ccBLACK);
            m_group->addChild(_animal_pic);
        }
    }
    
    CCString* str;
    
    if (pet_) {
        if (NEXT_EVO_IS_UNKNOWN) {
            str = CCString::create("??");
        } else {
            if (ANIMAL_IS_AT_MAX_LEVEL) {
                str = CCString::createWithFormat("%d",
                                                 ((int)pet_->ppVo.top_speed+
                                                  (int)pet_->ppVo.corner_speed+
                                                  (int)pet_->ppVo.jumping));
            } else {
                str = CCString::createWithFormat("%d",
                                                 ((int)vo_.base_top_speed+
                                                  (int)vo_.base_corner_speed+
                                                  (int)vo_.base_jumping));
            }
        }
    } else {
        str = CCString::createWithFormat("%d",
                                         ((int)vo_.base_top_speed+
                                          (int)vo_.base_corner_speed+
                                          (int)vo_.base_jumping));
    }
    
    horsePower = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), vo_.rarity_stars);
    horsePower->setPosition(ccp(0,
                                -this->getContentSize().height*0.25f));
    horsePower->setScale(0.75f);
    m_group->addChild(horsePower);
    
    bool _canTrade = true;
    bool need_market_lv = false;
    
    Building* _building_market = m_playerState->getBuilding(kBuildingFusion, 0);
    
    int _unlocks_at_level = 0;
    
    if (pet) { //evolve
        
        //not enough items
        for (vector<IntKeyValue>::iterator it = pet->pVo.items_needed_to_evolve.begin(); it != pet->pVo.items_needed_to_evolve.end(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    int _item_amount = m_playerState->getNItem(_item_needed.key);
                    if (_item_amount < _item_needed.value) {
                        _canTrade = false;
                        break;
                    }
                }
            }
        }
//        //not enough coins
//        if (m_playerState->player.balance.gold < pet->pVo.coins_needed_to_evolve) {
//            canTrade = false;
//        }
        //pet level not at max
        if (pet->ppVo.level < pet->pVo.max_level) {
            _canTrade = false;
        }
        //market level not high enough
        if (pet->pVo.next_evolution.size() > 1) {
            //Find highest unlocks_at_level
            for (int i=0; i<pet->pVo.next_evolution.size(); i++) {
                FloatKeyValue _tmp_fkv = pet->pVo.next_evolution[i];
                if (_tmp_fkv.key != "") {
                    GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
                    if (_unlocks_at_level < _tmp_pet_vo.unlocks_at_level) {
                        _unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
                    }
                }
            }
        } else {
            FloatKeyValue _tmp_fkv = pet->pVo.next_evolution[0];
            GDPetVO _tmp_pet_vo = g_gameContent->getPetVO(_tmp_fkv.key);
            _unlocks_at_level = _tmp_pet_vo.unlocks_at_level;
        }
        if (_building_market->pbVo.level < _unlocks_at_level) {
            _canTrade = false;
            need_market_lv = true;
        }
        
    }else { //fuse
        
        //not enough items
        for (vector<IntKeyValue>::iterator it = vo.items_needed_for_fusion.begin(); it != vo.items_needed_for_fusion.end(); ++it) {
            IntKeyValue _item_needed = (IntKeyValue)(*it);
            if (_item_needed.key != "") {
                GDItemVO _itvo = g_gameContent->getItemVO(_item_needed.key);
                if (_itvo.type != "") {
                    int _item_amount = m_playerState->getNItem(_item_needed.key);
                    if (_item_amount < _item_needed.value) {
                        _canTrade = false;
                        break;
                    }
                }
            }
        }
//        //not enough coins
//        if (m_playerState->player.balance.gold < vo.coins_needed_for_fusion) {
//            canTrade = false;
//        }
        //market level not high enough
        if (_building_market->pbVo.level < vo.unlocks_at_level) {
            _canTrade = false;
            need_market_lv = true;
            _unlocks_at_level = vo.unlocks_at_level;
        }
    }
    
    if (need_market_lv) {
        CREATE_CHAR_BUFFER(_buffer, 32);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["need_building_lv_short"].c_str(), _building_market->bVo.title.c_str(), _unlocks_at_level);
        
        needMarketLvLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeSmall, true);
        needMarketLvLabel->setColor(kTextColorOrange);
        needMarketLvLabel->setAnchorPoint(ccp(0.5f, 1.0f));
        needMarketLvLabel->setPosition(horsePower->getPositionX(),
                                       horsePower->getPositionY()
                                       - this->getContentSize().height*0.1f);
//        horsePower->setPosition(ccp(horsePower->getPosition().x,
//                                    horsePower->getPosition().y +
//                                    this->getContentSize().height*0.1f));
        m_group->addChild(needMarketLvLabel);
    }
    
    canTrade = _canTrade;
    
    if (canTrade) {
        CCSprite *_checkSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::HUDCheckmarkBig));
        _checkSprite->setPosition(ccp(getContentSize().width*0.2f,
                                      0));
        m_group->addChild(_checkSprite);
    }
    
    return true;
}

void AnimalMarketItem::update(float delta_)
{
#define OPACITY_LEVEL 125
    
    if (m_questionMark) {
        CCSprite *normalSprite = dynamic_cast<CCSprite*>(m_normalSprite);
        if (normalSprite)
            normalSprite->setOpacity(OPACITY_LEVEL);
        needMarketLvLabel->setOpacity(OPACITY_LEVEL);
        
        CCObject *object = NULL;
        CCARRAY_FOREACH(horsePower->getChildren(), object)
        {
            CCSprite* sprite = dynamic_cast<CCSprite*>(object);
            if (sprite) {
                sprite->setOpacity(OPACITY_LEVEL);
            }
            CCLabelBMFont* label = dynamic_cast<CCLabelBMFont*>(object);
            if (label) {
                label->setOpacity(OPACITY_LEVEL);
            }
        }
    }
}

void AnimalMarketItem::animateIdleWithDelay(float delay_)
{
    runAction(CCSequence::create(CCDelayTime::create(delay_),
                                 CCCallFunc::create(this, callfunc_selector(AnimalMarketItem::animateIdle)),
                                 NULL));
}

void AnimalMarketItem::animateIdle()
{
    m_group->stopAllActions();
    m_group->runAction(CustomEventsHelper::Instance().getIdleAnimationAnimateOnce());
}

