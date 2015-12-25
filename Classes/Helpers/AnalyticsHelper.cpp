//
//  AnalyticsHelper.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 10/29/2557 BE.
//
//

#include "AnalyticsHelper.h"

#include "../Entities/PlayerState.h"
extern PlayerState *g_playerState;

#include "../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

USING_NS_CC;

extern bool hasPurchasedDuringThisSession;
extern const char* purchasedWithProductIdentifier;

CSJson::Value AnalyticsHelper::getPlayerInfo()
{
    CSJson::Value root;
    
    root["ranchLevel"] = g_playerState->player.racing.level;
    root["winStreak"] = g_playerState->player.info.win_streak;
    root["highestWinStreak"] = g_playerState->player.info.highest_win_streak;
    root["totalRaces"] = g_playerState->player.info.total_races;
    root["totalWins"] = g_playerState->player.info.total_wins;
    root["gem"] = g_playerState->player.balance.gem;
    root["coin"] = g_playerState->player.balance.gold;
    
    int coinsFromAllHabitats = PlayerState::getInstance()->getSumOfAllResourcesProducedByAllBuildingsOfThisType(kBuildingHabitat);
    int coinsFromAllBooths = PlayerState::getInstance()->getSumOfAllResourcesProducedByAllBuildingsOfThisType(kBuildingTicket);
    
    root["uncollectedCoins"] = coinsFromAllHabitats + coinsFromAllBooths; //uncollected coins
    
    int foodFromFeedMills = PlayerState::getInstance()->getSumOfAllResourcesProducedByAllBuildingsOfThisType(kBuildingFoodProduction);
    
    root["uncollectedFood"] = foodFromFeedMills; //uncollected food
    
    root["food"] = g_playerState->player.balance.food;
    root["fan"] = g_playerState->player.balance.fan;
    root["maxCoins"] = g_playerState->getTotalCoinStorageCapacity();
    root["maxFood"] = g_playerState->getTotalFoodStorageCapacity();
    
    return root;
}

CSJson::Value AnalyticsHelper::getPetsInfo()
{
    Pet* bestPet = g_playerState->getPetWithHighestHorsePower();
    Pet* leaderPet = g_playerState->getPet(g_playerState->player.racing.host);
    
    CSJson::Value root;
    
    if (bestPet == NULL || leaderPet == NULL)
        return root;
    
    int bestPetIsLeader = (int)(bestPet == leaderPet);
    
    root["bestPetIsLeader"] = bestPetIsLeader;
    root["bestPetHP"] = bestPet->getHorsepower();
    root["leaderHP"] = leaderPet->getHorsepower();
    
    int petCount = 0;
    int penPetCount = 0;
    float allPetsHorsePower = 0.0f;
    float allPetsCoinsProducedPerMin = 0.0f;
    float penPetsCoinsProducedPerMin = 0.0f;
    int highestRarityStars = 0;
    
    int penPetHungry = 0;
    int penPetSleeping = 0;
    
    for (std::vector<Pet*>::iterator it=g_playerState->Pets.begin(); it!=g_playerState->Pets.end(); ++it) {
        Pet *_pet = (Pet*)(*it);
        
        if (_pet == NULL)
            continue;
        
        petCount++;
        allPetsHorsePower += (float)_pet->getHorsepower();
        allPetsCoinsProducedPerMin += _pet->getCoinsProducedPerSecond();
        
        highestRarityStars = MAX(highestRarityStars, _pet->pVo.rarity_stars);
        
        if (_pet->isInAnimalPen()) {
            penPetCount++;
            penPetsCoinsProducedPerMin += _pet->getCoinsProducedPerSecond();
            
            if (_pet->isHungry()) {
                penPetHungry++;
            }
            if (_pet->isSleeping()) {
                penPetSleeping++;
            }
        }
    }
    
    int averageHorsePower = (int)(allPetsHorsePower/(float)petCount);

    root["averageHP"] = averageHorsePower;
    root["highestRarity"] = highestRarityStars;
    root["petCount"] = petCount;
    root["penPetCount"] = penPetCount;
    root["penPetHungry"] = penPetHungry;
    root["penPetSleeping"] = penPetSleeping;

    root["BestPetCoinsRate"] = (int)roundf(bestPet->getCoinsProducedPerSecond()*60.0f);
    root["leaderCoinsRate"] = (int)roundf(leaderPet->getCoinsProducedPerSecond()*60.0f);
    
    int averageCoinsProducedPerMinOfAllPets = (int)roundf((allPetsCoinsProducedPerMin/(float)petCount)*60.0f);
    int averageCoinsProducedPerMinOfPetsInPen = (int)roundf((penPetsCoinsProducedPerMin/(float)penPetCount)*60.0f);
    
    root["averageCoinsRateAllPets"] = averageCoinsProducedPerMinOfAllPets;
    root["averageCoinsRatePenPets"] = averageCoinsProducedPerMinOfPetsInPen;
    root["totalCoinsRateOfPenPets"] = (int)roundf(penPetsCoinsProducedPerMin*60.0f);
    
    return root;
}

CSJson::Value AnalyticsHelper::getBuildingsInfo()
{
    CSJson::Value root;
    
    Building* mansion = g_playerState->getBuilding(kBuildingHQ, 0);
    Building* library = g_playerState->getBuilding(kBuildingLibrary, 0);
    Building* garage = g_playerState->getBuilding(kBuildingGarage, 0);
    Building* market = g_playerState->getBuilding(kBuildingFusion, 0);
    Building* raceassoc = g_playerState->getBuilding(kBuildingAssociation, 0);
    Building* itembarn = g_playerState->getBuilding(kBuildingItem, 0);
    Building* animalbarn = g_playerState->getBuilding(kBuildingAnimal, 0);
    Building* ticketbooth = g_playerState->getBuilding(kBuildingTicket, 0);
    
    
    int habitatCount = 0;
    
    int feedMillCount = 0;
    int highestFeedMillLevel = 0;
    int totalFeedMillLevel = 0;
    
    int feedSiloCount = 0;
    int highestFeedSiloLevel = 0;
    int totalFeedSiloLevel = 0;
    
    int bankCount = 0;
    int highestBankLevel = 0;
    int totalBankLevel = 0;
    
    bool buildingIsBeingUpgradedOrUnderContruction = false;
    
    for (std::vector<Building*>::iterator it= g_playerState->Buildings.begin(); it!= g_playerState->Buildings.end(); ++it) {
        
        Building *_building = (Building*)(*it);
        
        if (_building == NULL)
            continue;
        
        if (_building->pbVo.status == BUILDING_STATUS_UPGRADE ||
            _building->pbVo.status == BUILDING_STATUS_CONSTRUCT) {
            buildingIsBeingUpgradedOrUnderContruction = true;
        }
        
        if (_building->pbVo.type == kBuildingHabitat ||
            _building->pbVo.type == kBuildingHabitatPremium) {
            habitatCount++;
        }
        
        if (_building->pbVo.type == kBuildingFoodProduction) {
            feedMillCount++;
            highestFeedMillLevel = MAX(highestFeedMillLevel, _building->pbVo.level);
            totalFeedMillLevel += _building->pbVo.level;
        }
        
        if (_building->pbVo.type == kBuildingFoodStorage) {
            feedSiloCount++;
            highestFeedSiloLevel = MAX(highestFeedSiloLevel, _building->pbVo.level);
            totalFeedSiloLevel += _building->pbVo.level;
        }
        
        if (_building->pbVo.type == kBuildingBank) {
            bankCount++;
            highestBankLevel = MAX(highestBankLevel, _building->pbVo.level);
            totalBankLevel += _building->pbVo.level;
        }
    }
    int averageFeedMillLevel = (feedMillCount == 0 ? 0 : (int)roundf((float)totalFeedMillLevel/(float)feedMillCount));
    int averageFeedSiloLevel = (feedSiloCount == 0 ? 0 : (int)roundf((float)totalFeedSiloLevel/(float)feedSiloCount));
    int averageBankLevel =     (bankCount     == 0 ? 0 : (int)roundf((float)totalBankLevel/(float)bankCount));
    
    root["mansionLevel"] = (mansion ? mansion->pbVo.level : 0);
    root["libraryLevel"] = (library ? library->pbVo.level : 0);
    root["garageLevel"] = (garage ? garage->pbVo.level : 0);
    root["marketLevel"] = (market ? market->pbVo.level : 0);
    root["raceAssocLevel"] = (raceassoc ? raceassoc->pbVo.level : 0);
    root["itemBarnLevel"] = (itembarn ? itembarn->pbVo.level : 0);
    root["animalBarnLevel"] = (animalbarn ? animalbarn->pbVo.level : 0);
    root["ticketBoothLevel"] = (ticketbooth ? ticketbooth->pbVo.level : 0);
    
    root["penCount"] = habitatCount;
    root["feedMillCount"] = feedMillCount;
    root["feedMillHighestLevel"] = highestFeedMillLevel;
    root["feedMillAverageLevel"] = averageFeedMillLevel;
    root["feedSiloCount"] = feedSiloCount;
    root["feedSiloHighestLevel"] = highestFeedSiloLevel;
    root["feedSiloAverageLevel"] = averageFeedSiloLevel;
    root["bankCount"] = bankCount;
    root["bankHighestLevel"] = highestBankLevel;
    root["bankAverageLevel"] = averageBankLevel;
    root["buildingIsBeingWorked"] = (int)buildingIsBeingUpgradedOrUnderContruction;
    
    return root;
}

CSJson::Value AnalyticsHelper::getAbilitiesInfo()
{
    CSJson::Value root;
    
    int dashForwardLevel = 0;
    int bananaTrapLevel = 0;
    int stompLevel = 0;
    int shootForwardLevel = 0;
    int DoubleShootLevel = 0;
    
    bool abilityIsBeingUpgraded = false;
    
    Ability *dashForward = g_playerState->getAbilityForKey(kAbilityDashForward);
    if (dashForward) {
        dashForwardLevel = dashForward->paVo.level;
        
        if (dashForward->paVo.status == kAbilityIsUpgrading)
            abilityIsBeingUpgraded = true;
    }
    
    Ability *bananaTrap = g_playerState->getAbilityForKey(kAbilityBananaTrap);
    if (bananaTrap) {
        bananaTrapLevel = bananaTrap->paVo.level;
        
        if (bananaTrap->paVo.status == kAbilityIsUpgrading)
            abilityIsBeingUpgraded = true;
    }
    
    Ability *stomp = g_playerState->getAbilityForKey(kAbilityStomp);
    if (stomp) {
        stompLevel = stomp->paVo.level;
        
        if (stomp->paVo.status == kAbilityIsUpgrading)
            abilityIsBeingUpgraded = true;
    }
    
    Ability *shootForward = g_playerState->getAbilityForKey(kAbilityShootForward);
    if (shootForward) {
        shootForwardLevel = shootForward->paVo.level;
        
        if (shootForward->paVo.status == kAbilityIsUpgrading)
            abilityIsBeingUpgraded = true;
    }
    
    Ability *doubleShoot = g_playerState->getAbilityForKey(kAbilityDoubleShoot);
    if (doubleShoot) {
        DoubleShootLevel = doubleShoot->paVo.level;
        
        if (doubleShoot->paVo.status == kAbilityIsUpgrading)
            abilityIsBeingUpgraded = true;
    }
    
    root["dashLevel"] = dashForwardLevel;
    root["trapLevel"] = bananaTrapLevel;
    root["stompLevel"] = stompLevel;
    root["shootLevel"] = shootForwardLevel;
    root["doubleShootLevel"] = DoubleShootLevel;
    root["abilityIsBeingWorked"] = (int)abilityIsBeingUpgraded;

    return root;
}

CSJson::Value AnalyticsHelper::getSessionPurchasedInfo()
{
    CSJson::Value root;
    root["purchased"] = (int)hasPurchasedDuringThisSession;
    root["productID"] = purchasedWithProductIdentifier;
    
    return root;
}

double AnalyticsHelper::convertFloatTo1DecimalDouble(float number)
{
    CREATE_CHAR_BUFFER(_buf, 16);
    
    snprintf(_buf, _buf_size, "%.1f", (roundf(number * 10.0f) * 0.1f));
    
    return atof(_buf);
}

double AnalyticsHelper::convertFloatTo2DecimalDouble(float number)
{
    CREATE_CHAR_BUFFER(_buf, 16);
    
    snprintf(_buf, _buf_size, "%.2f", (roundf(number * 100.0f) * 0.01f));
    
    return atof(_buf);
}


