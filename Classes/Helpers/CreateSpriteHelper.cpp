//
//  CreateSpriteHelper.cpp
//  Cheetah
//
//  Created by Kaweerut on 1/15/57 BE.
//
//

#include "CreateSpriteHelper.h"
#include "UIHelper.h"

extern float sizeMultiplier;

CCSprite* CreateSpriteHelper::getHorsePowerSprite(const char *power, int stars, HorsePowerCOLOR color)
{
    CCSprite *spriteGroup = CCSprite::create();
    
    CCSprite *ribbon = NULL;

    if (color==HorsePowerCOLOR_BLUE)
    {
        ribbon = UIHelper::getSprite(UIHelper::NSClassesRibbonFriend);
    }
    else
    {
        ribbon = UIHelper::getSprite(UIHelper::NSClassesRibbonEnemy);
    }

    spriteGroup->addChild(ribbon);
    
    CCSprite *baseClassSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NSClassesE));
    float baseClassSpriteWidth = baseClassSprite->getContentSize().width;
    baseClassSprite->removeFromParent();
    baseClassSprite = NULL;

    CCSprite *classSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::NSClassesE + stars - 1));
    spriteGroup->addChild(classSprite);
    
    UIHelper::setTwoNodesToMiddlePosition(baseClassSpriteWidth,
                                          ribbon->getContentSize().width*ribbon->getScale(),
                                          classSprite,
                                          ribbon,
                                          -25.0f*sizeMultiplier,
                                          ccp(-20.0f*sizeMultiplier,
                                              0));
    

    CCLabelBMFont *lblAverage = CCLabelHelper::createBMFont(power, kFontSizeSmall, kTextColorWhite);
    lblAverage->setTag(LABEL_AVERAGE_TAG);
    lblAverage->setPosition(ribbon->getPosition());
    spriteGroup->addChild(lblAverage);

    return spriteGroup;
}
