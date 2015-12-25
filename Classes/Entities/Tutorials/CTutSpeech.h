//
//  CTutSpeech.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__CTutSpeech__
#define __Cheetah__CTutSpeech__

#include "CTut.h"
#include "cocos2d.h"
#include "../../VOs/GDBuildingVO.h"
#include <spine/spine-cocos2dx.h>

#define SPEECH_ANIMATION_STARTING_ROTATION_ANGLE (15)
#define SPEECH_ANIMATION_STARTING_SCALE (0.4)//(0.75)
#define SPEECH_ANIMATION_TIME_ROTATION (0.35f)//(0.5f)
#define SPEECH_ANIMATION_TIME_SCALING (0.5f)//(0.7f)
#define SPEECH_ANIMATION_DELAY_UNTIL_TAPPABLE (0.3f)
#define SPEECH_ANIMATION_fPeriod (0.8f)//(0.3f)

//children of messageDialogSprite
#define TAG_ARROW 123
#define TAG_TEXT 124

//children of chooseDialogSprite
#define TAG_MENU 125
#define TAG_TITLE 126

enum CTutCharacter {
    CT_CHR_NONE,
    CT_CHR_UNCLE,
    CT_CHR_SARAH,
    CT_CHR_MERCHANT,
    CT_CHR_FANS,
};

enum CTutCharacterSide {
    CT_CHR_SIDE_LEFT,
    CT_CHR_SIDE_RIGHT,
};

enum CTutSpeechOption {
    CT_SPC_OPTION_NONE,
    CT_SPC_OPTION_CHOOSE_ITEM,
    CT_SPC_OPTION_FILL_NAME,
    CT_SPC_OPTION_NONE_WITH_PICTURE,
};

#define kTutNChoiceKeys     3

#define kUncleHappy      "samuel_happy"
#define kUncleExplain    "samuel_explain"
#define kUncleNote       "samuel_note"
#define kSarahAngry      "sarah_angry"
#define kSarahHappy      "sarah_happy"
#define kSarahNormal     "sarah_normal"
#define kSarahScared     "sarah_scared"
#define kFanIdle         "idle"
#define kMerchantIdle    "portrait_idle"
#define kMerchantHappyJump    "portrait_idle_stand_jump1"

#define kMarginBetweenMerchantItems (20.0f*sizeMultiplier)

class CTutSpeech: public CTut, cocos2d::CCObject {
    
private:
    
    void _onPetChosen(cocos2d::CCObject* object_);
    void _onItemChosen(cocos2d::CCObject* object_);
    static void _onHitTheBackground(void* inst_, void* sender_);
    static void _onNameFilled(void* sender_, std::string result_);
    
    void onAnimationFinished(CCObject *sender);
    
public:
    
    /** Character to apear in the speech */
    CTutCharacter    chr;
    
    /** Left of right side for charactor to be appeared. */
    CTutCharacterSide side;
    
    /** Animation for character */
    std::string     animation;
    
    /** Animation loop */
    bool            loop;
    
    /** Animation for character */
    std::string     animation2;
    
    /** Animation loop */
    bool            loop2;
    
    /** Character's scale */
    float           scale;
    
    /** Message to show in the speech */
    std::string     message;
    
    /** Define an option with the speech */
    CTutSpeechOption option;
    
    /** Array of choices (key) */
    std::string     keys[kTutNChoiceKeys];
    
    GDBuildingVO vo;
    
    float getChooseItemsScale();
    
    CTutSpeech(CTutCharacter chr_, CTutCharacterSide side_, std::string animation_, bool loop_, std::string message_, CTutSpeechOption option_, void* sender_, void (*callback_)(void* sender, std::string result_)) {
        
        CTut();
        id = CT_ID_SPEECH;
        
        chr = chr_;
        side = side_;
        animation = animation_;
        loop = loop_;
        animation2 = "";
        loop2 = false;
        scale = 1.5f;
        message = message_;
        option = option_;
        for(int i=0; i<kTutNChoiceKeys; i++) {
            keys[i] = "";
        }
        
        sender = sender_;
        callback = callback_;
        
        unsetAfterPerformCallback();
    }
    
    CTutSpeech(CTutCharacter chr_, CTutCharacterSide side_, std::string animation_, bool loop_, std::string message_, CTutSpeechOption option_,GDBuildingVO vo_, void* sender_, void (*callback_)(void* sender, std::string result_)) {
        
        CTut();
        id = CT_ID_SPEECH;
        
        chr = chr_;
        side = side_;
        animation = animation_;
        loop = loop_;
        animation2 = "";
        loop2 = false;
        scale = 1.5f;
        message = message_;
        option = option_;
        for(int i=0; i<kTutNChoiceKeys; i++) {
            keys[i] = "";
        }
        
        sender = sender_;
        callback = callback_;
        vo = vo_;
        
        unsetAfterPerformCallback();
    }
    
    CTutSpeech(CTutCharacter chr_, CTutCharacterSide side_, std::string animation_, bool loop_, std::string animation2_, bool loop2_, std::string message_, CTutSpeechOption option_, void* sender_, void (*callback_)(void* sender, std::string result_)) {
        
        CTut();
        id = CT_ID_SPEECH;
        
        chr = chr_;
        side = side_;
        animation = animation_;
        loop = loop_;
        animation2 = animation2_;
        loop2 = loop2_;
        scale = 1.5f;
        message = message_;
        option = option_;
        for(int i=0; i<kTutNChoiceKeys; i++) {
            keys[i] = "";
        }
        
        sender = sender_;
        callback = callback_;
        
        unsetAfterPerformCallback();
    }
    
    virtual void perform(void* tutUnit_);
    virtual void update(float delta_);
    
};

#endif /* defined(__Cheetah__CTutSpeech__) */
