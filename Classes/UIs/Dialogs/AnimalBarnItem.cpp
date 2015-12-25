//
//  AnimalBarnItem.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/4/2557 BE.
//
//

#include "AnimalBarnItem.h"

#include <spine/spine-cocos2dx.h>

#include "../../Helpers/CreateSpriteHelper.h"

#include "../../SceneManager.h"
extern SceneManager *g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

USING_NS_CC;
using namespace std;
using namespace cocos2d::extension;

//extern CCPoint screenFactor;
extern float retinaMultiplier;
extern float sizeMultiplier;

AnimalBarnItem *AnimalBarnItem::create(Pet *pet_)
{
    AnimalBarnItem *pRet = new AnimalBarnItem();
    pRet->init(pet_);
    pRet->autorelease();
    return pRet;
}

bool AnimalBarnItem::init(Pet *pet_)
{
    if (!CCNikScrollViewButton::init(UIHelper::getSprite(UIHelper::PetShop_uiHabitat), NULL))
    {
        return false;
    }

    pet = pet_;
    
    CCString *fileName = CCString::createWithFormat("%s.png",pet->pVo.src.c_str());
    CCLog_("animalpic:%s",pet->pVo.src.c_str());
    CCSprite *_animal_pic = CCSprite::createWithSpriteFrameName(fileName->getCString());
    _animal_pic->setPosition(this->getContentSize()*0.5f
                             + ccp(-14.0f * sizeMultiplier,
                                   50.0f * sizeMultiplier));
    _animal_pic->setScale(0.8f);
    this->addChild(_animal_pic);
    
    CCString *str = CCString::createWithFormat("%d",
                                               ((int)pet->ppVo.top_speed+
                                                (int)pet->ppVo.corner_speed+
                                                (int)pet->ppVo.jumping));
    
    CCSprite *horsePower = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), pet->pVo.rarity_stars);
    //horsePower->setScale(2.1f-pow(1.1f, sizeMultiplier-1.0f));
    horsePower->setPosition(ccp(this->getContentSize().width*0.5f,this->getContentSize().height*0.25f));
    horsePower->setScale(0.75f);
    this->addChild(horsePower);
    
    return true;
}
