//
//  ChooseHelperDialog.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 6/30/2557 BE.
//
//

#ifndef __Cheetah__ChooseHelperDialog__
#define __Cheetah__ChooseHelperDialog__

#define kChooseHelperDialogFirstSpaceToTheLeft 70.0f
#define kChooseHelperScrollItemShift 40.0f

#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/CCLoadSpriteURL.h"
#include "../../Settings.h"
#include "../../VOs/PlayerHelperPetVO.h"
#include "ScrollDialog.h"

class ChooseHelperItem : public CCNikScrollViewButton {
    
    bool                                        m_isSelected;
    
    cocos2d::CCSprite*                          m_petSprite;
    cocos2d::extension::CCSkeletonAnimation*    m_petSpine;
    cocos2d::CCSprite*                          m_horsepowerSprite;
    cocos2d::CCSprite*                          m_stunTimeSprite;
    CCLoadSpriteURL*                            m_photoSprite;
    cocos2d::CCSprite*                          m_friendSprite;
    cocos2d::CCLabelBMFont*                     m_friendText;
    cocos2d::CCSprite*                          m_checkmark;
    
public:
    
    PlayerHelperPetVO                           m_helper_pet_vo;
    
    static ChooseHelperItem* createWithPlayerHelperPetVO(PlayerHelperPetVO vo_);
    static ChooseHelperItem* createWithPlayerHelperPetVO(PlayerHelperPetVO vo_, bool isSelected_);
    virtual bool init();
    
    void updateContent(bool isSelected_);
    
    PlayerHelperPetVO getHelperPetVO() { return m_helper_pet_vo; }
};

class ChooseHelperDialog : public ScrollDialog
{
    cocos2d::CCLabelBMFont*         m_label_foot;
    
    std::vector<ChooseHelperItem*>  m_items;
//    int                             m_items_selected_index;
    
    cocos2d::CCLabelBMFont*         m_facebookLabel;
    
    cocos2d::CCLabelBMFont*         m_middleText1;
    cocos2d::CCLabelBMFont*         m_middleText2;
    cocos2d::CCLabelBMFont*         m_middleText3;
    
public:
    
    static ChooseHelperDialog *create(CCModalController *parentModalController_);
    virtual bool init(CCModalController *parentModalController_, cocos2d::CCSize dialogSize);
    virtual ~ChooseHelperDialog();

    virtual void onExit();

    void startLoading();
    void finishLoading();
    void updateContent();


    void onSelect(cocos2d::CCObject* sender_);
    void onFacebook(cocos2d::CCObject* sender_);
    static void onFacebookLoggedIn(void* sender_, bool success_);
    static void onFBInstalledFriendRetrieved(void* sender_, bool success_);
    void onGo(cocos2d::CCObject* sender_);
    
    static void onHelperPetRetrieved(void* sender_, std::string resp_json_str_);
    static void onInvitationSent(void* sender_, int numberOfInvites, std::string fbids_);
    static void onApprequestAdd(void* sender_, std::string resp_json_str_);

    static void doNothing(void* inst_, void* sender_) { /*do nothing */ };
};

#endif /* defined(__Cheetah__ChooseHelperDialog__) */
