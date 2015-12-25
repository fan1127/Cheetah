//
//  MiniDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 2/18/2557 BE.
//
//

#ifndef __Cheetah__MiniDialog__
#define __Cheetah__MiniDialog__

#include "cocos2d.h"
#include "cocos-ext.h"

#include "../../Entities/EntityTypes.h"

#define kMiniDialogTransitionDuration 0.1f

#define kFontLabelColour    ccc3( 63, 86, 72)
#define kFontValueColour    ccc3( 76,117, 93)
#define kFontBlueColour     ccc3( 55,134,196)
#define kFontGreenColour    ccc3(  0,166, 81)

#define HOLDS_MAX "Holds max"
#define COIN_CAPACITY_INCREASE "Coin Storage Increase"
#define FOOD_CAPACITY_INCREASE "Food Storage Increase"

#include "../../Extensions/CCPhatTouchableSprite.h"

class MiniDialog : public cocos2d::CCNode {
    
    int                 m_spread;
    
protected:

    void                                *m_entity;
    EntityType                          m_entity_type;
    
    cocos2d::extension::CCScale9Sprite  *m_bg;
    CCPhatTouchableSprite               *m_block_sprite;
    cocos2d::CCSprite                   *m_close;
    cocos2d::CCSprite                   *m_ray;
    cocos2d::CCSprite                   *m_rayLeft;

    static bool m_centerDialog;

    void addContent(const char *icon_framename_, cocos2d::CCPoint icon_pos_, const char *label_, const char *value, const char *plus_number_);
    void addContent(const char *icon_framename_, cocos2d::CCPoint icon_pos_, float icon_scale_, const char *label_, const char *value, const char *plus_number_);

    void centerDialog();
    void keepDialogInView();

public:
    
    bool initWithEntity(void *entity_);
    
    virtual void update(float delta_);
    bool compose();
    void open();
    void close();

};

#endif /* defined(__Cheetah__MiniDialog__) */
