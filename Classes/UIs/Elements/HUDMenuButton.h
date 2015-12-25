//
//  HUDMenuButton.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/30/2557 BE.
//
//

#ifndef __Cheetah__HUDMenuButton__
#define __Cheetah__HUDMenuButton__

#include "cocos2d.h"
#include "../../Extensions/CCMenuItemScaleSprite.h"
#include "../../Helpers/UIHelper.h"

#define kHUDMenuButtonMoveDuration  0.2f

class HUDMenuButton : public CCMenuItemScaleSprite {
  
    cocos2d::CCPoint        m_hidePos, m_showPos;
    bool                    m_show;
    
    cocos2d::CCSprite       *m_buttonSprite;
    cocos2d::CCLabelBMFont  *m_label;
    cocos2d::CCSprite       *m_badgeSprite;
    cocos2d::CCLabelBMFont  *m_badgeLabel;
    
public:
    
    static HUDMenuButton *create(const char *button_bg_filename_,
                                 const char *button_icon_filename_,
                                 const char *button_label_,
                                 int badge_number_);
    
    bool init(const char *button_bg_filename_,
              const char *button_icon_filename_,
              const char *button_label_,
              int badge_number_);

    void setAnimation(UIHelper::UIAnimations anim, float delay);

    void setPrePositionShowOnly(cocos2d::CCPoint showPos_);
    void setPrePosition(cocos2d::CCPoint showPos_, cocos2d::CCPoint hidePos_);
    void show(bool show_);
    void show(bool show_, bool animated_);
    void show(bool show_, bool animated_, float delay_);
    
    void setLabel(const char *label_str_);
    void setBadge(int badge_number_);
    void setBadgePosition(cocos2d::CCPoint position_);
    void setBadgePositionX(float pos_x_);
    void setBadgePositionY(float pos_y_);
    
    bool isShowed() { return m_show; }
    
};

#endif /* defined(__Cheetah__HUDMenuButton__) */
