//
//  CCLabelHelper.cpp
//  Cheetah
//
//  Created by Tanapon on 3/6/57 BE.
//
//

#include "CCLabelHelper.h"
USING_NS_CC;

extern float sizeMultiplier;

// DEFAULT SCALE TO IPHONE

CCLabelBMFont *CCLabelHelper::createBMFont(const char *str, StyleLabelType labelStyle)
{
    float fontSize;
    bool hasShadow;
    cocos2d::ccColor3B color = ccc3(255, 255, 255);
    
    if (labelStyle == StyleDialogTitle)
    {
        fontSize = 60.0f;
        hasShadow = true;
        color = kTextColorWhite;
    }
    else if (labelStyle == StyleDialogTextLight)
    {
        fontSize = 45.0f;
        hasShadow = false;
        color = kTextColorLight;
    }
    else if (labelStyle == StyleDialogTextDark)
    {
        fontSize = 45.0f;
        hasShadow = false;
        color = kTextColorDark;
    }
    else if (labelStyle == StyleMiniDialogTitle)
    {
        fontSize = 60.0f;
        hasShadow = true;
        color = kTextColorWhite;
    }
    else if (labelStyle == StyleButtonCoinAndGems)
    {
        fontSize = 60.0f;
        hasShadow = true;
    }
    else if (labelStyle == StyleButtonCoinAndGemsNotEnough)
    {
        fontSize = 60.0f;
        hasShadow = true;
        color = kTextColorRed;
    }
    else if (labelStyle == StyleTextAboveButton)
    {
        fontSize = 45.0f;
        hasShadow = false;
        color = kTextColorLightBrown;
    }
    else
    {
        fontSize = 60.0f; //default value in case of error
        hasShadow = true; //default value in case of error
        perror("style is invalid");
    }
    
    return createBMFont(DEFAULT_SCALING, str, fontSize, hasShadow, color, kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(const char *str, float fontSize, const cocos2d::ccColor3B &color)
{
    return createBMFont(DEFAULT_SCALING, str, fontSize, false, color, kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(const char *str, float fontSize, bool hasShadow)
{
    return createBMFont(DEFAULT_SCALING, str, fontSize, hasShadow, ccc3(255, 255, 255), kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color)
{
    return createBMFont(DEFAULT_SCALING, str, fontSize, hasShadow, color, kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, float width, cocos2d::CCTextAlignment alignment)
{
    return createBMFont(DEFAULT_SCALING, str, fontSize, hasShadow, color, width, alignment);
}

// WITH SCALE TO IPHONE

CCLabelBMFont *CCLabelHelper::createBMFont(float fontScale, const char *str, float fontSize, const cocos2d::ccColor3B &color)
{
    return createBMFont(fontScale, str, fontSize, false, color, kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow)
{
    return createBMFont(fontScale, str, fontSize, hasShadow, ccc3(255, 255, 255), kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

CCLabelBMFont *CCLabelHelper::createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color)
{
    return createBMFont(fontScale, str, fontSize, hasShadow, color, kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
}

cocos2d::CCLabelBMFont* CCLabelHelper::createGatchaFont(const char *str, float fontSize)
{
    float finalFontScale = 1.0f;
    CCLabelHelper::getFontNameAndScale(DEFAULT_SCALING, fontSize, true, &finalFontScale);
    std::string fontName = "fonts/gacha_fontprice.fnt";
    CCLabelBMFont* label = CCLabelBMFont::create(str, fontName.c_str(), kCCLabelAutomaticWidth, kCCTextAlignmentLeft);
    label->setScale(finalFontScale);
    return label;
}

CCLabelBMFont *CCLabelHelper::createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, float width, cocos2d::CCTextAlignment alignment)
{
    float finalFontScale = 1.0f;
    
    std::string fontName = CCLabelHelper::getFontNameAndScale(fontScale, fontSize, hasShadow, &finalFontScale);
    
    CCLabelBMFont* label = CCLabelBMFont::create(str, fontName.c_str(), width, alignment);
    label->setScale(finalFontScale);
    label->setColor(color);
    return label;
}

std::string CCLabelHelper::getFontNameAndScale(float fontScale, float fontSize, bool hasShadow, float *finalScale)
{
    if (fontScale == DEFAULT_SCALING)
    {
        fontSize *= sizeMultiplier;
    }
    else
    {
        fontSize *= fontScale;
    }
    
    float scale;
    
    std::string fontName;
    
    if (hasShadow) {
        if (fontSize <= 26.0f)
        {
            fontName = "fonts/dimbo_shadow_24.fnt";
            scale = fontSize / 24.0f;
        }
        else if (fontSize <= 41.0f)
        {
            fontName = "fonts/dimbo_shadow_40.fnt";
            scale = fontSize / 40.0f;
        }
        else if (fontSize <= 51.0f)
        {
            fontName = "fonts/dimbo_shadow_50.fnt";
            scale = fontSize / 50.0f;
        }
        else if (fontSize <= 62.0f)
        {
            fontName = "fonts/dimbo_shadow_60.fnt";
            scale = fontSize / 60.0f;
        }
        else if (fontSize <= 72.0f)
        {
            fontName = "fonts/dimbo_shadow_70.fnt";
            scale = fontSize / 70.0f;
        }
        else if (fontSize <= 81.0f)
        {
            fontName = "fonts/dimbo_shadow_80.fnt";
            scale = fontSize / 80.0f;
        }
        else //if (fontSize <= 110.0f)
        {
            fontName = "fonts/dimbo_shadow_110.fnt";
            scale = fontSize / 110.0f;
        }
    }
    else
    {
        if (fontSize <= 26.f)
        {
            fontName = "fonts/dimbo_23_4.fnt";
            scale = fontSize / 23.4f;
        }
        else if (fontSize <= 41.0f)
        {
            fontName = "fonts/dimbo_40.fnt";
            scale = fontSize / 40.0f;
        }
        else //if (fontSize <= 50.0f)
        {
            fontName = "fonts/dimbo_50.fnt";
            scale = fontSize / 50.0f;
        }
    }
    
    if (scale >= 0.96f && scale <= 1.04f) scale = 1.0f;
    
    *finalScale = scale;
    
    return fontName;
}




