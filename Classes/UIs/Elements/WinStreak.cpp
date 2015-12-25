//
//  WinStreak.cpp
//  Cheetah
//
//  Created by Teerapat on 5/30/57 BE.
//
//

#include "WinStreak.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCPopOverDialog.h"

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"
extern TargetDevice targetDevice;
extern float retinaMultiplier;
extern float sizeMultiplier;
extern float margin;

WinStreak* WinStreak::create(bool win, int win_streak) {
    WinStreak *obj = new WinStreak();
    if (!obj->init(win, win_streak)) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;
    
}

bool WinStreak::init(bool win, int win_streak) {
    
    if(win) {
        if (!CCNikButton::init(CCSprite::createWithSpriteFrameName("oldversion/_winning_streak_bonus_bg.png"), NULL)) {
            return false;
        }
        
        float win_streak_bonus_base = g_gameContent->GameConfig["WINNING_STREAK_BONUS_PERCENTAGE_BASE"];
        float win_streak_bonus_max = g_gameContent->GameConfig["WINNING_STREAK_BONUS_PERCENTAGE_MAX_DEFAULT"];
        
        CCLog_("win_streak_bonus_max = %f",win_streak_bonus_max);
        
        Building *garage = g_playerState->getBuilding(kBuildingGarage, 0);
        if (garage) win_streak_bonus_max = garage->bVo.param_4;
        
//        int user_state_win_streak = g_playerState->player.info.win_streak;
        int user_state_win_streak = win_streak;
        CCLog_("user_state_win_streak = %d",user_state_win_streak);
        
        bonus = 1 + (win_streak_bonus_base * (user_state_win_streak - 1));
        if (bonus <= 1) bonus = 1;
        if (bonus >= win_streak_bonus_max) bonus = win_streak_bonus_max;
        bonus = roundf(bonus*100.0f)*0.01f;
        
        if(bonus>=1.0f) {
            this->setTarget(this, menu_selector(WinStreak::onTouch), CCNikButtonEventTouchUpInside);
            
            CREATE_CHAR_BUFFER(_buf2, 64);
            snprintf(_buf2, _buf2_size, g_gameContent->Word["win_streak_format"].c_str(), bonus);
            
            CCLabelBMFont *winstreakTxt = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["win_streak_label"].c_str(), kFontSizeMedium, true);
            winstreakTxt->setPosition(ccp(this->getContentSize().width*0.5,+this->getContentSize().height*0.82));
            this->addChild(winstreakTxt);
            
            CCLabelBMFont *percentStreakTxt = CCLabelHelper::createBMFont(retinaMultiplier, _buf2, kFontSizeExtraLarge, true);
            percentStreakTxt->setPosition(this->getContentSize()*0.5);
            this->addChild(percentStreakTxt);
            
            CCLabelBMFont *coinsBonusTxt = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["win_coins_bonus_label"].c_str(), kFontSizeMedium, true);
            coinsBonusTxt->setPosition(ccp(this->getContentSize().width*0.5,+this->getContentSize().height*0.22));
            this->addChild(coinsBonusTxt);
            
//            CCLog_("Player Win Streak = %d",user_state_win_streak);
        }
        else this->setVisible(false);
    }
    else {
        if (!CCNikButton::init(CCSprite::createWithSpriteFrameName("oldversion/_winning_streak_bonus_bg_grey.png"), NULL)) {
            return false;
        }

        CCLabelBMFont *winstreakTxt = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["lost_streak_label"].c_str(), kFontSizeMedium, true);
        winstreakTxt->setPosition(ccp(this->getContentSize().width*0.5,+this->getContentSize().height*0.82));
        this->addChild(winstreakTxt);
        
        bonus = 0;
        CREATE_CHAR_BUFFER(_buf2, 64);
        snprintf(_buf2, _buf2_size, g_gameContent->Word["lost_streak_format"].c_str(), bonus);
        
        CCLabelBMFont *percentStreakTxt = CCLabelHelper::createBMFont(retinaMultiplier, _buf2, kFontSizeExtraLarge, true , kTextColorRed);
        percentStreakTxt->setPosition(this->getContentSize()*0.5);
        this->addChild(percentStreakTxt);
        
        CCLabelBMFont *coinsBonusTxt = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["lost_coins_bonus_label"].c_str(), kFontSizeMedium, true);
        coinsBonusTxt->setPosition(ccp(this->getContentSize().width*0.5,+this->getContentSize().height*0.22));
        this->addChild(coinsBonusTxt);
        
    }
    
    return true;
}

void WinStreak::onTouch(cocos2d::CCNode *sender)
{
    CREATE_CHAR_BUFFER(_buf1, 128);
    CREATE_CHAR_BUFFER(_buf2, 128);
    
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    CCLabelBMFont *titleLabel = CCLabelHelper::createBMFont(retinaMultiplier, g_gameContent->Word["player_win_streak_title"].c_str(), kFontSizeLarge, kTextColorDark);
    
    CCLabelBMFont *detailLabel = CCLabelHelper::createBMFont(retinaMultiplier,  g_gameContent->Word["player_win_streak_detail"].c_str(), kFontSizeMedium, kTextColorDark);
    
    snprintf(_buf1, _buf1_size, g_gameContent->Word["player_win_streak_number"].c_str(), g_playerState->player.info.win_streak );
    
    CCLabelBMFont *detailLabelGray = CCLabelHelper::createBMFont(retinaMultiplier, _buf1, kFontSizeMedium, kTextColorGray);
    CREATE_CHAR_BUFFER(_buf3, 64);
    
    snprintf(_buf3, _buf3_size, "%.2fx", bonus);
    snprintf(_buf2, _buf2_size, g_gameContent->Word["player_win_streak_bonus"].c_str(), _buf3 );
    CCLabelBMFont *detailLabel2 = CCLabelHelper::createBMFont(retinaMultiplier, _buf2, kFontSizeMedium, kTextColorGray);
    CCSize popoverSize;
    switch (targetDevice) {
        case TD_IPHONE: {

            
        }break;
        case TD_IPHONE_RETINA:{
            popoverSize = CCSize(titleLabel->getContentSize().width*3 , titleLabel->getContentSize().height*7)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            titleLabel->setPosition(ccp(14+ titleLabel->getContentSize().width*0.5f, popover->getContentSize().height - titleLabel->getContentSize().height*0.7f ));
            popover->addChild(titleLabel);
            detailLabel->setPosition(ccp( titleLabel->getContentSize().height*2.9 ,
                                         titleLabel->getPositionY() - titleLabel->getContentSize().height ));
            detailLabel->setWidth(titleLabel->getContentSize().width*1.5f);
            popover->addChild(detailLabel);
            detailLabelGray->setPosition(ccp( titleLabel->getContentSize().height*3,detailLabel->getPositionY()-titleLabel->getContentSize().height ));
            popover->addChild(detailLabelGray);
            detailLabel2->setPosition(ccp( titleLabel->getContentSize().height*2.8,detailLabel->getPositionY()-titleLabel->getContentSize().height*1.6f ));
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
            
        }break;
        case TD_IPAD:       {
            popoverSize = CCSize(titleLabel->getContentSize().width*4 , titleLabel->getContentSize().height*7.5f)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            titleLabel->setPosition(ccp( titleLabel->getContentSize().height*1.9f, popover->getContentSize().height - titleLabel->getContentSize().height*0.7f ));
            popover->addChild(titleLabel);
            detailLabel->setPosition(ccp( titleLabel->getContentSize().height*2.9 ,
                                         titleLabel->getPositionY() - titleLabel->getContentSize().height ));
            detailLabel->setWidth(titleLabel->getContentSize().width*1.5f);
            popover->addChild(detailLabel);
            detailLabelGray->setPosition(ccp( titleLabel->getContentSize().height*3,detailLabel->getPositionY()-titleLabel->getContentSize().height ));
            popover->addChild(detailLabelGray);
            detailLabel2->setPosition(ccp( titleLabel->getContentSize().height*2.8,detailLabel->getPositionY()-titleLabel->getContentSize().height*1.5f ));
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
        }  break;
        case TD_IPAD_RETINA:    {
            popoverSize = CCSize(titleLabel->getContentSize().width*3 , titleLabel->getContentSize().height*7)*sizeMultiplier;
            CCPopOverDialog *popover = CCPopOverDialog::create(popoverSize, CCPopoverArrowDirectionDown);
            titleLabel->setPosition(ccp( titleLabel->getContentSize().width*0.92f, popover->getContentSize().height - titleLabel->getContentSize().height*0.9f ));
            popover->addChild(titleLabel);
            detailLabel->setPosition(ccp( titleLabel->getContentSize().height*4.5 ,
                                         titleLabel->getPositionY() - titleLabel->getContentSize().height*1.5 ));
            detailLabel->setWidth(titleLabel->getContentSize().width*2.3);
            popover->addChild(detailLabel);
            detailLabelGray->setPosition(ccp( titleLabel->getContentSize().height*5,detailLabel->getPositionY()-titleLabel->getContentSize().height*1.8 ));
            popover->addChild(detailLabelGray);
            detailLabel2->setPosition(ccp( titleLabel->getContentSize().height*4.6,detailLabel->getPositionY()-titleLabel->getContentSize().height*2.8 ));
            popover->addChild(detailLabel2);
            popover->presentPopover(sender, true);
            
        }  break;
    }
}
