//
//  CTutAchievement.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "CTutAchievement.h"
#include "TutBuildStructure.h"
#include "../../Services/GameContentService.h"
#import "../../Helpers/FlurryCpp.h"
#include "../../Extensions/Modal/CCModalController.h"

#include "../../Helpers/CustomEventsHelper.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

extern PlayerState* g_playerState;
extern GameContentService* g_gameContent;

std::string TutAchievement::key()
{
    return "do_achievement";
}

bool TutAchievement::check()
{
    return !g_playerState->Checklist[key()] && g_playerState->Checklist[TutBuildHabitat2::key()];
}

TutAchievement *TutAchievement::create(HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_)
{
    if(!check())
        return NULL;
    
    TutAchievement* tut = new TutAchievement();
    tut->hudPanel = hudPanel_;
    tut->shopPanel = shopPanel_;
    tut->isoMapBuilding = isoMapBuilding_;
    tut->modalController = modalController_;
    
    if(!tut->init())
    {
        CC_SAFE_DELETE(tut);
        return NULL;
    }
    tut->autorelease();
    return tut;
}

bool TutAchievement::init()
{
    if(!CTutUnit::init())
        return false;
    
    m_key = key();
    
    m_first_dialog_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleNote, false,
                                    g_gameContent->Word["tut_achievement_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, tapBGAndContinue));
    
    m_point_at_achievement_button_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(hudPanel->mainMenu,
                                           hudPanel->menuButtons[HUDPanelMenuAchievement],
                                           true, CCPointZero, 135.0f,
                                           this, onTapAchievementButton));
    
    m_last_dialog_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleHappy, false,
                                    g_gameContent->Word["tut_tutorial_end_speech_1"].c_str(),
                                    CT_SPC_OPTION_NONE,
                                    this, tapBGAndEndTutorial));
    
    m_tutSelector = 0;
    return true;
}

void TutAchievement::start()
{
    hudPanel->setDangerButtonsEnabled(false);
    hudPanel->menuButtons[HUDPanelMenuAchievement]->setEnabled(true);
    shopPanel->setDangerButtonsEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("none");
    
    FlurryCpp::logEvent(EVENT_TUT_ACHIEVEMENT, true);
    FlurryCpp::logEvent(EVENT_TUT_ACHIEVEMENT_START);
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuHome, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuGacha, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuAchievement, true);
}

void TutAchievement::finish()
{
    hudPanel->setDangerButtonsEnabled(true);
    shopPanel->setDangerButtonsEnabled(true);
    shopPanel->setManuallyCloseShopPanelEnabled(true);
    isoMapBuilding->setCancelBuildingPlacementEnabled(true);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    g_playerState->checklist(key());
    
    FlurryCpp::endTimedEvent(EVENT_TUT_ACHIEVEMENT, NULL);
    FlurryCpp::logEvent(EVENT_TUT_ACHIEVEMENT_END);
    
    hudPanel->setVisibilityAll(true);
    hudPanel->setEnableTrackSignBeating(true);
}

void TutAchievement::update(float delta_)
{
    CTutUnit::update(delta_);
    
    if(waitingForAchievementWindowToClose && (modalController->getNStackView() == 0))
    {
        waitingForAchievementWindowToClose = false;
        goToNextTutorial();
    }
}

void TutAchievement::tapBGAndContinue(void *sender_, std::string result_)
{
    auto _this = (TutAchievement*)sender_;
    _this->goToNextTutorial();
}

void TutAchievement::tapBGAndEndTutorial(void *sender_, std::string result_)
{
    auto _this = (TutAchievement*)sender_;
    _this->goToNextTutorial();
    
    
    //---- things to do just after tutorial is completed ----
    
    _this->hudPanel->setOverrideExploreButtonBadgeNumber(AVAILABLE_VILLAGES_AFTER_TUTORIAL_ENDS);
    
    //
    // the tutorial has finished
    //
    
    CustomEventsHelper::Instance().doAfterTutorialsEnds();
    
    CCDictionary* params = CCDictionary::create();
    int pNameInt = (int)_FBAppEventParameterNameSuccess;
    std::stringstream pName;
    pName << pNameInt;
    params->setObject(ccs("1"), pName.str().c_str());
    FBAppEventsWrapper::logEvent(_FBAppEventNameCompletedTutorial,
                                 params);
    
    AppsFlyerWrapper::trackEvent(EVENT_TUTORIAL_COMPLETED);
}

void TutAchievement::onTapAchievementButton(void *sender_, std::string result_)
{
    TutAchievement* _this = (TutAchievement*)sender_;
    if(result_ != CTutPointAtButtonFailed)
        _this->waitingForAchievementWindowToClose = true;
}
