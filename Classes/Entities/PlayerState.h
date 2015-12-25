//
//  PlayerState.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#ifndef __Cheetah__PlayerState__
#define __Cheetah__PlayerState__

#include <cocos2d.h>
#include "../VOs/PlayerStateVO.h"

#include "CheckList.h"
#include "Building.h"
#include "Track.h"
#include "Pet.h"
#include "Ability.h"
#include "ChestGems.h"

#include "../VOs/HistoryItemVO.h"

#include "../VOs/PlayerWorldMapVillageVO.h"
#include "../VOs/PlayerHelperPetVO.h"
#include "../extensions/spine/Json.h"
#include "../VOs/PlayerPurchaseStatVO.h"

#define kHQBuilding     "hq"
#define kHostHabitat    "habitat"

#define kAbilityDashForward         "dash_forward"
#define kAbilityBananaTrap          "banana_trap"
#define kAbilityStomp               "stomp"
#define kAbilityShootForward        "shoot_forward"
#define kAbilityDoubleShoot         "double_shoot"

#define kAbilityIsUpgrading         "upgrading"

class PlayerState {
    
    bool                            m_didParseData;
    bool                            m_didMoveBuilding;
    bool                            m_didMoveDecoration;
    
    std::vector<IntKeyValue*>       m_items; //active & inactive items
    
    void _clearAllItems();
    void _clearItems();

    void sendPlayerLevelUpEvent(int newLevel);
    void sendPetLevelUpEvent(Pet* pet);

    bool _parseUserData(const char *json_str, bool opponent);
    
public:
    
    
    
    PlayerStateVO                       player;
    PlayerPurchaseStatVO                 PurchaseStat;
    std::map<std::string, bool>         Checklist;
    
    Building*                           HQBuilding;
    Building*                           HostHabitat;
    Pet*                                PetRunner;
    
    Building*                           Builder;
    std::vector<Building*>              Buildings;
    std::vector<Track*>                 Tracks;
    std::vector<Track*>                 Obstacles;
    std::vector<Decoration*>            Decorations;
    std::vector<Pet*>                   Pets;
    std::map<std::string, Ability*>     Abilities;
    std::map<std::string, ChestGems*>   ChestGemsDict;
    std::vector<IntKeyValue*>           Items;
    
    std::vector<HistoryItemVO>          HistoryItems;
    int                                 LastRacedVillageIndex;
    std::map<int, PlayerWorldMapVillageVO> WorldMapVillages;
    std::vector<PlayerHelperPetVO>      HelperPets;
    int                                 HelperPetsSelectedIndex;
    PlayerHelperPetVO                   HelperVo;
    
    int trackCount;
    
    PlayerInfoVO                    playerInfo;
    PlayerPetVO                     playerPet;
    float                           playerTime;
    PlayerInfoVO                    opponentInfo;
    PlayerPetVO                     opponentPet;
    float                           opponentTime;
    int                             coinsReward;
    std::string                     replayGuestRaw; //Player 
    std::string                     replayHostRaw; //Defendant
    std::string                     replayHelperPet;
    std::string                     itemDrops;
    std::string                     fbFriendsRawData;
    std::string                     rawData;
    
    int                             last_seen_mins_ago;
    
    static PlayerState *getInstance();
    static PlayerState *getPlayer();
    static PlayerState *getOpponent();
    static bool isCreated();
    
    PlayerState();
    ~PlayerState();

    void clear();
    void expectNewData();

    std::string getUserParam();
    std::string getGCID();
    std::string getDeviceToken();
    void setUUID(std::string newUUID);
    void setTOKEN(std::string newTOKEN);
    
#pragma mark - Balance
    
    bool deductGems(int gems);
    bool deductCoins(int coins);
    bool deductFood(int foods);
    
    ///////////////////////////////////////////////////////
#pragma mark - Building
    bool newBuilding(PlayerBuildingVO vo);
    bool addBuilding(Building *building);
    Building* getBuilder();
    void moveBuilding(Building* building);
    bool didMoveBuilding() { return m_didMoveBuilding; }
    std::string getChangedBuildingsJSON();
    bool gainExp(GDBuildingVO vo);
    
    ///////////////////////////////////////////////////////
    // Track
    bool addTrack(Track *track_);
    bool removeTrack(Track *track_);
    
    // Decoration
    bool addDecoration(Decoration *decoration_);
    bool removeDecoration(Decoration *decoration_);
    void moveDecoration(Decoration *decoration_);
    bool didMoveDecoration() { return m_didMoveDecoration; }
    std::string getChangedDecorationsJSON();
    
    void updateHelperPetsList();
    
    // Items
    std::vector<GDItemVO>                   itemsToAddToBarn;
    
    // Information for checking
    bool racedAtLeastOnce;
    
    // ChestGems
    bool addChestGems(PlayerChestGemsVO vo_);
    bool removeChestGems(ChestGems* chestgems_);
    
    // Achievement
    bool prePrecessAchievementClaim(GDAchievementsVO avo);
    
    //MerchantItem
    bool preBuyItemFromMerchant(std::string item_type);
    bool merchantHasItemForSell();
    bool merchantHasOnlyOneLastItemForSell();
    
    // Parse
    std::string uncompressData(const char *json_str);
//    bool loadUserDataFromLocal();

    bool parseCurrentTime(Json*json);
    bool parseUserRaceData(Json *root);
    bool parseUserData(const char *json_str_);
    bool parseOppoData(const char *json_str_);
    bool parseUserInfo(Json *_root);
    void determineUser(Json *_root, bool opponent);
    bool parseUserProfile(Json *_root);
    bool parseUserState(Json * json);
    bool parseUserBalance(Json *_root);
    bool parseTrackData(Json *_root);
    bool parseBuildingData(Json *_root);
    bool parseDecorationData(Json *_root);
    bool parsePetData(Json *_root);
    bool parseAbilityData(Json *_root);
    bool parseChestGemsData(Json *_root);
    bool parseItemData(Json *_root);
    bool parseHistoryItemData(Json *_root);
    bool parseSellItemResult(Json *_root);
    bool parseWorldMapVillages(Json *_root);
    bool parseRaceReplay(const char*json);
    void parseReplayData(Json*json);
    bool parseChecklistData(Json *_root);
    
    static void onMapDataRetrieved(void* sender, std::string resp_str);
    
    // Action
    void removePetData(Json*json);
    void removeDecorationData(Json*json);
    
    void updateBuildingsTimer(float delta_);
    void updateItemData();
    
    std::string getTrackData();
    bool saveUserDataToLocal();
    bool didParseData();
    
    void checklist(std::string checkkey_);
    static void onChecklistUpdated(void* sender, std::string json_str);

    // Data
    
    bool isOnline();
    bool isBot();
    bool isFacebookLoggedIn();
    
    bool isBeingRaced();
    
    int getSecondsSinceLastBeingRaced();
    int getSecondsLeftToPlayAgain();
    
    int getNBuilding(std::string type_);
    int getNBuildingWithPets(std::string type_);
    Building *getBuilding(std::string buid_);
    Building *getBuilding(std::string type_, int index_);
    int getSumOfAllResourcesProducedByAllBuildingsOfThisType(std::string type_);
    Building *getAvailableHabitat();
    Building *getAvailableTrainingGym();
    void calAllBuildingCountdown();
    Track *getTrackAtIndex(int index_);
    Track *getTrackToStart(int index_);
    Track *getObstacleAtIndex(int index_);
    Pet *getPet(std::string puid_);
    Pet *getPetWithHighestHorsePower();
    int getHorsePowerFromPet(Pet *pet);
    int getNPet();
    int getNHomelessPet();
    int getNItem(std::string type_);
    int getNItems();
    int getNTree();
    bool isItemFull() { return getNItems() >= getTotalItemStorageCapacity(); }
    
    int getNDecoration(std::string type);
    int getNKeptDecoration(std::string type);
    Decoration* getKeptDecoration(std::string type);
    bool isDecorationMaximumReached(std::string type);
    
//    int getNUnclaimedHistoryItem();
    bool isThisANewHistoryItem(std::string race_id);
    int getNewHistoryBadgeNumber();
    void clearNewHistoryBadgeNumber();
    
    void checkAndPrintIfItemBarnIsFull();
    bool checkAndReturnIfItemBarnIsFull();

    Ability* getUpgradingAbility();
    Ability* getUpgradingAbility(int index_);

    void calculateTrackpointsUsed();
    
    void addGold(int amount_);
    void addFood(int amount_);
    
    int getTotalCoinStorageCapacity();
    int getTotalFoodStorageCapacity();
    int getTotalItemStorageCapacity();
    int getMaxCoinCanCollect(int amount_);
    int getMaxFoodCanCollect(int amount_);
  
    int getVisitorsPerHour();
    int getFoodsProductionPerHour();
    
    Ability* getAbilityForKey(std::string key);
    bool hasAbilityForKey(std::string key);
    
    
    void cleanAllEffects();
};

#endif /* defined(__Cheetah__PlayerState__) */
