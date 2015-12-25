//
//  CCLabelHelper.h
//  Cheetah
//
//  Created by Tanapon on 3/6/57 BE.
//
//

#ifndef __Cheetah__CCLabelHelper__
#define __Cheetah__CCLabelHelper__

#include <iostream>
#include <string>
#include "cocos2d.h"

typedef enum {
    SLD_IPHONE,
    SLD_IPHONE_RETINA,
    SLD_IPAD,
    SLD_IPAD_RETINA
} StyleLabelDevice;

typedef enum {
	StyleDialogTitle,
    StyleDialogTextLight,
    StyleDialogTextDark,
	StyleMiniDialogTitle,
    StyleButtonCoinAndGems,
    StyleButtonCoinAndGemsNotEnough,
    StyleTextAboveButton
} StyleLabelType;

#define DEFAULT_SCALING -1.0f

#define kFontSizeVerySmall  30.0f
#define kFontSizeSmall      40.0f
#define kFontSizeMedium     50.0f
#define kFontSizeLarge      60.0f
#define kFontSizeVeryLarge  70.0f
#define kFontSizeExtraLarge 80.0f
#define kTextColorWhite     ccc3(255, 255, 255)
#define kTextColorLight     ccc3(76, 117, 93)
#define kTextColorDark      ccc3(56, 56, 56)
#define kTextColorBlue      ccc3(55, 134, 196)
#define kTextColorGreen     ccc3(0, 166, 81)
#define kTextColorYellow    ccc3(255,255, 0)
#define kTextColorRed       ccc3(252, 78, 9)
#define kTextColorOrange    ccc3(245, 90, 32)
#define kTextColorOrange2   ccc3(240, 110, 80)
#define kTextColorGray      ccc3(81, 81, 81)
#define kTextColorWhiteGray ccc3(164, 164, 164)
#define kTextColorCoin      ccc3(255, 191, 72)
#define kTextColorFood      ccc3(227, 195, 145)
#define kTextColorGem       ccc3(110, 108, 189)
#define kTextColorGray      ccc3(81, 81, 81)
#define kTextColorWhiteGray ccc3(164, 164, 164)
#define kTextColorBrown     ccc3(55, 48, 42)
#define kTextColorLightBrown    ccc3(84,75,66)
#define kTextColorDarkBrown     ccc3(67, 60, 54)

class CCLabelHelper: public cocos2d::CCSprite {
    
public:
    
    static cocos2d::CCLabelBMFont* createBMFont(const char *str, StyleLabelType labelStyle);
    static cocos2d::CCLabelBMFont* createBMFont(const char *str, float fontSize, const cocos2d::ccColor3B &color);
    static cocos2d::CCLabelBMFont* createBMFont(const char *str, float fontSize, bool hasShadow);
    static cocos2d::CCLabelBMFont* createBMFont(const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color);
    static cocos2d::CCLabelBMFont* createBMFont(const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, float width, cocos2d::CCTextAlignment alignment);
    static cocos2d::CCLabelBMFont* createGatchaFont(const char *str, float fontSize);

    static cocos2d::CCLabelBMFont* createBMFont(float fontScale, const char *str, float fontSize, const cocos2d::ccColor3B &color);
    static cocos2d::CCLabelBMFont* createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow);
    static cocos2d::CCLabelBMFont* createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color);
    static cocos2d::CCLabelBMFont* createBMFont(float fontScale, const char *str, float fontSize, bool hasShadow, const cocos2d::ccColor3B &color, float width, cocos2d::CCTextAlignment alignment);

    static std::string getFontNameAndScale(float fontScale, float fontSize, bool hasShadow, float *finalScale);
};

#endif /* defined(__Cheetah__CCLabelHelper__) */


