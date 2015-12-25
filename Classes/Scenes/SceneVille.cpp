//
//  SceneVille.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#include <spine/Skeleton.h>
#include "SceneVille.h"
USING_NS_CC;

using namespace std;

#include "../SceneManager.h"
extern SceneManager *g_sceneManager;
extern CCScene *g_currentScene;
extern string gcid;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../Entities/PlayerState.h"
extern PlayerState *g_playerState;
extern PlayerState *g_opponentState;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIUserGcidCheck.h"
#include "../Services/APIs/APIUserGcidTie.h"
#include "../Services/APIs/APIOpponentGet.h"
#include "../Services/APIs/APIMapGet.h"
#include "../Services/APIs/APIRaceGet.h"

#include "../Extensions/ParticleSystem/ParticleHeaders.h"

#include "../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

extern float retinaMultiplier;
extern float sizeMultiplier;
extern unsigned int currentTime;

#include "../UIs/Dialogs/LoadGameDialog.h"
#include "../UIs/Dialogs/FBLoginDialog.h"

#include "../Extensions/CCAlert/CCAlert.h"

#include "../Entities/Tutorials/CTutAchievement.h"
#include "../Helpers/AppRating/AppRating.h"
#include "../Helpers/CustomEventsHelper.h"

#include "../Extensions/WeatherEffects/WeatherEnvironment.h"
#include "../Helpers/DataCache.h"
#include "../UIs/BlockingLayer.h"
#include "../Helpers/PointerPool.h"

#include "../Helpers/AdsManagerWrapper.h"

bool SceneVille::init() {
    
    if (!CCScene::init()) {
        return false;
    }
    
    m_visit_uuid = "";
    m_did_process_gamecenter = false;
    m_tryCloseDialogDelay = 0.f;

    isoMap = IsoMapBuilding::create();
    isoMap->setAnchorPoint(ccp(0.5f, 0.5f));
    
    mapToScreenUI = MapToScreenUI::create();
    addChild(mapToScreenUI, 101);
    
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)isoMap;
    _isoMapBuilding->mapToScreenUI = mapToScreenUI;
    
    scrollView = CCNikScrollView::create();
    scrollView->setZoomingEnabled(true);
    scrollView->setDelegate(_isoMapBuilding);
    isoMap->setPosition(isoMap->getContentSize()*-1/4.f);
    scrollView->addChild(isoMap);
    scrollView->setContentView(isoMap->getContentSize()/2.f);
    scrollView->setMaximumScale(1.5f);
    scrollView->setMinimumScale(scrollView->getContentSize().width/(isoMap->getContentSize().width/2.f));
    scrollView->zoomOnPosition(isoMap->getContentSize()*0.5f, scrollView->getMinimumScale(), 0.f);
    addChild(scrollView, 1);
    scheduleOnce(schedule_selector(SceneVille::zoomIn), 0.0f);
    
    isoMap->scrollView = scrollView;
    
#ifdef ENABLE_SNOW
    addChild(WeatherEnvironment::create(scrollView, isoMap, PARTICLE_TYPE_SNOW, SNOW_DEFAULT_AMOUNT), 2);
#endif

    
#define TEST_ALL_ASSETS
    
#ifdef IS_DEBUG_BUILD
#ifdef TEST_ALL_ASSETS
    
    CCNode          *test_assets;
    test_assets = CCNode::create();
    double _petScale = sizeMultiplier * 0.712251282f;
    int count = 0;

    
      for(vector<GDPetVO>::iterator it=GameContentService::getInstance()->Pet.begin(); it!=GameContentService::getInstance()->Pet.end(); ++it)
      {
        GDPetVO test_pet_vo = *it;
          
        count++;
        CCLog_("%d) LOADING PETS     %s => %s%s", count, test_pet_vo.title.c_str(), kPetSrcPath, test_pet_vo.src.c_str());
          
        CCSkeletonAnimation *_petSpine = g_sceneManager->sharedSkeletonData->getSkeleton(kPetSrcPath, test_pet_vo.src.c_str());
        _petSpine->setScale(_petScale);
        _petSpine->setAnimation("habitat_idle_stand", true);
        _petSpine->timeScale = 1.0f;
        test_assets->addChild(_petSpine);
        
        CCString *fileName = CCString::createWithFormat("%s.png", test_pet_vo.src.c_str());
          
        CCSprite *_animal_pic = CCSprite::createWithSpriteFrameName(fileName->getCString());
        test_assets->addChild(_animal_pic);
           
      }
    
      count = 0;
    
      for(vector<GDBuildingVO>::iterator it=GameContentService::getInstance()->Building.begin(); it!=GameContentService::getInstance()->Building.end(); ++it)
      {
        GDBuildingVO test_building_vo = *it;
          
        count++;
        CCLog_("%d) LOADING BUILDINGS     %s => %s%s", count, test_building_vo.title.c_str(), kBuildingSrcPath, test_building_vo.src.c_str());
          
        CCSkeletonAnimation *_spineConstruction = g_sceneManager->sharedSkeletonData->getSkeleton(kBuildingSrcPath, test_building_vo.src.c_str());
        _spineConstruction->setContentSize(CCSize(test_building_vo.src_width, test_building_vo.src_height));
        _spineConstruction->timeScale = 1.0f;
        test_assets->addChild(_spineConstruction);
      }
    
    
      count = 0;
    
      for(vector<GDDecorationVO>::iterator it=GameContentService::getInstance()->Decoration.begin(); it!=GameContentService::getInstance()->Decoration.end(); ++it)
      {
        GDDecorationVO test_decoration_vo = *it;
        
        count++;
        CCLog_("%d) LOADING DECORATIONS     %s => %s%s", count, test_decoration_vo.title.c_str(), kDecorationSrcPath, test_decoration_vo.src.c_str());
        
        CCSkeletonAnimation *_decorationCount = g_sceneManager->sharedSkeletonData->getSkeleton(kDecorationSrcPath, test_decoration_vo.src.c_str());
        _decorationCount->timeScale = 1.0f;
        test_assets->addChild(_decorationCount);
      }
    
      count = 0;
    
      for(vector<GDItemVO>::iterator it=GameContentService::getInstance()->Item.begin(); it!=GameContentService::getInstance()->Item.end(); ++it)
      {
        GDItemVO test_item_vo = *it;
        
        count++;
        CCLog_("%d) LOADING ITEMS     %s => %s%s", count, test_item_vo.title.c_str(), kIAPGatchaOneTimeOfferKey, test_item_vo.src.c_str());
        
          
        CCSprite *_itemSprite = CCSprite::createWithSpriteFrameName(test_item_vo.src.c_str());
        test_assets->addChild(_itemSprite);
      }

    
#endif
#endif
    
    modalController = CCModalController::create();
    modalController->setSender(this);
    modalController->onWillPopView = willPopView;
    modalController->onPushView = disableTouch;
    modalController->onPopView = enableTouch;
    modalController->onCloseAll = enableTouch;
    modalController->retain();
    
    ///////////////////////////////////
    // UI Panels
    hudPanel = HUDPanel::create();
    hudPanel->addExpBar();
    hudPanel->addSettingButton();
    hudPanel->addResourceBar(HUDPanelBarGems, NULL);
    hudPanel->addResourceBar(HUDPanelBarFoods, hudPanel->resourceBars[HUDPanelBarGems]);
    hudPanel->addResourceBar(HUDPanelBarCoins, hudPanel->resourceBars[HUDPanelBarFoods]);
    hudPanel->addTrackEditBar();
    hudPanel->addMainMenuButton(HUDPanelMenuMap, NULL, POS_BOTTOM_ARRANGE_FROM_RIGHT);
    hudPanel->addMainMenuButton(HUDPanelMenuShop, NULL, POS_BOTTOM_ARRANGE_FROM_LEFT);
    hudPanel->addMainMenuButton(HUDPanelMenuInbox, NULL, POS_FIXED_DEFAULT);
    hudPanel->addMainMenuButton(HUDPanelMenuFriends, NULL, POS_FIXED_DEFAULT);
    hudPanel->addMainMenuButton(HUDPanelMenuGacha, NULL, POS_FIXED_DEFAULT);
    hudPanel->addMainMenuButton(HUDPanelMenuAchievement, NULL, POS_FIXED_DEFAULT);
    hudPanel->buildMainMenuButtons();
//    hudPanel->setSignalSignDisplayed(true);
    BackendService::getInstance()->setActiveCallback(hudPanel, HUDPanel::onBackendServiceActive);
    hudPanel->updateHUD();
    hudPanel->updateHUDTrack(isoMap->isTrackComplete());
    hudPanel->updateBadgeAchievement();
    _isoMapBuilding->hudPanel = hudPanel;
    addChild(hudPanel, 9);
    hudPanel->resourceBars[HUDPanelBarGems]->setMaxLevelLabelShowed(false);
    
    shopPanel = ShopPanel::create();
    shopPanel->setSender(this);
    shopPanel->onClose = enableTouch;
    addChild(shopPanel, 10);
    _isoMapBuilding->shopPanel = shopPanel;
    
    _isoMapBuilding->collectableFoods = CollectableItems::create(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::HUDIconFoodNoShadow)));
    _isoMapBuilding->collectableFoods->onCollected = IsoMapBuilding::onFoodIconHitHUD;
    _isoMapBuilding->collectableFoods->setSender(isoMap);
    addChild(_isoMapBuilding->collectableFoods, 10);
    
    _isoMapBuilding->collectableCoins = CollectableItems::create(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::HUDIconCoinNoShadow)));
    _isoMapBuilding->collectableCoins->onCollected = IsoMapBuilding::onCoinIconHitHUD;
    _isoMapBuilding->collectableCoins->setSender(isoMap);
    addChild(_isoMapBuilding->collectableCoins, 10);

    _isoMapBuilding->collectableGems = CollectableItems::create(UIHelper::getSpritePath(UIHelper::UISprites(UIHelper::TraceBuildingsOptionsGemIconForAnimation)));
    _isoMapBuilding->collectableGems->onCollected = IsoMapBuilding::onGemsIconHitHUD;
    _isoMapBuilding->collectableGems->setSender(isoMap);
    addChild(_isoMapBuilding->collectableGems, 10);
    
    tutMgr = CTutManager::create();

#ifndef DEBUG_SKIP_ALL_TUTS
#ifndef DEBUG_SKIP_ASK_NAME
    tutMgr->addTut(TutIntroAndAskName::create(_isoMapBuilding, hudPanel));
#endif
    tutMgr->addTut(TutBuildStructure::create<TutBuildHabitat>(kBuildingHabitat, false, hudPanel, shopPanel, _isoMapBuilding, modalController, scrollView));
    tutMgr->addTut(TutFusePet::create(scrollView, hudPanel, shopPanel, _isoMapBuilding, modalController));
#endif
    addChild(tutMgr, 2000);

    merchant = NULL;
    addMerchant();
    
    addFoodTutorials();
    addSecondRaceTutorial();// will be called in onLevelUpEffectDissmissed;
    hudPanel->setLevelUpEffectDismissCallback(this, SceneVille::onLevelUpEffectDissmissed);

    addUpgradeTutorials();
    addGachaTutorial();
    addItemTutorials();

    addMorePetsTutorial();
    addAchievementTutorials();

    //Must have no tutorials
    if (tutMgr->getTutUnits().size() == 0) {
        this->runFBLoginDialogLogic();
    }
    
    if (g_sceneManager && g_sceneManager->pendingMsg) {
        CREATE_CHAR_BUFFER(_buf, 64);
        CCObject *obj;
        CCARRAY_FOREACH(g_sceneManager->pendingMsg, obj) {
            snprintf(_buf, _buf_size, "%s",((CCString *)obj)->getCString());
            if(obj!=NULL){
                g_sceneManager->notification->addMessage(MSGN_TYPE_INFO,_buf );
            }
        }
        g_sceneManager->pendingMsg->removeAllObjects();
    }
    
    if (hudPanel) {
        
        CCSprite *foodIconSprite = hudPanel->resourceBars[HUDPanelBarFoods]->getIconSprite();
        foodIconSprite->convertToWorldSpaceAR(CCPointZero);
        
        CCPoint pos = foodIconSprite->convertToWorldSpaceAR(CCPointZero);
        float scale = hudPanel->getScale()*foodIconSprite->getScale();
        _isoMapBuilding->collectableFoods->setDest(pos, scale);
        
        CCSprite *goldIconSprite = hudPanel->resourceBars[HUDPanelBarCoins]->getIconSprite();
        goldIconSprite->convertToWorldSpaceAR(CCPointZero);
        
        pos = goldIconSprite->convertToWorldSpaceAR(CCPointZero);
        scale = hudPanel->getScale()*goldIconSprite->getScale();
        _isoMapBuilding->collectableCoins->setDest(pos, scale);
        
        CCSprite* gemIconSprite = hudPanel->resourceBars[HUDPanelBarGems]->getIconSprite();
        gemIconSprite->convertToWorldSpaceAR(CCPointZero);
        pos = gemIconSprite->convertToWorldSpaceAR(CCPointZero);
        scale = hudPanel->getScale()*gemIconSprite->getScale();
        _isoMapBuilding->collectableGems->setDest(pos, scale);
        
        hudPanel->updateHUD();
        
    }
    
    itemStorageDialog = NULL;
    processGameCenter();
    preProcessMapGet();
    
    g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM, true, 0.25f);
    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.25f, 0.25f, 0.25f);

    ((IsoMapBuilding*)isoMap)->addDroppedItemToBarn();

    if (AdsManagerWrapper::meetAdsDisplayingRequirements()
        &&
        PlayerState::getInstance()->racedAtLeastOnce
        )
    {
        PlayerState::getInstance()->racedAtLeastOnce = false;
        
        AdsManagerWrapper::showInterstitial();
    }
    
    scheduleUpdate();
    schedule(schedule_selector(SceneVille::saveIsoMapInBackend), kIsoMapSaveInterval);

    if (AppRating::Instance().canRate())
    {
        scheduleOnce(schedule_selector(SceneVille::rateAppEvent), 3.0f);
    }

    g_gameAudio->unloadEffects(GameAudioService::Race);
    
    
    return true;
}

SceneVille::~SceneVille() {
    
    if (modalController) {
        modalController->release();
        modalController = NULL;
    }
    
    if (merchant) {
        merchant->release();
        merchant = NULL;
    }
    
}

void SceneVille::onExit()
{
    CScene::onExit();
    BackendService::getInstance()->unsetActiveCallback();
}

void SceneVille::update(float delta) {
    
    if (!g_sceneManager) {
        CCLog_("SceneVille::update - sceneManager is unitialized!");
        return;
    }

    if(hudPanel->isLevelUpEffectShowed() && !gachaPanel)
    {
        if(m_tryCloseDialogDelay <= 0.f)
        {
            modalController->closeAll();
            mapToScreenUI->closeAllUIs();
            m_tryCloseDialogDelay = 1.5f;
        }
        else
            m_tryCloseDialogDelay -= delta;
    }

    switch (g_sceneManager->sceneSelector) {
            
        case SCENE_BOOST: {
            if (g_sceneManager->transition->isFinished()) {
                g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
                g_sceneManager->switchScene(g_sceneManager);
            }
        }break;
        case SCENE_VILLE: {
            if (isoMap) {
                isoMap->update(delta);
            }
        }break;
        case SCENE_VISIT: {
//            if (g_opponentState) {
//                if (g_opponentState->didParseData() && g_sceneManager->transition->didFadeOut) {
//                    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
//                    g_sceneManager->switchScene(g_sceneManager);
//                }
//            }
        }break;
        case SCENE_WORLDMAP: {
            if (g_playerState && g_sceneManager) {
                if (g_playerState->WorldMapVillages.size() > 0 && g_sceneManager->transition->isFinished()) {
                    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
                    g_sceneManager->switchScene(g_sceneManager);
                }
            }
        }break;
        default:
            break;
            
    }
}

void SceneVille::saveIsoMapInBackend(float delta) {
    
    if (isoMap) {
        ((IsoMapBuilding*)isoMap)->saveInBackend(delta);
    }
}

void SceneVille::rateAppEvent()
{
    AppRating::Instance().showRateDialog();
}

void SceneVille::addMerchant() {
    
    if (merchant || !tutMgr) return;
    
    merchant = Merchant::createWithIsoMap(isoMap, tutMgr);
    if (!merchant) return;
    
    merchant->retain();
    IsoMapBuilding* _isoMapBuilding = (IsoMapBuilding*)isoMap;
    _isoMapBuilding->merchant = merchant;
    tutMgr->addTut(TutIntoMerchant::create(scrollView, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, merchant));
    
}

void SceneVille::addMorePetsTutorial() {
    
    if (tutMgr) {
        tutMgr->addTut(TutBuildStructure::create<TutBuildHabitat2>(kBuildingHabitat, false, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController, scrollView));
        
        tutMgr->addTut(TutRetrievePet::create(scrollView, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController));
        tutMgr->addTut(TutBuildBank::createIntro());
        tutMgr->addTut(TutBuildFeedSilo::createIntro());
//        tutMgr->addTut(TutSetLeader::create(scrollView, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController));
    }
    
}

void SceneVille::addFoodTutorials()
{
    CCAssert(tutMgr, "tutorial manager is missing");
#ifndef DEBUG_SKIP_ALL_TUTS
    tutMgr->addTut(TutFeedPet::create(scrollView, hudPanel, shopPanel, (IsoMapBuilding*)isoMap));
    tutMgr->addTut(TutBuildStructure::create<TutBuildFeedMill>(kBuildingFoodProduction, false, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController, scrollView));
#endif

}

void SceneVille::addSecondRaceTutorial()
{
#ifndef DEBUG_SKIP_ALL_TUTS
#ifndef DEBUG_SKIP_SECOND_RACE
    tutMgr->addTut(TutSecondRacePart1::create(hudPanel, shopPanel, (IsoMapBuilding*)isoMap));
#endif
#endif
}

void SceneVille::addUpgradeTutorials()
{
    tutMgr->addTut(TutBuildStructure::create<TutUpgradeMansion>(kBuildingHQ, true, hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController, scrollView));
}

void SceneVille::addFeedSiloTutorials(){
    tutMgr->addTut(TutBuildFeedSilo::create());
}

void SceneVille::addBankTutorials(){
    tutMgr->addTut(TutBuildBank::create());
}

void SceneVille::addGachaTutorial() {
#ifndef DEBUG_SKIP_ALL_TUTS
#ifndef DEBUG_SKIP_GACHA
    tutMgr->addTut(TutGacha::create(hudPanel, shopPanel, (IsoMapBuilding*)isoMap));
    tutMgr->addTut(TutInformPremiumGachaUnlock::create(hudPanel, shopPanel, (IsoMapBuilding*)isoMap));
#endif
#endif
}

void SceneVille::addItemTutorials()
{
#ifndef DEBUG_SKIP_ALL_TUTS
    
    tutMgr->addTut(TutSellItem::create(scrollView, hudPanel, shopPanel, (IsoMapBuilding*)isoMap));
#endif
}

void SceneVille::addAchievementTutorials()
{
    tutMgr->addTut(TutAchievement::create(hudPanel, shopPanel, (IsoMapBuilding*)isoMap, modalController));
}

void SceneVille::zoomAtUncle()
{
    //Lock บ้านคุณลุง
//    g_gameAudio->stopAllEffect();
//    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.1, 0.1, 3.0);
    scrollView->zoomOnPosition(ccp(scrollView->getContentView().width*0.55, scrollView->getContentView().height*0.43), 1.0f, 1.0f);
}

void SceneVille::zoomAtHabitat()
{
    //Lock ไปที่ habitat
//    g_gameAudio->stopAllEffect();
//    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.1, 0.1, 3.0);
    
    if (Building *building = g_playerState->getBuilding(kBuildingHabitat, 0)) {
        scrollView->zoomOnPosition(building->getPosition() - isoMap->getContentSize()/4.f, 1.0f, 1.0f);
    }
    
//    scrollView->zoomOnPosition(ccp(7040*retinaMultiplier,5952*retinaMultiplier) - isoMap->getContentSize()/3.f, 1.0f, 1.0f);
}

void SceneVille::zoomToAnimalBarn()
{
    //Lock ไปที่ animal barn
//    g_gameAudio->stopAllEffect();
//    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.1, 0.1, 3.0);
    
    if (Building *building = g_playerState->getBuilding(kBuildingAnimal, 0)) {
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        switch (targetDevice) {
            default:
            case TD_IPHONE:
            case TD_IPHONE_RETINA: {
                if(screenSize.width==960) { //3.5 inch
                    scrollView->zoomOnPosition(building->getPosition() - isoMap->getContentSize()/4.f + ccp(200*retinaMultiplier, -90*retinaMultiplier), 1.0f, 1.0f);
                }
                else {
                    scrollView->zoomOnPosition(building->getPosition() - isoMap->getContentSize()/4.f + ccp(300*retinaMultiplier, -90*retinaMultiplier), 1.0f, 1.0f);
                }
            }break;
            case TD_IPAD:
            case TD_IPAD_RETINA: {
                scrollView->zoomOnPosition(building->getPosition() - isoMap->getContentSize()/4.f + ccp(200*retinaMultiplier, -90*retinaMultiplier), 1.0f, 1.0f);
                
            }break;
        }
    }
    
}

void SceneVille::zoomToNewHabitat()
{
    //Lock to new habitat
//    g_gameAudio->stopAllEffect();
//    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 0.1, 0.1, 3.0);

    //scroll to blank workspace
    scrollView->zoomOnPosition(ccp(scrollView->getContentView().width*0.43, scrollView->getContentView().height*0.41), 1.0f, 1.0f);

}

void SceneVille::processGameCenter() {
    
    CCLog_("------ processGameCenter --------");
    CCLog_("gcid=%s", gcid.c_str());
    CCLog_("player.gcid=%s", g_playerState->player.gcid.c_str());
    
    if (!m_did_process_gamecenter) {
        if (gcid != "") {
            if (g_playerState->player.gcid == "" || g_playerState->player.gcid != gcid) { //
                m_did_process_gamecenter = true;
                BackendService::getInstance()->send(APIUserGcidCheck::create(gcid, this, SceneVille::onGCIDChecked));
            }else if (g_playerState->player.gcid != gcid) {
                
            }
        }
    }
    
}

void SceneVille::preProcessMapGet() {
    
    if (g_playerState->getNPet() > 0) {
        BackendService::getInstance()->send(APIMapGet::create(hudPanel,
                                                              HUDPanel::onMapDataRetrieved));
    }
    
}

void SceneVille::onGCIDChecked(void *sender, string json_str) {
    
    if (typeid(*g_currentScene) == typeid(SceneVille)) {
    
        if (SceneVille* _this = (SceneVille*)sender) {

            //JsonAllocator allocator;
            if (Json *_root = Json_create(json_str.c_str())) {
            
                string _uuid = Json_getString(_root, "uuid", "");
                
                if (_uuid != "") {
                    
                    string _name = Json_getString(_root, "name", "");
                    int _level = Json_getInt(_root, "level", 0);
                    
                    CCLog_("%s Lv.%d", _name.c_str(), _level);
                    
                    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                    LoadGameDialog *_dialog = LoadGameDialog::create(_this->modalController, _name, _level);
                    _dialog->setPosition(_screenSize*0.5f);
                    _dialog->setCloseLocked(true);
                    
                    if (_this->modalController->getParent() == NULL) {
                        _this->getParent()->addChild(_this->modalController, 10001);
                    }
                    
                    _this->modalController->pushView(_dialog);

                }else {
                    
                    g_playerState->player.gcid = gcid;
                    BackendService::getInstance()->send(APIUserGcidTie::create(gcid, NULL, LoadGameDialog::onUserGcidTied));
                    
                }
                
                Json_dispose(_root);
                
            }
            
        }
        
    }
    
}

void SceneVille::zoomIn() {
    
    Building *building = g_playerState->getBuilding(kBuildingAchievement, 0);
    if (building) {
        scrollView->zoomOnPosition(building->getPosition() - isoMap->getContentSize()/4.f, 1.3f, 1.0f);
    } else {
        scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, 1.0f, 1.0f);
    }
    
}

void SceneVille::fadeOutButWaitForOpponent(int sceneIndex_, string oppo_uuid_) {
    
    BackendService::getInstance()->clearAllRequests();
    ((IsoMapBuilding*)isoMap)->saveInBackend(0);
    
    hudPanel->clearUIDialog();
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_opponent_get;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    m_visit_uuid = oppo_uuid_;
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex_;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneVille::fadeOutButWaitForWorldMap(int sceneIndex) {
    
    BackendService::getInstance()->clearAllRequests();
    ((IsoMapBuilding*)isoMap)->saveInBackend(0);
    
    hudPanel->clearUIDialog();
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = fire_map_get;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneVille::fire_map_get(void *sender) {

    if (BackendService::isCreated()) {
        g_playerState->WorldMapVillages.clear();
        g_opponentState->expectNewData();
        BackendService::getInstance()->send(APIMapGet::create(g_playerState,
                                                              PlayerState::onMapDataRetrieved));
    }
    
}

void SceneVille::fire_opponent_get(void *sender) {
    
    SceneVille* _this = (SceneVille*)g_currentScene;
    if (!_this) return;
    
    BackendService::getInstance()->cancelAllRequestsInQueue();
    BackendService::getInstance()->send(APIOpponentGet::create(_this->m_visit_uuid,
                                                               _this,
                                                               SceneVille::onOpponentRetrieved));
    
}

void SceneVille::onOpponentRetrieved(void *sender, std::string respn_str) {
    
    SceneVille* _this = (SceneVille*)g_currentScene;
    if (!_this) return;
    
    if (!PlayerState::getOpponent()->parseOppoData(respn_str.c_str())) return;
    
    g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
    g_sceneManager->switchScene(g_sceneManager);
    
}

void SceneVille::fadeOutThisSceneAndChangeTo(int sceneIndex) {
    
    BackendService::getInstance()->clearAllRequests();
    ((IsoMapBuilding*)isoMap)->saveInBackend(0);
    
    hudPanel->clearUIDialog();
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    
    g_sceneManager->sceneSelector = (SceneList)sceneIndex;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }
    
}

void SceneVille::fadeOutThisSceneButWaitForReplay(int sceneIndex_, string race_id_) {
 
    BackendService::getInstance()->clearAllRequests();
    ((IsoMapBuilding*)isoMap)->saveInBackend(0);
    
    hudPanel->clearUIDialog();
    
    if (g_sceneManager->transition) {
        g_sceneManager->transition->OnSceneFadedOut = _fireReplayGet;
    }
    
    scrollView->zoomOnPosition(scrollView->getContentView()*0.5f, scrollView->getMinimumScale(), 0.6f);
    
    m_view_replay_race_id = race_id_;
    g_sceneManager->sceneSelector = (SceneList)sceneIndex_;
    if (g_sceneManager->transition) {
        g_sceneManager->transition->fadeSceneOut(g_sceneManager);
    }

}

void SceneVille::_fireReplayGet(void *sender_) {
    
    SceneVille *_sceneVille = (SceneVille*)g_currentScene;
    
    PlayerState::getOpponent()->expectNewData();
    BackendService::getInstance()->send(APIRaceGet::create(_sceneVille->m_view_replay_race_id,
                                                           _sceneVille,
                                                           SceneVille::_onReplayRetrieved));
    
}

void SceneVille::_onReplayRetrieved(void *sender_, std::string resp_str) {
    
    if (g_opponentState->parseRaceReplay(resp_str.c_str())) {
        g_sceneManager->transition->OnSceneFadedOut = SceneManager::switchScene;
        g_sceneManager->switchScene(g_sceneManager);
        return;
    }
    
    CCAssert(false, "Replay-data received corrupted!");
    
}

void SceneVille::onLevelUpEffectDissmissed(void *sender_) {
    
    SceneVille *_this = (SceneVille*)sender_;
    
    _this->addSecondRaceTutorial();
    _this->addGachaTutorial();
    
}

void SceneVille::disableTouch(void *sender) {

//    g_sceneManager->phatTouches->disable();
    
    SceneVille *_this = (SceneVille*)sender;
    if (!_this) return;
    
    _this->scrollView->setScrollEnabled(false);
    
}

void SceneVille::enableTouch(void *sender) {
    
//    g_sceneManager->phatTouches->enable();
    
    SceneVille *_this = (SceneVille*)sender;
    if (!_this) return;
    
    _this->scrollView->setScrollEnabled(true);
    _this->hudPanel->showAllMainMenuButtons(true);

}

void SceneVille::willPopView(void *sender, void *view) {
    
    SceneVille *_this = (SceneVille*)sender;
    if (!_this) return;
        
    if (_this->itemStorageDialog != view) return;
    _this->itemStorageDialog = NULL;
    
}

void SceneVille::presentGachaPanel() {
    
    if (gachaPanel) return;
    
    gachaPanel = GachaPanel::create();
    gachaPanel->setDelegate(hudPanel);
    gachaPanel->presentPanel(this);

}

void SceneVille::runFBLoginDialogLogic()
{
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCLog_("=================== ENTERING  SCENE_VILLE ==================");
    
    unsigned int fblogindialog_hasloggedin = CCUserDefault::sharedUserDefault()->getBoolForKey("fblogindialog_hasloggedin");
    CCLog_("fblogindialog_hasloggedin:%d",fblogindialog_hasloggedin);
    
    PlayerState* _playerState = PlayerState::getInstance();
    bool _didClaimFBLogin = _playerState->Checklist["facebook_1"];
    
#if IS_DEBUG_BUILD
//#define TEST_OPEN_DIALOG
#endif
    
    


    
    
#ifdef TEST_OPEN_DIALOG
    FBLoginDialog *_dialog = FBLoginDialog::create(this->modalController, false);
    _dialog->setPosition(_screenSize*0.5f);
    this->addChild(this->modalController, 100);
    this->modalController->pushView(_dialog);
#endif
    
    
    if (!fblogindialog_hasloggedin && !_didClaimFBLogin) {
        
        unsigned int fblogindialog_last_opened = (unsigned int)CCUserDefault::sharedUserDefault()->getDoubleForKey("fblogindialog_last_opened");
        
        if (fblogindialog_last_opened == 0) {
            CCUserDefault::sharedUserDefault()->setDoubleForKey("fblogindialog_last_opened", (double)currentTime);
            fblogindialog_last_opened = currentTime;
        }
 
#ifdef TEST_OPEN_DIALOG
        CCUserDefault::sharedUserDefault()->setIntegerForKey("fblogindialog_times_opened", 0);
        CCUserDefault::sharedUserDefault()->setIntegerForKey("player_times_won", 10);
#endif
        int player_times_won = CCUserDefault::sharedUserDefault()->getIntegerForKey("player_times_won");
        
        int fblogindialog_times_opened = CCUserDefault::sharedUserDefault()->getIntegerForKey("fblogindialog_times_opened");
        CCLog_("fblogindialog_times_opened:%d",fblogindialog_times_opened);
        
#define MAX_FBLOGINDIALOG_TIMES_OPENED 3
        
        int SHOWFBLOGINDIALOG_1_PLAYER_TIMES_WON = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_1_PLAYER_TIMES_WON"];
        int SHOWFBLOGINDIALOG_2_PLAYER_TIMES_WON = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_2_PLAYER_TIMES_WON"];
        int SHOWFBLOGINDIALOG_3_PLAYER_TIMES_WON = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_3_PLAYER_TIMES_WON"];
        int SHOWFBLOGINDIALOG_1_TIME_SINCE_LAST_OPENED = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_1_TIME_SINCE_LAST_OPENED"];
        int SHOWFBLOGINDIALOG_2_TIME_SINCE_LAST_OPENED = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_2_TIME_SINCE_LAST_OPENED"];
        int SHOWFBLOGINDIALOG_3_TIME_SINCE_LAST_OPENED = g_gameContent->GameConfig["SHOWFBLOGINDIALOG_3_TIME_SINCE_LAST_OPENED"];
        
        CCLog_("player_times_won:%d",player_times_won);
        CCLog_("fblogindialog_last_opened:%d",fblogindialog_last_opened);
        CCLog_("currentTime:%d",currentTime);
        CCLog_("(currentTime - fblogindialog_last_opened):%d    (Time since dialog last opened",(currentTime - fblogindialog_last_opened));
        
        bool openFBLoginDialog = false;
        bool lastChance = false;
        bool isFBLoggedIn = g_sceneManager->facebook->isLoggedIn();
        
        CCLog_("isFBLoggedIn=%d", isFBLoggedIn);
        
        if(!isFBLoggedIn
           && fblogindialog_times_opened < MAX_FBLOGINDIALOG_TIMES_OPENED)
        {
            if (fblogindialog_times_opened == 0 &&
                player_times_won >= SHOWFBLOGINDIALOG_1_PLAYER_TIMES_WON &&
                (currentTime - fblogindialog_last_opened) >= SHOWFBLOGINDIALOG_1_TIME_SINCE_LAST_OPENED)
            {
                CCLog_("openning FBLoginDialog 1");
                openFBLoginDialog = true;
            }
            
            if (fblogindialog_times_opened == 1 &&
                player_times_won >= SHOWFBLOGINDIALOG_2_PLAYER_TIMES_WON &&
                (currentTime - fblogindialog_last_opened) >= SHOWFBLOGINDIALOG_2_TIME_SINCE_LAST_OPENED)
            {
                CCLog_("openning FBLoginDialog 1");
                openFBLoginDialog = true;
            }
            
            if (fblogindialog_times_opened == 2 &&
                player_times_won >= SHOWFBLOGINDIALOG_3_PLAYER_TIMES_WON &&
                (currentTime - fblogindialog_last_opened) >= SHOWFBLOGINDIALOG_3_TIME_SINCE_LAST_OPENED)
            {
                CCLog_("openning FBLoginDialog 1");
                openFBLoginDialog = true;
                lastChance = true;
            }
            CCLog_("openFBLoginDialog=%d", (int)openFBLoginDialog);
            
            if (openFBLoginDialog)
            {
                CCLog_("---> openFBLoginDialog! <---");
                CCUserDefault::sharedUserDefault()->setDoubleForKey("fblogindialog_last_opened", (double)currentTime);
                fblogindialog_last_opened = currentTime;
                
#ifndef TEST_OPEN_DIALOG
                CCUserDefault::sharedUserDefault()->setIntegerForKey("fblogindialog_times_opened", fblogindialog_times_opened+1);
                FBLoginDialog *_dialog = FBLoginDialog::create(this->modalController, false); //we're not using lastChance anymore
                _dialog->setPosition(_screenSize*0.5f);
                this->addChild(this->modalController, 100);
                this->modalController->pushView(_dialog);
#endif
            }
        }
    }
}
