//
//  MatchPanel.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 10/9/56 BE.
//
//

#ifndef __Cheetah__MatchPanel__
#define __Cheetah__MatchPanel__

#include "cocos2d.h"

//#include "../../Extensions/CCPhatButton.h"
#include "../../Extensions/CCNikButton.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCPopOverDialog.h"
#include "../../Entities/Building.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../UIs/Elements/ResourceProgressButton.h"
#include "../../UIs/Dialogs/InstantRaceDialog.h"
#include <sstream>
#include <string>
#include <map>

#define TOP_LINE_MARGIN (62.0f)

enum SPININDEX {
    SPININDEX_GREEN,
    SPININDEX_RED,
    SPININDEX_YELLO,
};

enum SPINWSTATE {
    SPINWSTATE_NOTHING,
    SPINWSTATE_SPINNING,
    SPINWSTATE_FINISHED,
};

//#define SKIP_CHOOSE_HELPER_WHEN_0

#define RACE_STATUS_READY 1
#define RACE_STATUS_WAKEUP 2
#define RACE_STATUS_FEED 3

#define RACE_BTN_START_ANIM_VALUE ccp(0.0f,-40.0f*sizeMultiplier)//0.8f
#define RACE_BTN_ANIM_TIME 0.2f

#define RACE_LABEL_HAS_RACE_FEE_POSITION ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*0.55f)
#define RACE_LABEL_NO_RACE_FEE_POSITION ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*0.78f)
#define RACE_LABEL_FEED_POSITION ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*0.85f)
#define RACE_FLAG_HAS_RACE_FEE_POSITION ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*0.5f)
#define RACE_FLAG_NO_RACE_FEE_POSITION ccp(t_btnRace->getContentSize().width*0.5f, t_btnRace->getContentSize().height*0.37f)

class MatchPanel: public cocos2d::CCLayer {
    
    bool                    m_isInitFinish = false;
    int                     m_countDown = 59;
    int                     m_raceFee = 0;
    
    cocos2d::CCLayer*       m_centerPanelLayer = NULL;
    cocos2d::CCMenu*        m_menuExit = NULL;
    
    bool                    m_isShowSelectPetPanel = false;
    bool                    m_raceSceneLaunching = false;
    
    cocos2d::CCLayer*       m_leftPanelLayer = NULL;
    
    cocos2d::extension::CCSkeletonAnimation         *m_petSpine = NULL;
    cocos2d::CCSprite*      m_horsePowerSprite = NULL;
    
    
    // OponentInfo
    cocos2d::CCSprite       *m_opponentPetLabel         = NULL;
    std::vector<std::string>  listItemsDrop;
    
    CCPopOverDialog     *dialogOpponent;
    bool                showPopup;
    
    void _compose_opponent_view();
    void _compose_player_view();
    void _dispose_player_view();
    
    bool canTouch;
    void checkItemCapacity();

    bool checkDupItems(std::string newItem);
    
    CCLabelBMFont *raceLabel;
    CCLabelBMFont *raceFeeLabel;
    CCSprite* t_btnRace;
    CCSprite* t_btnRaceCoin;
    int raceStatus;
    CCPoint m_menuRaceDefaultPosition;
    
    bool    m_chooseHelperEnabled;
    
public:
    
    bool showOpponentDialog;
    //public by tutorial
    CCNode*                 m_selectPetGroup = NULL;
    //-------------------------
    
    std::vector<cocos2d::CCMenu*> m_menuPets;
    cocos2d::CCMenu*        m_menuRace = NULL;
    
    bool                    instant;
    void                    *isoMap;
    
    static MatchPanel *create(HUDPanel *hudPanel_);
    
    bool init(HUDPanel *hudPanel_);
    virtual void update(float delta_);

    void onRaceButton(cocos2d::CCNode* sender);
    void deductGemsBeforeLaunch(int deducted_gems_);
    void updateHorsePowers();
    void gemIconBeating(cocos2d::CCObject* obj_); //part of CCAction
    void launchChooseHelper();
    void launchRaceScene();
    static void onRaceNewCompleted(void* sender, std::string resp_str);
    void backToMapScene(cocos2d::CCNode* sender);
    void updatePetPlayer(Pet *pet, CCPoint position);
    
    void lookAtTheirPet(void* scene_);
    
    void goldPress(cocos2d::CCNode *sender);
    void foodPress(cocos2d::CCNode *sender);
    void gemsPress();
    void openIAPStore(RESOURCE_TYPE resourceType);
    
    
    void (*onWillLaunchChooseHelper)(void*);
    void* onWillLaunchChooseHelperSender;
    
    void setChooseHelperEnabled(bool enabled_) { m_chooseHelperEnabled = enabled_; }
    
    void buttonPressed(cocos2d::CCObject* sender_);
    
    cocos2d::CCSprite* emotionHungry;
    void showHungry(const char* typeNeed,float scaleItem);
    void hideHungry();
    void delHungry(cocos2d::CCObject* sender_);
    
    static void onPetWokeUp(void* sender_, std::string resp_json_str_);
    static void onPetFed(void* sender_, std::string respn_json_str_);
    
    void onItem(cocos2d::CCObject* object_);
    
};



#endif /* defined(__Cheetah__MatchPanel__) */
