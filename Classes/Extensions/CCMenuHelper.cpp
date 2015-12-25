//
//  CCMenuHelper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 7/28/2557 BE.
//
//

#include "CCMenuHelper.h"
USING_NS_CC;


CCMenu *CCMenuHelper::createMenu(const char* frameName, CCObject* target, SEL_MenuHandler selector)
{
    return CCMenuHelper::createMenu(CCSprite::createWithSpriteFrameName(frameName), target, selector);
}

CCMenu *CCMenuHelper::createMenu(CCSprite *sprite, CCObject* target, SEL_MenuHandler selector)
{
    CCMenuItemScaleSprite *menuSprite = CCMenuItemScaleSprite::create(sprite, target, selector);
//    menuSprite->setTag(CCMENUHELPER_TAG_ITEMSCALESPRITE);
    CCMenu *menu = CCMenu::create(menuSprite, NULL);
    menu->setContentSize(sprite->getContentSize());
    return menu;
}

CCMenuItem *CCMenuHelper::getMenuItemScaleSprite(CCMenu *menu)
{
    return CCMenuHelper::getMenuItemScaleSprite(menu, 0);
}

CCMenuItem *CCMenuHelper::getMenuItemScaleSprite(CCMenu *menu, int index)
{
    return (CCMenuItem*)menu->getChildren()->objectAtIndex(index);
//    return (CCMenuItem*)menu->getChildByTag(CCMENUHELPER_TAG_ITEMSCALESPRITE);
}
