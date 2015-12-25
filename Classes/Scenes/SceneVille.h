//
//  SceneVille.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__SceneVille__
#define __Cheetah__SceneVille__

#include "cocos2d.h"

#include "CScene.h"
#include "../Entities/IsoMapBuilding.h"
#include "../Entities/MapToScreenUI.h"

#include "../UIs/Panels/HUDPanel.h"
#include "../UIs/ShopPanel.h"
#include "../UIs/GachaPanel.h"

#include "../Extensions/Modal/CCModalController.h"
#include "../Extensions/CCNikScrollView.h"
#include "../UIs/Dialogs/ItemStorageDialog.h"

#include "../Entities/Tutorials/TutIntroAndAskName.h"
#include "../Entities/Tutorials/TutBuildStructure.h"
#include "../Entities/Tutorials/TutFusePet.h"
#include "../Entities/Tutorials/TutFeedPet.h"
#include "../Entities/Tutorials/TutSecondRacePart1.h"
#include "../Entities/Tutorials/TutGacha.h"
#include "../Entities/Tutorials/TutInformPremiumGachaUnlock.h"
#include "../Entities/Tutorials/TutRetrievePet.h"
#include "../Entities/Tutorials/TutSetLeader.h"
#include "../Entities/Tutorials/TutSellItem.h"
#include "../Entities/Tutorials/TutIntroMerchant.h"
#include "../Entities/Tutorials/TutBuildFeedSilo.h"
#include "../Entities/Tutorials/TutBuildBank.h"


#include "../Entities/Merchant/Merchant.h"

class SceneVille: public CScene {
    
    void zoomAtUncle();
    void zoomAtHabitat();
    void zoomToNewHabitat();
    void zoomToAnimalBarn();
    
    std::string             m_visit_uuid;
    std::string             m_view_replay_race_id;
    bool                    m_did_process_gamecenter;

    float                   m_tryCloseDialogDelay;
    
public:
    
    GachaPanel              *gachaPanel;
    ShopPanel               *shopPanel;
    CTutManager             *tutMgr;
    Merchant                *merchant;
    
    /** Just a pointer for ItemStorageDialog appears in SceneVille */
    ItemStorageDialog       *itemStorageDialog;

    virtual bool init();
    virtual void update(float delta);
    CREATE_FUNC(SceneVille);
    ~SceneVille();

    virtual void onExit();

    void saveIsoMapInBackend(float delta);
    void rateAppEvent();
    
    void addMerchant();
    void addMorePetsTutorial();
    void addFoodTutorials();
    void addSecondRaceTutorial();
    void addUpgradeTutorials();
    void addGachaTutorial();
    void addItemTutorials();
    void addAchievementTutorials();

    void addFeedSiloTutorials();
    void addBankTutorials();
    
    void runFBLoginDialogLogic();

    void fadeOutButWaitForOpponent(int sceneIndex, std::string oppo_uuid);
    void fadeOutButWaitForWorldMap(int sceneIndex);
    void fadeOutThisSceneAndChangeTo(int sceneIndex);
    virtual void fadeOutThisSceneButWaitForReplay(int sceneIndex, std::string race_id);
    
    void presentGachaPanel();
    void processGameCenter();
    void preProcessMapGet();
    
    // check gcid callback
    static void onGCIDChecked(void* sender, std::string resp_str);
    
    static void disableTouch(void *sender);
    static void enableTouch(void *sender);
    static void willPopView(void* sender, void* view);
    static void fire_map_get(void *sender);
    
    static void fire_opponent_get(void* sender);
    static void onOpponentRetrieved(void* sender, std::string respn_str);
    
    static void onLevelUpEffectDissmissed(void *sender);
    
    void zoomIn();
    
private:
    
    static void _fireReplayGet(void *sender);
    static void _onReplayRetrieved(void *sender, std::string resp_json);
    
};

#endif /* defined(__Cheetah__SceneVille__) */
