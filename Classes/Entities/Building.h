//
//  Building.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#ifndef __Cheetah__Building__
#define __Cheetah__Building__

#include "cocos2d.h"
#include <spine/spine-cocos2dx.h>
#include "../VOs/PlayerBuildingVO.h"
#include "Pet.h"
#include "../UIs/Elements/ProgressBar.h"
#include "../Extensions/CCPhatTouchableSprite.h"

#include "../VOs/GDItemVO.h"
#include "../Settings.h"

#define kPanelLabelColor    ccc3(6.0f, 57.0f, 94.0f)//ccc3(0.0f, 99.0f, 131.0f)
#define kPanelLabelColor2   ccc3(200.0f, 200.0f, 200.0f)

#define kBuildingSrcPath        "common/building/"
#define kBuildingSpecificFront  "front"
#define HABITAT_PET_MAX 4

//location to go after remove pet
//#define kAddToAnimalBarn 1
//#define kAddToAvailableHabitat 2

#define COLLECTING_LIGHT_MAX    2

#define kBuldingMinimumProducedAmountToCollect 1.0f
#define kBuildingRetrivingItemLayerZOrder   100

enum BuildingUpgradeType {
    BuildingUpgradeTypeNormal,
    BuildingUpgradeTypeInstant,
    BuildingUpgradeTypeBuyMissingResources
};

enum BuildingBuildType {
    BuildingBuildTypeNormal,
    BuildingBuildTypeBuyMissingResources
};

class Building : public cocos2d::CCNode {
    
    float                                   m_delta;
    bool                                    m_active;
    int                                     m_countDown;
    int                                     m_countUp;
    int                                     m_prev_level;
    
    cocos2d::CCLayer*                           m_spineLayer;
    cocos2d::extension::CCSkeletonAnimation*    m_finishAnimation;
    
    ProgressBar                             *m_progress_bar;
    ProgressBar                             *m_energy_bar;
    ProgressBar                             *m_ability_bar;

    
    float                                   m_params[3];
    float                                   m_resourceProduced;

//pragma mark - mark for actions
    bool                                    m_markForFinishNow;
    void                                    *m_holdFinishNowSender;
    void                                    (*m_holdFinishNowCallback)(void*,std::string);
    
    bool                                    m_markForPetFuse;
    GDPetVO                                 m_holdPetFuseVo;
    bool                                    m_holdPetFuseInstant;
    void                                    *m_holdPetFuseSender;
    void                                    (*m_holdPetFuseCallback)(void*, std::string);
    
    bool                                    m_markForPetRetrieve;
    void                                    *m_holdPetRetrieveSender;
    void                                    (*m_holdPetRetrieveCallback)(void*, std::string);
    

    
    bool _showProgressBar();
    void _hideProgressBar();
    
    bool _showEnergyBar();
    void _hideEnergyBar();
    
    bool _showAbilityBar(void* ability_);
    void _hideAbilityBar();
    
    cocos2d::CCSprite*                      m_collectingBubble;
    cocos2d::CCAction*                      m_collectingAttention;
    
    bool _showCollectingBubble(bool max_);
    void playSoundAnimateToBarn();
    
    cocos2d::CCLayer                        *m_retrivingItemLayer;
    void _checkAndCreateRetrievingItemLayer();
    void _checkAndDeleteRetrievingItemLayer(bool force_delete_);
    
    std::vector<GDPetVO>                    m_gotPets;
    int                                     animateAddAnimalToBarnIndex;
    
    std::vector<GDItemVO>                   m_gotItems;
    int                                     animateAddItemToBarnIndex;
   
    cocos2d::CCSprite*                      m_upgradableSignSprite;
    void _updateUpgradableSign();
   
public:
    
    enum ActionResult {
        OK,
        NOT_ENOUGH_COINS,
        MAXIMUM_LEVEL_REACHED,
        BUILDER_IS_BUSY,
    };
    
    bool addPetAnimationRunningWithSmallDelay;

    bool didMove;
    bool bubbleStatus;
    void showBubbleTrade();
    void hideBubbleTrade();
    
    void entityActionStart(void *entityAction);
    void entityActionFinish(void *entityAction);
    
    CCSprite *_bubble;
    CCSprite *_bubble_maket;
    void*                                   preferenceRootParent;
    
    cocos2d::extension::CCSkeletonAnimation* spineConstruction;
    cocos2d::extension::CCSkeletonAnimation* spineUpgrade;
    cocos2d::extension::CCSkeletonAnimation* spineBuilding;
    cocos2d::extension::CCSkeletonAnimation* spineBuildingFront;
    
    GDBuildingVO                            bVo;
    PlayerBuildingVO                        pbVo;
    
    CCNode                                  *parent;
    Pet                                     *pet;
//    TouchableItem                           *product;
    CCPhatTouchableSprite                   *product;
    bool                                    didTryToCollect;
    bool                                    m_bIsHost = false;
    
    cocos2d::extension::CCSkeletonAnimation *spine_pet_running = NULL;
    
    void (*onBuildingWillOpen)(void*);
    void *onBuildingWillOpenSender;
    
    static Building* createWithBuildingVO(GDBuildingVO bvo);
    static Building* createWithBuildingVO(GDBuildingVO bvo, PlayerBuildingVO pbvo);
    
    bool initWithBuildingVO(GDBuildingVO vo, PlayerBuildingVO pbvo);
    virtual void update(float delta);
    void updateTimer(float delta);

    bool isStateEqual(PlayerBuildingVO vo);
    bool newState(BuildingBuildType buildType, bool update_in_backend);
    bool syncState(PlayerBuildingVO vo);
    void finishState();
    bool finishNowState(void *sender, void (*callback)(void*,std::string));
    void openState();
    ActionResult upgradeState();
    ActionResult upgradeState(BuildingUpgradeType upgradeType, int gems_cost);
    
    //Core functions
    bool createBuilding();
    bool createBuilding(bool bumping);
    bool createBuilding(bool bumping, bool force);
    bool initSpineConstruction(bool force);
    bool initSpineUpgrading(bool force);

    /** This function should be called before leave IsoMap */
    void willRemoveFromIsoMap();
    
    /** Mark for actions */
    void markForFinishNow(void *sender, void (*callback)(void*,std::string));
    void unmarkForFinishNow();
    void fireForFinishNow();
    
    void markForPetRetrieve(void *sender, void (*callback)(void*,std::string));
    void unmarkForPetRetrieve();
    void fireForPetRetrieve();
    
    void markForPetFuse(GDPetVO pet_vo,
                        bool instant,
                        void *sender,
                        void (*callback)(void*,std::string));
    void unmarkForPetFuse();
    void fireForPetFuse();
    
    void updateTrackPointsIfThisIsRaceAsso();
    
    void collect();
    void collect(bool tell_backend_);
    bool collectingEffect(int collect_amount_);
    static void onResourceTouched(void *inst, void *sender);
    static void onResourceCollected(void *sender, std::string json_str);
    bool isMovable();
    
    void move();
    std::string getMoveJsonRendered();
    
    void updateCountdown();
    void updateFlip();
//    void constructionGraphic();
//    void upgradingGraphic();
    void updateProgressBarVisibility();
    
//    void openBuilding();
//    void openBuildingInBackend();
    std::string getBuildingStatus();
    void setBuildingStatus(std::string status);
    void runUpgradeFinishEffect();
    void nullUpgradeFinishEffect();
    void cleanEffect();
    
    void hideProgressBarAnimated(float duration_);
    void showProgressBarAnimated(float duration_);
    
    // Functions for Habitat building type
    bool addPetToBuilding(Pet *pet_);
    void setPetAsHost(bool host_);
    void removePetFromBuilding();
    void playDropAnimalSound();
    void addPetToBuildingAnimationDidFinish();
    void addPetToBuildingAnimationDidFinishWithSmallDelay();
    void removePetFromBuildingAnimationDidFinish();
    bool isPetContained();
    
    void setInitialProduct();
    // Functions for Food Production building type
    void setInitialProducedFood();
    void updateFoodProduction();
    float getAmountEnoughToCollectFood();
    float getAmountToShowFoodIcon();
    void emptyResourceProduced() { m_resourceProduced = 0; }
    
    float getProductRate();
    float getNextProductRate();
    float getFoodCapacityRate();
    float getNextFoodCapacityRate();
    
    cocos2d::CCPoint getResourcePosition();
    int getSecondsLeftToFinish();
    float getPercentToFinish(GDBuildingVO nxt_bvo_);

    //Funstion for Coin Earning (ticket booth building type)
    void setInitialCoins();
    void updateCoinEarning();
    float getAmountEnoughToCollectCoin();
    float getAmountToShowCoinIcon();
    
    //Common Functions
    void setActive(bool active_);
    void hideCollectingBubble();
    float getResourceProduced();
    void deductResourceProduced(float collect_amount_);
    
    
    void addAnimalToBarn(std::vector<GDPetVO> gotPets);
    void animateAnimalToBarn();
    
    void addItemToBarn(std::vector<GDItemVO> gotItems);
    void animateItemToBarn();
    
    void checkPBVOType(float delta_);
    
};

#endif /* defined(__Cheetah__Building__) */
