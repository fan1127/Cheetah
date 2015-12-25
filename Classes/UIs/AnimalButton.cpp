//
//  AnimalButton.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 9/22/2557 BE.
//
//

#include "AnimalButton.h"
#include "../Extensions/CCMenuHelper.h"
#include "../Helpers/CreateSpriteHelper.h"
#include "../Helpers/UIHelper.h"

USING_NS_CC;

extern float sizeMultiplier;

AnimalButton* AnimalButton::createWithPet(Pet *pet_, CCObject* target_, SEL_MenuHandler selector_)
{
    AnimalButton *btn = AnimalButton::create();
    
    CCString *fileName = CCString::createWithFormat("%s.png",pet_->pVo.src.c_str());
    //btn->btnSquare = UIHelper::getSprite(UIHelper::RaceElementsInactiveAnimal);
//    btn->setScale(btn->getScale()*0.8f);
    btn->btnSquare = CCSprite::createWithSpriteFrameName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::RaceElementsInactiveAnimal)));
    
    btn->itemSize = btn->btnSquare->getContentSize();
    
    btn->animalPic = CCSprite::createWithSpriteFrameName(fileName->getCString());
    btn->animalPic->setPosition(ccp(btn->btnSquare->getContentSize().width*0.5f,
                                      (btn->btnSquare->getContentSize().height*0.5f)
                                      + (29.0f * sizeMultiplier)
                                      ));
    btn->animalPic->setScale(0.82f);
    btn->btnSquare->addChild(btn->animalPic);
    
    CCString *str = CCString::createWithFormat("%d",((int)pet_->ppVo.top_speed+
                                                     (int)pet_->ppVo.corner_speed+
                                                     (int)pet_->ppVo.jumping));
    
    btn->horsePowerSprite = CreateSpriteHelper::getHorsePowerSprite(str->getCString(), pet_->pVo.rarity_stars);
    btn->horsePowerSprite->setScale(0.648f);
    btn->horsePowerSprite->setPosition(ccp(btn->animalPic->getPositionX()+(11.0f * sizeMultiplier),
                                             btn->animalPic->getPositionY()+(-96.0f * sizeMultiplier)));
    btn->btnSquare->addChild(btn->horsePowerSprite);
    
    CCMenuItemSprite *_menuItem_selectPet = CCMenuItemScaleSprite::create(btn->btnSquare, target_, selector_);
    
    btn->addChild(_menuItem_selectPet);
    
    btn->setTouchPriority(HUD_MENU_TOUCH_PRIORITY);

    return btn;
}

void AnimalButton::toggleButtonActive(bool active_)
{
    if (active_) {
        animalPic->setVisible(false);
        horsePowerSprite->setVisible(false);
        //btnSquare->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsActiveAnimal));
        btnSquare->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::RaceElementsActiveAnimal))));
    } else {
        animalPic->setVisible(true);
        horsePowerSprite->setVisible(true);
        //btnSquare->setDisplayFrame(UIHelper::getSpriteFrame(UIHelper::RaceElementsInactiveAnimal));
        btnSquare->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::RaceElementsInactiveAnimal))));
    }
}

void AnimalButton::setHorsePowerLabelString(const char *string)
{
    CCLabelBMFont *label = (CCLabelBMFont*)horsePowerSprite->getChildByTag(LABEL_AVERAGE_TAG);
    label->setString(string);
}
