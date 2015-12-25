//
//  AchievementDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__AchievementDialog__
#define __Cheetah__AchievementDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Settings.h"
#include "../../VOs/GDAchievementsVO.h"
#include "CCScale9Sprite.h"
#include "ScrollDialog.h"

//#define kFirstSpaceToTheLeft 0.0f
#define kSpacesBetweenItems 1.125f //1.0f = no spaces
#define kLeftOffset 4.0f
#define kUpperOffset 28.0f
//**** must change this value when add new group achievements *****
#define kMaxAchievement 14
//**** must change this value when add new trees *****
#define kMaxTrees 100
//**** must change this value when editing rare_pet requirements *****
#define RARE_PET_1_STARS 5
#define RARE_PET_2_STARS 6
#define RARE_PET_3_STARS 7

class AchievementDialog : public ScrollDialog
{
    CCModalController           *_modalController;
    
    int                         m_number_of_apprequest;
    bool                        canTouch;
    std::string                 lastAchievement;
    int                         lastTag;
    int                         id_achievement;
    cocos2d::CCPoint            posTouch;
    
    bool                        returnNumInviteReady;
    
    void clearWaiting();
    void showWaiting();
    CCSprite *fxB1;
    CCSprite *fxB2;
    CCSprite *fxA;
    
    int indexFrameInvite;
    
    GDAchievementsVO _inviteFacebook_vo;

    std::vector<CCSprite*> m_stars;

    cocos2d::extension::CCScale9Sprite* entryToScrollTo;

    void setDescriptionTxt(GDAchievementsVO _avo, int tag);
    void createStars(CCNode* parent, int status);
    void setProgress(GDAchievementsVO _avo, int tag);
    void setClaim(GDAchievementsVO _avo, int tag);
    void setCompleted(GDAchievementsVO _avo, int tag);
    void updateAchievement(GDAchievementsVO _avo, int tag,
                           cocos2d::extension::CCScale9Sprite* entry = NULL);
    void clearDetail(int tag);
    void updateContent();
    void onAchievement(cocos2d::CCObject *sender_);
    void touchClaimButton(cocos2d::CCObject *sender_);
    void updateClaim();
    void updateAchievementsGameCenter();
    void setWaiting(GDAchievementsVO _avo, int tag);
    void updateInviteAchievement(void *sender_);
    
public:

    static void onAchievementClaimed(void* sender_, std::string resp_json_str_);
    static AchievementDialog *create(CCModalController *parentModalController_);
    virtual bool init(CCModalController* parentModalController_, CCSize size);
    
    virtual void onExit();
    virtual void scrollViewWillBeginDragging(CCNikScrollView *view);
    static void onHitShadow(void* sender_);
    
    static unsigned int checkCurrentProgress(GDAchievementsVO _avo);
    static unsigned int checkAchievementPass();
    
    static void onAppRequestCountReceived(void* sender_, std::string resp_json_str_);
    static void onCollectGemsEffectFinished(void* sender);
    
};


#endif /* defined(__Cheetah__AchievementDialog__) */
