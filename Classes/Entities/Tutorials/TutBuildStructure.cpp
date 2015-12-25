//
//  TutBuildStructure.cpp
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#include "TutBuildStructure.h"
USING_NS_CC;
using namespace std;

extern PlayerState* g_playerState;

#include "../../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../../Services/BackendService.h"
extern BackendService* g_gameBackend;

#include "../../Helpers/FlurryCpp.h"
#include "TutRaceResult.h"
#include "TutFirstRace.h"
#include "TutFeedPet.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"


bool TutBuildStructure::check(std::string buildingType_, std::string tutorialKey_)
{
    if (g_playerState->Checklist[tutorialKey_])
        return false;
    Building* _structure = g_playerState->getBuilding(buildingType_, 0);
    if(!_structure)
        return true;
    return _structure->pbVo.status != BUILDING_STATUS_DELIVERED && _structure->pbVo.status != BUILDING_STATUS_UPGRADE;
}

bool TutBuildStructure::init() {
    
    if (!CTutUnit::init())
        return false;
    
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    getTutorDialog(),
                                    CT_SPC_OPTION_NONE,
                                    this, TutBuildStructure::onTapTheBG));
    int building_index = 0;
    return m_isUpgradeTutorial ? setupUpgradingTutorials(building_index) : setupConstructionTutorials(building_index);
    
}

bool TutBuildStructure::setupConstructionTutorials(int index) {
    
    m_tap_shop_btn_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(hudPanel->mainMenu,
                                           hudPanel->menuButtons[HUDPanelMenuShop],
                                           true,
                                           true,
                                           CCPointZero,
                                           45.0f,
                                           this,
                                           TutBuildStructure::onShopButtonTapped));
    
    m_drag_building_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutDragBuildingToBuild());
    
    m_point_at_placeholder_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, this, TutBuildStructure::onBuildingBuilt));
    
    m_point_at_building_to_finishnow_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtBuilding(NULL, isoMapBuilding->entityAction, "", this, TutBuildStructure::onBuildingTappedToFinishNow));
    
    m_point_at_finishnow_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, false, CCPointZero, 45.0f, this, TutBuildStructure::onBuildingFinishNow));
    
    m_point_at_finishnow_btn_in_dialog_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, false, CCPointZero, 45.0f, this, TutBuildStructure::onBuildingFinishNowInDialog));
    
    m_validate_building_complete_index = m_tuts.size();
    m_tuts.push_back(new TutBuildStructureValidateComplete(index, this, TutBuildStructure::onBuildingValidatedComplete));
    
    m_point_at_building_to_open_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtBuilding(NULL, isoMapBuilding->entityAction, "", this, TutBuildStructure::onBuildingTapped));
    
    m_validate_building_delivered_index = m_tuts.size();
    m_tuts.push_back(new TutBuildStructureValidateDelivered(index, this, TutBuildStructure::onBuildingValidatedDelivered));
    
    m_end_of_tutorial_tut_index = m_tuts.size();
    forwardToTheCorrectTutSelector();
    
    return true;
}

bool TutBuildStructure::setupUpgradingTutorials(int index)
{
    auto building = g_playerState->getBuilding(getBuildingType(), index);
    
    if (scrollView && isoMapBuilding)
        m_tuts.push_back(new CTutFocusOnBuilding(scrollView, isoMapBuilding, building, this, TutBuildStructure::onBuildingFocused));
    
    m_point_at_building_to_upgrade_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtBuilding(building, isoMapBuilding->entityAction, "", this, TutBuildStructure::onBuildingTappedToUpgrade));
    
    m_point_at_upgrade_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, false, CCPointZero, 45.0f, this, TutBuildStructure::onUpgradeButtonTapped));
    
    m_point_at_upgrade_now_btn_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, false, CCPointZero, 45.0f, this, TutBuildStructure::onUpgradeNowButtonTapped));
    
    m_point_at_upgrade_now_btn_in_dialog_tut_index = m_tuts.size();
    m_tuts.push_back(new CTutPointAtButton(NULL, false, CCPointZero, 45.0f, this, TutBuildStructure::onUpgradeNowInDialogButtonTapped));
    
    m_end_of_tutorial_tut_index = m_tuts.size();
    m_tutSelector = 0;
    return true;
}

void TutBuildStructure::start() {
    
    hudPanel->setDangerButtonsEnabled(false);
    shopPanel->setManuallyCloseShopPanelEnabled(false);
    shopPanel->setItemTypeCanBeGrabbed(getBuildingType());
    shopPanel->setDangerButtonsEnabled(false);
    shopPanel->setZoomWhenAddBuildings(true);
    isoMapBuilding->setCancelBuildingPlacementEnabled(false);
    isoMapBuilding->setGrabTrackEnabled(false);
    isoMapBuilding->setGrabDecorationEnabled(false);
    isoMapBuilding->setGrabBuildingEnabled(false);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned(getBuildingType());
    
    FlurryCpp::logEvent(getAnalyticEventType(), true);
    FlurryCpp::logEvent(getAnalyticStartEventType());
    
#if !(IS_RELEASE_BUILD)
    hudPanel->getSettingMenu()->setEnabled(true);
#endif
}

void TutBuildStructure::finish() {
    
    hudPanel->setDangerButtonsEnabled(true);
    hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    shopPanel->setItemTypeCanBeGrabbed("");
    shopPanel->setDangerButtonsEnabled(true);
    shopPanel->setZoomWhenAddBuildings(false);
    isoMapBuilding->setGrabTrackEnabled(true);
    isoMapBuilding->setGrabDecorationEnabled(true);
    isoMapBuilding->setGrabBuildingEnabled(true);
    isoMapBuilding->entityAction->setEntityTypeCanBeOpenned("");
    
    g_playerState->checklist(getKey());
    
    FlurryCpp::endTimedEvent(getAnalyticEventType(), NULL);
    FlurryCpp::logEvent(getAnalyticEndEventType());
    
    FBAppEventsWrapper::logEvent(getAnalyticEndEventType());
}

void TutBuildStructure::update(float delta_) {
    
    CTutUnit::update(delta_);
    
    if(m_isUpgradeTutorial)
    {
        updateUpgradeTutorial(delta_);
        return;
    }
    updateBuildTutorial(delta_);
    
}

void TutBuildStructure::updateBuildTutorial(float delta_) {
    
    if (m_tutSelector == m_tap_shop_btn_index)
    {
        hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(true);
        
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton &&
            hudPanel->mainMenu &&
            hudPanel->areAllMainMenuButtonsShowed()) {
            _tut->menuButton = hudPanel->mainMenu;
        }
        
        return;
    }
    else
    {
        hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
    }
    
    if (m_tutSelector == m_point_at_placeholder_btn_tut_index)
    {
        if (!isoMapBuilding->placeHolder || !isoMapBuilding->placeHolder->menuYes)
            return;
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton)
            _tut->menuButton = isoMapBuilding->placeHolder->menuYes;
        return;
    }
    
    if (m_tutSelector == m_point_at_building_to_finishnow_tut_index)
    {
        if (this->forwardToTheCorrectTutSelectorExcludingConstruct())
        {
            CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)m_tuts[m_point_at_building_to_finishnow_tut_index];
            if(_tut)
                _tut->clearGraphics();
            return;
        }
        pointAtBuilding(BUILDING_STATUS_CONSTRUCT);
        return;
    }
    
    if (m_tutSelector == m_point_at_finishnow_btn_tut_index)
    {
        if(this->forwardToTheCorrectTutSelectorExcludingConstruct())
            return;
        
        if (!isoMapBuilding->entityAction)
            return;
        
        CCMenu* _menu = (CCMenu*)isoMapBuilding->entityAction->m_menuLayer->getChildByTag(getFinishNowButtonTag());
        if(!_menu) {
            //if placeholder was on illegal grid, failed to build, go back to the beginning
            if (isoMapBuilding->placeHolder) {
                m_tutSelector = m_point_at_placeholder_btn_tut_index;
            }
            return;
        }
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if(!_tut || _tut->menuButton)
            return;
        _tut->menuButton = _menu;
        return;
    }
    
    if (m_tutSelector == m_point_at_finishnow_btn_in_dialog_tut_index)
    {
        if(this->forwardToTheCorrectTutSelectorExcludingConstruct())
            return;
        
        if (modalController->getNStackView() == 0)
        {
            this->forwardToTheCorrectTutSelector();
            return;
        }
        
        InstantDialog* _dialog = (InstantDialog*)modalController->m_views[modalController->m_views.size()-1];
        if(!_dialog || !_dialog->instantMenu)
            return;
        
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton)
            _tut->menuButton = _dialog->instantMenu;
        return;
    }
    
    if (m_tutSelector == m_point_at_building_to_open_tut_index)
        pointAtBuilding(BUILDING_STATUS_COMPLETE);
}

void TutBuildStructure::updateUpgradeTutorial(float delta_)
{
    Building* _structure = g_playerState->getBuilding(getBuildingType(), 0);
    if(_structure && (_structure->pbVo.level > 1 && _structure->getBuildingStatus() == BUILDING_STATUS_DELIVERED))
    {
        CCAssert(m_tutSelector >= m_point_at_building_to_upgrade_tut_index, "invalid index");
        if(m_tutSelector < m_end_of_tutorial_tut_index && m_tutSelector > m_point_at_building_to_upgrade_tut_index)
            clearMenuButton(this);
        m_tutSelector = m_end_of_tutorial_tut_index;
        return;
    }
    
    if(m_tutSelector == m_point_at_building_to_upgrade_tut_index)
    {
        pointAtBuilding(NULL);
        return;
    }
    if(m_tutSelector == m_point_at_upgrade_btn_tut_index)
    {
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton)
            _tut->menuButton = (CCMenu*)isoMapBuilding->entityAction->m_menuLayer->getChildByTag(getUpgradeButtonTag());
        return;
    }
    if(m_tutSelector == m_point_at_upgrade_now_btn_tut_index)
    {
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton)
            _tut->menuButton = isoMapBuilding->entityAction->m_upgradeDialog->m_upgradeButton;
        return;
    }
    if(m_tutSelector == m_point_at_upgrade_now_btn_in_dialog_tut_index)
    {
        if (modalController->getNStackView() == 0)
        {
            m_tutSelector = m_point_at_building_to_upgrade_tut_index;
            return;
        }
        
        InstantDialog* _dialog = (InstantDialog*)modalController->m_views[modalController->m_views.size()-1];
        if(!_dialog || !_dialog->instantMenu)
            return;
        
        CTutPointAtButton* _tut = (CTutPointAtButton*)m_tuts[m_tutSelector];
        if (!_tut->menuButton)
            _tut->menuButton = _dialog->instantMenu;
        return;
    }
    return;
}

void TutBuildStructure::pointAtBuilding(const char* buildingStatusToTouch)
{
    int n = g_playerState->getNBuilding(getBuildingType());
    for (int i=0; i<n; i++)
    {
        Building* _structure = g_playerState->getBuilding(getBuildingType(), i);
        if(!_structure || (buildingStatusToTouch != NULL && _structure->pbVo.status != buildingStatusToTouch))
            continue;
        CTutPointAtBuilding* _tut = (CTutPointAtBuilding*)m_tuts[m_tutSelector];
        if (_tut && !_tut->building)
            _tut->building = _structure;
        return;
    }
}

void TutBuildStructure::clearMenuButton(TutBuildStructure* sender)
{
    CTutPointAtButton* _tut = (CTutPointAtButton*)sender->m_tuts[sender->m_tutSelector];
    _tut->menuButton = NULL;
}


void TutBuildStructure::forwardToTheCorrectTutSelector()
{
    int index = 0;
    if (typeid(*this) == typeid(TutBuildHabitat2)) {
        index = 1;
    }
    
    Building* _structure = g_playerState->getBuilding(getBuildingType(), index);
    if(!_structure)
        return;
    
    if (_structure->pbVo.status == BUILDING_STATUS_CONSTRUCT)
        m_tutSelector = m_point_at_building_to_finishnow_tut_index;
    else if (_structure->pbVo.status == BUILDING_STATUS_COMPLETE)
        m_tutSelector = m_validate_building_complete_index;
    else if (_structure->pbVo.status == BUILDING_STATUS_DELIVERED)
        m_tutSelector = m_validate_building_delivered_index;
}

bool TutBuildStructure::forwardToTheCorrectTutSelectorExcludingConstruct()
{
    int index = 0;
    if (typeid(*this) == typeid(TutBuildHabitat2)) {
        index = 1;
    }
    
    Building* _structure = g_playerState->getBuilding(getBuildingType(), index);
    if (_structure == NULL) return false;
    
    if (_structure->pbVo.status == BUILDING_STATUS_COMPLETE)
    {
        m_tutSelector = m_validate_building_complete_index;
        return true;
    }
    if (_structure->pbVo.status == BUILDING_STATUS_DELIVERED)
    {
        m_tutSelector = m_validate_building_delivered_index;
        return true;
    }
    return false;
}

#pragma mark - callback functions

void TutBuildStructure::onTapTheBG(void* sender_, std::string result_) {
    
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutSpeech>(_this))
        return;
    
    if (_this->scrollView->isMovingOrZooming())
        return;
    
    CTutSpeech* _tut = (CTutSpeech*)_this->m_tuts[_this->m_tutSelector];
    if (_tut->option == CT_SPC_OPTION_NONE)
        _this->goToNextTutorial();
    
}

void TutBuildStructure::onShopButtonTapped(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    
    //Update data for Drag Building Tutorial
    CCLog_("%d", (int)_this->shopPanel->m_itemBuildings.size());
    for (vector<ShopItemBuilding*>::iterator it = _this->shopPanel->m_itemBuildings.begin(); it != _this->shopPanel->m_itemBuildings.end(); ++it)
    {
        ShopItemBuilding* _buttonItem = (ShopItemBuilding*)(*it);
        if(!_buttonItem || (_buttonItem->vo.type != _this->getBuildingType()))
            continue;
        CTutDragBuildingToBuild* _tut = (CTutDragBuildingToBuild*)_this->m_tuts[_this->m_drag_building_tut_index];
        if(!_tut)
            continue;
        
        _tut->shopPanel = _this->shopPanel;
        _tut->buttonItem = _buttonItem;
        _tut->message = "";
        _tut->sender = _this;
        _tut->callback = TutBuildStructure::onBuildingAdded;
        break;
    }
    _this->goToNextTutorial();
}

void TutBuildStructure::onBuildingAdded(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutDragBuildingToBuild>(_this))
        return;
    
    if (result_ == _this->getBuildingType())
    {
        _this->goToNextTutorial();
        return;
    }
    
    //clean before goback
    CTutDragBuildingToBuild* _tut = (CTutDragBuildingToBuild*)_this->m_tuts[_this->m_drag_building_tut_index];
    if (_tut)
    {
        _tut->shopPanel = NULL;
        _tut->buttonItem = NULL;
        _tut->message = "";
        _tut->sender = NULL;
        _tut->callback = NULL;
    }
    _this->goToPreviousTutorial();
}

void TutBuildStructure::onBuildingBuilt(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    
    if (result_ == CTutPointAtButtonFailed){
        _this->m_tutSelector = _this->m_tap_shop_btn_index;
        _this->start();
    } else {
        _this->hudPanel->menuButtons[HUDPanelMenuShop]->setEnabled(false);
        _this->m_tutSelector = _this->m_point_at_finishnow_btn_tut_index;
    }
}


void TutBuildStructure::onBuildingTappedToFinishNow(void* sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtBuilding>(_this))
        return;
    
    if (result_ == CTutPointAtBuildingFailed)
        _this->goToPreviousTutorial();
    else
        _this->goToNextTutorial();
}

void TutBuildStructure::onBuildingFinishNow(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    
    if (result_ == CTutPointAtButtonFailed) {
        _this->m_tutSelector = _this->m_point_at_building_to_finishnow_tut_index;
    }else {
        _this->m_tutSelector = _this->m_point_at_finishnow_btn_in_dialog_tut_index;
    }
}

void TutBuildStructure::onBuildingFinishNowInDialog(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    
    if (result_ == CTutPointAtButtonFailed) {
        _this->m_tutSelector = _this->m_point_at_building_to_finishnow_tut_index;
    }else {
        _this->m_tutSelector = _this->m_validate_building_delivered_index; //building will finish to delivered right away, skipping tapping the build to open
    }
}

void TutBuildStructure::onBuildingValidatedComplete(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<TutBuildStructureValidateComplete>(_this))
        return;
    
    if (result_ == TutBuildStructureValidateSuccess)
        _this->m_tutSelector = _this->m_point_at_building_to_open_tut_index;
}

void TutBuildStructure::onBuildingTapped(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtBuilding>(_this))
        return;
    
    _this->isoMapBuilding->entityAction->setEntityTypeCanNOTBeTouched("");
    if (result_ == CTutPointAtBuildingFailed)
        _this->goToPreviousTutorial();
    else
        _this->m_tutSelector = _this->m_validate_building_delivered_index;
}

void TutBuildStructure::onBuildingTappedToUpgrade(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtBuilding>(_this))
        return;
    if (result_ == CTutPointAtBuildingFailed)
        return;
    _this->goToNextTutorial();
}

void TutBuildStructure::onBuildingFocused(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutFocusOnBuilding>(_this))
        return;
    _this->goToNextTutorial();
}

void TutBuildStructure::onUpgradeButtonTapped(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    if (result_ == CTutPointAtButtonFailed)
    {
        _this->m_tutSelector = _this->m_point_at_building_to_upgrade_tut_index;
        return;
    }
    Building* _structure = g_playerState->getBuilding(_this->getBuildingType(), 0);
    CCAssert(_structure, "missing building");
    if(_structure->pbVo.status == BUILDING_STATUS_UPGRADE)
    {
        _this->m_tutSelector = _this->m_point_at_upgrade_now_btn_in_dialog_tut_index;
        return;
    }
    _this->goToNextTutorial();
}

void TutBuildStructure::onUpgradeNowButtonTapped(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    if (result_ == CTutPointAtButtonFailed)
    {
        _this->m_tutSelector = _this->m_point_at_building_to_upgrade_tut_index;
        return;
    }
    _this->goToNextTutorial();
}

void TutBuildStructure::onUpgradeNowInDialogButtonTapped(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<CTutPointAtButton>(_this))
        return;
    clearMenuButton(_this);
    if (result_ == CTutPointAtButtonFailed)
    {
        _this->m_tutSelector = _this->m_point_at_building_to_upgrade_tut_index;
        return;
    }
    _this->goToNextTutorial();
}

void TutBuildStructure::onBuildingValidatedDelivered(void *sender_, std::string result_)
{
    TutBuildStructure* _this = (TutBuildStructure*)sender_;
    if(!checkTutorialType<TutBuildStructureValidateDelivered>(_this))
        return;
    
    if (result_ == TutBuildStructureValidateSuccess)
        _this->goToNextTutorial(); //end tutorial
}

#pragma mark - TutBuildStructureValidate

void TutBuildStructureValidateState::update(float delta)
{
    if (!callback)
        return;
    TutBuildStructure* _sender = (TutBuildStructure*)sender;
    Building* _structure = g_playerState->getBuilding(_sender->getBuildingType(), index);
    if(!_structure)
        return;
    
    CCLog_("TutBuildStructureValidateComplete _structure->pbVo.status:%s",_structure->pbVo.status.c_str());
    if (_structure->pbVo.status == state)
        callback(sender, TutBuildStructureValidateSuccess);
}

#pragma mark - TutBuildHabitat

bool TutBuildHabitat::check(std::string buildingType_, std::string tutorialKey_)
{
    if (g_playerState->Checklist[tutorialKey_] || g_playerState->getNPet() > 0)
        return false;
    Building* _structure = g_playerState->getBuilding(buildingType_, 0);
    if(!_structure)
        return true;
    return _structure->pbVo.status != BUILDING_STATUS_DELIVERED;
}

const char* TutBuildHabitat::getKey() const
{
    return TutBuildHabitat::key();
}

const char* TutBuildHabitat::getAnalyticEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT;
}

const char* TutBuildHabitat::getAnalyticStartEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT_START;
}

const char* TutBuildHabitat::getAnalyticEndEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT_END;
}

const char* TutBuildHabitat::getTutorDialog() const
{
    return g_gameContent->Word["tut_build_habitat_speech_1"].c_str();
}

int TutBuildHabitat::getFinishNowButtonTag() const
{
    return FIRST_BUTTON;
}

void TutBuildHabitat::start()
{
    
    TutBuildStructure::start();
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    
#if !(IS_RELEASE_BUILD)
    hudPanel->setVisibilitySettingButton(true);
#endif
}

#pragma mark - TutBuildHabitat2

bool TutBuildHabitat2::init() {
    
    if (!CTutUnit::init())
        return false;
    
    GDBuildingVO _animal_barn_vo = g_gameContent->getBuildingVO(kBuildingAnimal, 1);
    GDBuildingVO _habitat_vo = g_gameContent->getBuildingVO(kBuildingHabitat, 1);
    if (_animal_barn_vo.type == "" && _habitat_vo.type == "") {
        return false;
    }
    
    CREATE_CHAR_BUFFER(_buffer, 128);
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_build_habitat2_speech_1"].c_str(), _animal_barn_vo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleNote, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE,
                                    this, TutBuildHabitat2::onTapTheBG));
    
    snprintf(_buffer, _buffer_size, g_gameContent->Word["tut_build_habitat2_speech_2"].c_str(), _habitat_vo.title.c_str());
    m_tuts.push_back(new CTutSpeech(CT_CHR_UNCLE, CT_CHR_SIDE_LEFT,
                                    kUncleExplain, false,
                                    _buffer,
                                    CT_SPC_OPTION_NONE,
                                    this, TutBuildHabitat2::onTapTheBG));
    
    int second_habitat_index = 1;
    return setupConstructionTutorials(second_habitat_index);
    
}

bool TutBuildHabitat2::check(std::string buildingType_, std::string tutorialKey_)
{
    if (!g_playerState->Checklist[TutBuildHabitat2::key()] &&
        !g_playerState->getBuilding(kBuildingHabitat, 1) && // doesn't have 2 habitats
        g_playerState->getNPet() > 1) { // has more than one pet
        return true;
    }
    
    return false;
}

const char* TutBuildHabitat2::getKey() const
{
    return TutBuildHabitat2::key();
}

const char* TutBuildHabitat2::getAnalyticEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT2;
}

const char* TutBuildHabitat2::getAnalyticStartEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT2_START;
}

const char* TutBuildHabitat2::getAnalyticEndEventType() const
{
    return EVENT_TUT_TUTBUILDHABITAT2_END;
}

const char* TutBuildHabitat2::getTutorDialog() const
{
    //TODO: fiix
    return g_gameContent->Word["tut_build_habitat2_speech_2"].c_str();
}

int TutBuildHabitat2::getFinishNowButtonTag() const
{
    return FIRST_BUTTON;
}

void TutBuildHabitat2::start()
{
    TutBuildStructure::start();
}

#pragma mark - TutBuildFeedMill

bool TutBuildFeedMill::check(std::string buildingType_, std::string tutorialKey_)
{
    if (g_playerState->Checklist[tutorialKey_] || !g_playerState->Checklist[TutFeedPet::key()])
        return false;
    Building* _structure = g_playerState->getBuilding(buildingType_, 0);
    if(!_structure)
        return true;
    return _structure->pbVo.status != BUILDING_STATUS_DELIVERED;
}

const char* TutBuildFeedMill::getKey() const
{
    return TutBuildFeedMill::key();
}

const char* TutBuildFeedMill::getAnalyticEventType() const
{
    return EVENT_TUT_TUTBUILDFEEDMILL;
}

const char* TutBuildFeedMill::getAnalyticStartEventType() const
{
    return EVENT_TUT_TUTBUILDFEEDMILL_START;
}

const char* TutBuildFeedMill::getAnalyticEndEventType() const
{
    return EVENT_TUT_TUTBUILDFEEDMILL_END;
}

const char* TutBuildFeedMill::getTutorDialog() const
{
    return g_gameContent->Word["tut_build_feedmill_speech_1"].c_str();
}

int TutBuildFeedMill::getFinishNowButtonTag() const
{
    return SECOND_BUTTON;
}

void TutBuildFeedMill::start()
{
    
    TutBuildStructure::start();
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    
#if !(IS_RELEASE_BUILD)
    hudPanel->setVisibilitySettingButton(true);
#endif
}

#pragma mark - TutUpgradeMansion

bool TutUpgradeMansion::check(std::string buildingType_, std::string tutorialKey_)
{
    if (g_playerState->Checklist[tutorialKey_])
        return false;
    if (!g_playerState->Checklist[TutRaceResult::key(1)] && !g_playerState->Checklist[TutRaceResult::key(2)])
        return false;
    
    Building* _structure = g_playerState->getBuilding(buildingType_, 0);
    if(!_structure || _structure->pbVo.level > 1)
        return false;
    return _structure->pbVo.status == BUILDING_STATUS_DELIVERED;
}

const char* TutUpgradeMansion::getKey() const
{
    return TutUpgradeMansion::key();
}

const char* TutUpgradeMansion::getAnalyticEventType() const
{
    return EVENT_TUT_UPGRADEMANSION;
}

const char* TutUpgradeMansion::getAnalyticStartEventType() const
{
    return EVENT_TUT_UPGRADEMANSION_START;
}

const char* TutUpgradeMansion::getAnalyticEndEventType() const
{
    return EVENT_TUT_UPGRADEMANSION_END;
}

const char* TutUpgradeMansion::getTutorDialog() const
{
    return g_gameContent->Word["tut_upgrade_mansion_speech_1"].c_str();
}

int TutUpgradeMansion::getUpgradeButtonTag() const
{
    return SECOND_BUTTON;
}

void TutUpgradeMansion::start()
{
    TutBuildStructure::start();
    
    hudPanel->setVisibilityAll(false);
    hudPanel->setVisibilityExpBarAndName(true);
    hudPanel->setVisibilityResourceBar(true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuShop, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuMap, true);
    hudPanel->setVisibilityMainMenuButton(HUDPanelMenuHome, true);
    
#if !(IS_RELEASE_BUILD)
    hudPanel->setVisibilitySettingButton(true);
#endif
}
