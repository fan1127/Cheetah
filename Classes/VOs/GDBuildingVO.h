//
//  GDBuildingVO.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 9/12/56 BE.
//
//

#ifndef __Cheetah__GDBuildingVO__
#define __Cheetah__GDBuildingVO__

#include <iostream>

#define BUILDING_SPACE_COL_MAX  6
#define BUILDING_SPACE_ROW_MAX  6
#define BUILDING_SPACE_GRID_NUM BUILDING_SPACE_COL_MAX*BUILDING_SPACE_ROW_MAX

#define BUILDING_STATUS_CONSTRUCT   "construct"
#define BUILDING_STATUS_COMPLETE    "complete"
#define BUILDING_STATUS_OPEN        "open"
#define BUILDING_STATUS_DELIVERED   "delivered"
#define BUILDING_STATUS_PREVIEW     "preview"
#define BUILDING_STATUS_UPGRADE     "upgrade"

#define BUILDING_TYPE_GENERAL       "general"
#define BUILDING_TYPE_HQ            "hq"
#define BUILDING_TYPE_PRODUCER      "producer"
#define BUILDING_TYPE_FOOD_STORAGE  "food_storage"
#define BUILDING_TYPE_HABITAT       "habitat"
#define BUILDING_TYPE_COIN_STORAGE  "coin_storage"
#define BUILDING_TYPE_ACHIEVEMENT   "achievement"
#define BUILDING_TYPE_ANIMALDORM    "animaldorm"
#define BUILDING_TYPE_SKILLS        "skills_upgrader"
#define BUILDING_TYPE_FUSION        "fusion"
#define BUILDING_TYPE_INVENTORY     "inventory"
#define BUILDING_TYPE_ASSOCIATION   "association"
#define BUILDING_TYPE_HISTORY       "history"
#define BUILDING_TYPE_TRAINING      "training"
#define BUILDING_TYPE_TICKETBOOTH   "ticket"

#define kBuildingCommonAniIdle      "idle"
// 0
#define kBuildingConstruction                   "building_construction"
#define kBuildingConstructionSrc                "building_construction"
#define kBuildingConstructionAniIdle            "idle"
#define kBuildingConstructionAniFinish          "finish"
#define kBuildingConstructionAniOpen            "open"

//
#define kBuildingUpgrade                        "building_upgrade"
#define kBuildingUpgradeSrc                     "upgrade_4x4"
#define kBuildingUpgradeAniIdle                 "idle"

#define kBuildingDummy              "dummy"
//1
#define kBuildingHQ                 "hq"
#define kBuildingUncleHouse         "uncle_house"

#define kBuildingFoodProduction                 "food_production"
#define kBuildingFoodProductionAniDepleted      "depleted"
#define kBuildingFoodProductionHQAniFull        "full"
#define kBuildingFoodProductionAniMaking        "making"

#define kBuildingFoodStorage        "food_storage"
#define kBuildingFoodStorageAniIdle "idle"

//2
#define kBuildingTicket             "ticket_booth"
#define kBuildingTicketAniIdle      "idle"

#define kBuildingBank               "bank"
#define kBuildingBangAniIdle        "idle"

#define kBuildingGate               "gate_lion"
#define kBuildingGateAniOpen        "idle"
#define kBuildingGateAniClose       "close"
//3
#define kBuildingItem               "item_storage"
#define kBuildingItemAniIdle        "idle"
//4
#define kBuildingAssociation        "pet_race_association"

#define kBuildingTraining           "training_gym"
#define kBuildingTrainingAniIdle    "idle"
#define kBuildingTrainingAniRunning "runing"

#define kBuildingGarage             "garage"
#define kBuildingLibrary            "library"

//5
#define kBuildingFusion             "fusion_tower"
#define kBuildingFusionAniIdle      "idle"
#define kBuiidingFusionAniFusion    "fusion"
//6
#define kBuildingHabitat            "habitat"
#define kBuildingHabitatPremium     "habitat_premium"
#define kBuildingHabitatAniIdle     "idle"
#define kBuildingHabitatAniFlag     "flag"
//7
#define kBuildingAnimal             "animal_storage"
#define kBuildingAnimalAniIdle      "idle"
//8
#define kBuildingHistory            "race_history_statue"
#define kBuildingHistoryAniIdle     "idle"
//9
#define kBuildingAchievement        "building_achievement"
#define kBuildingAchievementAniIdle "idle"

#define kBuildingStackNormal        "normal"
#define kBuildingStackGround        "ground"
#define kBuildingStackOnTop         "ontop"

struct GDBuildingVO {
    
    std::string     type;
    int             level;
    std::string     building_type;
    std::string     title;
    std::string     description;
    unsigned int    cost_coins;
    unsigned int    cost_gems;
    bool            cost_multiplier;
    unsigned int    time;
    bool            in_shop;
    unsigned int    grid_col;
    unsigned int    grid_row;
    bool            movable;
    std::string     stack;
    std::string     src;
    unsigned int    src_width;
    unsigned int    src_height;
    unsigned int    unlocks_at_level;
    unsigned int    village_exp_gain;
    float           param_1;
    float           param_2;
    float           param_3;
    float           param_4;
    float           param_5;
    float           param_6;
    
    GDBuildingVO();
    
};

#endif /* defined(__Cheetah__GDBuildingVO__) */
