//
//  TutBuildStructure.h
//  Cheetah
//
//  Created by Praveen on 23/06/15.
//
//

#ifndef __Cheetah__TutBuildStructure__
#define __Cheetah__TutBuildStructure__

#include "CTutManager.h"

#include "../../UIs/Panels/HUDPanel.h"
#include "../../UIs/ShopPanel.h"
#include "../../Entities/IsoMapBuilding.h"
#include "../../Extensions/Modal/CCModalController.h"

#define TutBuildStructureValidateSuccess "success"

#define FIRST_BUTTON 0
#define SECOND_BUTTON 1

class TutBuildStructureValidateState : public CTut
{
protected:
    int index;
    std::string state;
public:
    TutBuildStructureValidateState(int index,
                                   std::string state,
                                   void* sender,
                                   void (*callback)(void* sender, std::string result_)) {
        this->index = index;
        this->state = state;
        this->sender = sender;
        this->callback = callback;
    }
    virtual void update(float delta_);
};

class TutBuildStructureValidateComplete : public TutBuildStructureValidateState
{
public:
    TutBuildStructureValidateComplete(int index, void* sender, void (*callback)(void* sender, std::string result)) :
    TutBuildStructureValidateState(index, BUILDING_STATUS_COMPLETE, sender, callback){}
};

class TutBuildStructureValidateDelivered : public TutBuildStructureValidateState
{
public:
    TutBuildStructureValidateDelivered(int index, void* sender, void (*callback)(void* sender, std::string result)) :
    TutBuildStructureValidateState(index, BUILDING_STATUS_DELIVERED, sender, callback){}
};

class TutBuildStructure : public CTutUnit
{
protected:
    
    int m_tap_shop_btn_index;
    int m_drag_building_tut_index;
    int m_point_at_placeholder_btn_tut_index;
    int m_point_at_building_to_finishnow_tut_index;
    int m_point_at_finishnow_btn_tut_index;
    int m_point_at_finishnow_btn_in_dialog_tut_index;
    int m_validate_building_complete_index;
    int m_point_at_building_to_open_tut_index;
    int m_validate_building_delivered_index;
    
    int m_point_at_building_to_upgrade_tut_index;
    int m_point_at_upgrade_btn_tut_index;
    int m_point_at_upgrade_now_btn_tut_index;
    int m_point_at_upgrade_now_btn_in_dialog_tut_index;
    
    int m_end_of_tutorial_tut_index;
    
    void pointAtBuilding(const char* buildingStatusToTouch);
    
    static void clearMenuButton(TutBuildStructure* sender);
    
    template<class T> static bool checkTutorialType(TutBuildStructure* sender)
    {
        if (sender->m_tutSelector >= sender->m_tuts.size())
            return false;
        T* _tut = (T*)sender->m_tuts[sender->m_tutSelector];
        return _tut && (typeid(*_tut) == typeid(T));
    }
    
    bool m_isUpgradeTutorial;
    
public:
    
    /** required instance pointers **/
    HUDPanel* hudPanel;
    ShopPanel* shopPanel;
    IsoMapBuilding* isoMapBuilding;
    CCModalController* modalController;
    CCNikScrollView* scrollView;
    std::string buildingType;
    
    static bool check(std::string buildingType_, std::string tutorialKey_);
    template<class T> static TutBuildStructure* create(std::string buildingType_, bool isUpgradeTutorial, HUDPanel* hudPanel_, ShopPanel* shopPanel_, IsoMapBuilding* isoMapBuilding_, CCModalController* modalController_, CCNikScrollView* scrollView_)
    {
        if (!T::check(buildingType_, T::key()))
            return NULL;
        T* pRet = new T();
        CCAssert(pRet, "failed to allocate memory");
        pRet->hudPanel = hudPanel_;
        pRet->shopPanel = shopPanel_;
        pRet->isoMapBuilding = isoMapBuilding_;
        pRet->modalController = modalController_;
        pRet->scrollView = scrollView_;
        pRet->buildingType = buildingType_;
        pRet->m_key = T::key();
        pRet->m_isUpgradeTutorial = isUpgradeTutorial;
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
    
    virtual bool init();
    virtual bool setupConstructionTutorials(int index);
    virtual bool setupUpgradingTutorials(int index);
    virtual void start();
    virtual void finish();
    virtual void update(float delta_);
    virtual void updateBuildTutorial(float delta_);
    virtual void updateUpgradeTutorial(float delta_);
    
    virtual const char* getBuildingType() const { return buildingType.c_str(); }
    
    virtual const char* getKey() const = 0;
    virtual const char* getAnalyticEventType() const = 0;
    virtual const char* getAnalyticStartEventType() const = 0;
    virtual const char* getAnalyticEndEventType() const = 0;
    virtual const char* getTutorDialog() const = 0;
    virtual int getFinishNowButtonTag() const = 0;
    virtual int getUpgradeButtonTag() const = 0;
    
    void forwardToTheCorrectTutSelector();
    bool forwardToTheCorrectTutSelectorExcludingConstruct();
    
    static void onTapTheBG(void* sender_, std::string result_);
    static void onShopButtonTapped(void* sender_, std::string result_);
    static void onBuildingAdded(void* sender_, std::string result_);
    static void onBuildingBuilt(void* sender_, std::string result_);
    static void onBuildingTappedToFinishNow(void* sender_, std::string result_);
    static void onBuildingFinishNow(void* sender_, std::string result_);
    static void onBuildingFinishNowInDialog(void* sender_, std::string result_);
    static void onBuildingValidatedComplete(void* sender_, std::string result_);
    static void onBuildingValidatedDelivered(void* sender_, std::string result_);
    static void onBuildingTapped(void* sender_, std::string result_);
    static void onBuildingTappedToUpgrade(void* sender_, std::string result_);
    static void onBuildingFocused(void* sender_, std::string result_);
    static void onUpgradeButtonTapped(void* sender_, std::string result_);
    static void onUpgradeNowButtonTapped(void* sender_, std::string result_);
    static void onUpgradeNowInDialogButtonTapped(void* sender_, std::string result_);
};

class TutBuildHabitat : public TutBuildStructure
{
public:
    static bool check(std::string buildingType_, std::string tutorialKey_);
    static const char* key() { return "build_habitat"; }
    virtual const char* getKey() const;
    virtual const char* getAnalyticEventType() const;
    virtual const char* getAnalyticStartEventType() const;
    virtual const char* getAnalyticEndEventType() const;
    virtual const char* getTutorDialog() const;
    virtual int getFinishNowButtonTag() const;
    virtual int getUpgradeButtonTag() const { return 0; }
    
    virtual void start();
};

class TutBuildHabitat2 : public TutBuildStructure
{
public:
    static bool check(std::string buildingType_, std::string tutorialKey_);
    static const char* key() { return "build_habitat2"; }
    virtual const char* getKey() const;
    virtual const char* getAnalyticEventType() const;
    virtual const char* getAnalyticStartEventType() const;
    virtual const char* getAnalyticEndEventType() const;
    virtual const char* getTutorDialog() const;
    virtual int getFinishNowButtonTag() const;
    virtual int getUpgradeButtonTag() const { return 0; }
    
    virtual bool init();
    virtual void start();
};

class TutBuildFeedMill : public TutBuildStructure
{
public:
    static bool check(std::string buildingType_, std::string tutorialKey_);
    static const char* key() { return "build_feed_mill"; }
    virtual const char* getKey() const;
    virtual const char* getAnalyticEventType() const;
    virtual const char* getAnalyticStartEventType() const;
    virtual const char* getAnalyticEndEventType() const;
    virtual const char* getTutorDialog() const;
    virtual int getFinishNowButtonTag() const;
    virtual int getUpgradeButtonTag() const { return 0; }
    
    virtual void start();
};

class TutUpgradeMansion : public TutBuildStructure
{
public:
    static bool check(std::string buildingType_, std::string tutorialKey_);
    static const char* key() { return "upgrade_mansion"; }
    virtual const char* getKey() const;
    virtual const char* getAnalyticEventType() const;
    virtual const char* getAnalyticStartEventType() const;
    virtual const char* getAnalyticEndEventType() const;
    virtual const char* getTutorDialog() const;
    virtual int getFinishNowButtonTag() const { return 0; }
    virtual int getUpgradeButtonTag() const;
    
    virtual void start();
};

#endif /* defined(__Cheetah__TutBuildStructure__) */
