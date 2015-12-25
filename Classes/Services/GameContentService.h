//
//  GameContentService.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#ifndef __Cheetah__GameContentService__
#define __Cheetah__GameContentService__

#include "cocos2d.h"

#include "../VOs/GDExperienceLevelVO.h"
#include "../VOs/GDPetVO.h"
#include "../VOs/GDBuildingVO.h"
#include "../VOs/GDTrackVO.h"
#include "../VOs/GDDecorationVO.h"
#include "../VOs/PlayerDecorationVO.h"

#include "../VOs/GDHQUpgradeVO.h"
#include "../VOs/GDAbilityVO.h"
#include "../VOs/GDItemVO.h"
#include "../VOs/GDInitialItemVO.h"

#include "../VOs/GDGameStartBarVO.h"
#include "../VOs/GDMapVillageReturnVO.h"
#include "../VOs/GDGemExchangeRateVO.h"
#include "../VOs/SettingVO.h"

#include "../VOs/GDCloudGroupVO.h"
#include "../VOs/GDAchievementsVO.h"
#include "../VOs/GDIAPVO.h"

#include "../Entities/IsoMap.h"
#define kGameDataKey    "gamedata"
#define kGameDataUid    "gamedata_uid"

#define N_TILE_PACK 2
#define N_TILE_PLAY (ISO_TILE_SIZE/3)*(ISO_TILE_SIZE/3)
#define N_ZONE (ISO_TILE_SIZE/3/4)*(ISO_TILE_SIZE/3/4)

#include "../SceneManager.h"

#include "../Entities/Building.h"
#include "../Entities/Pet.h"
#include "../Entities/Ability.h"

#define kGameContentServiceDir  "gamedata/"

#define kGameContentServiceLiveGameConfig "live_game_config"

#define GAMEDATA_LIST(__LIST__) \
__LIST__.push_back("word"); \
__LIST__.push_back("game_config"); \
__LIST__.push_back("hints"); \
__LIST__.push_back("experience_level"); \
__LIST__.push_back("pet"); \
__LIST__.push_back("feed_times"); \
__LIST__.push_back("abilities"); \
__LIST__.push_back("game_start_bar"); \
__LIST__.push_back("initial_pet"); \
__LIST__.push_back("cloud"); \
__LIST__.push_back("item"); \
__LIST__.push_back("initial_item"); \
__LIST__.push_back("building"); \
__LIST__.push_back("track"); \
__LIST__.push_back("decoration"); \
__LIST__.push_back("building_limit_number"); \
__LIST__.push_back("iap_product"); \
__LIST__.push_back("achievements"); \
__LIST__.push_back("map_village_return"); \
__LIST__.push_back("gem_exchange_rate");

class GameContentService {
    
    void _clearAll();
    
//    bool _loadGameDataFromCache();
    bool m_didParse;
    
    int _calcGemPrice(int amount_, std::string type_);
    
public:
    
    std::string                                 unique_key;
    
    std::map<std::string, float>                GameConfig;
    std::map<std::string, float>                LiveGameConfig;
    std::map<std::string, std::string>          Word;
    
    std::vector<GDAchievementsVO>               Achievement;
    std::vector<GDIAPVO>                        Inapp;
    
    std::vector<GDCloudGroupVO>                 Cloud;
    std::vector<GDExperienceLevelVO>            ExperienceLevel;
    std::vector<GDPetVO>                        Pet;
    std::map<int, float>                        FeedTimes;
    
    std::map<std::string, std::vector<GDAbilityVO> > Abilites;
    std::vector<std::string>                    AbilitiesOrder;
    
    std::vector<std::string>                    InitialPet;
    std::vector<GDBuildingVO>                   Building;
    std::vector<GDTrackVO>                      Track;
    std::vector<GDDecorationVO>                 Decoration;
    std::vector<GDHQUpgradeVO>                  HQUpgrade;
    std::vector<std::map<std::string, int> >    BuildingLimitNumber;
    std::vector<GDItemVO>                       Item;
    std::vector<GDInitialItemVO>                InitialItem;
    std::vector<GDGameStartBarVO>               GameStartBar;
    std::vector<GDMapVillageReturnVO>           MapVillageReturn;
    std::vector<GDGemExchangeRateVO>            GemExchangeRate;
    
    std::string                                 gameDataBuffer;
    std::string                                 backendURL;
    
    cocos2d::CCSize                             tileSize;
    cocos2d::CCPoint                            tileShift;
    
    cocos2d::CCDictionary*                      userRecord;
    cocos2d::CCDictionary*                      playerRecord;
    cocos2d::CCDictionary*                      opponentRecord;
    
    static GameContentService* getInstance();
    static void destroyInstance();
    
    GameContentService();
    ~GameContentService();
    
    bool loadGameDataFromLocal(const char* type);
    bool loadGameDataFromLocal();
    
    void pullLiveGameConfig();
    static void onLiveGameConfigRetrieved(void* sender, std::string resp_str);
    
    float getPercentageFinished();
    
    void removeAssetContent(SceneList previousScene_);
    void loadAssetContent(SceneList newScene_);
    std::string correctFileNameWithPath(const char* pszFileName);
    static std::string uncompress(std::string compressedBase64EncodedStr_);
    
    bool parseGameData(const char *json_);
    bool parseGameData(const char *json_, bool from_local_);
    
    GDAchievementsVO getAchievementVO(std::string type);
    GDExperienceLevelVO getNextExperienceLevelVO(int level_);
    GDPetVO         getPetVO(std::string type_);
    GDBuildingVO    getBuildingVO(std::string type_, int level_);
    int             getBuildingMaxLevel(std::string type_);
    GDTrackVO       getTrackVO(std::string type_);
    GDDecorationVO  getDecorationVO(std::string type_);
    int             getNDecorationVOUnlocksAtLevel(int level_);
    int             getNDecorationVOJustUnlocksAtLevel(int level_);
    int             getBuildingMax(std::string type_, int level_);
    GDItemVO        getItemVO(std::string type_);
    /** operator: '>' '<' '>=' '<=' '!=' '==' */
    std::string     getRandomItemExcept(const char *operator_,
                                        int rarity_,
                                        std::vector<std::string> except_items_);
    GDInitialItemVO getInitialItemVO(std::string type_);
    int             getNTrackVOUnlocksAtLevel(int level_);
    GDTrackVO       getTrackVOUnlocksAtLevel(int level_);
    GDTrackVO       getTrackVOUnlocksAtLevel(int level_, int index_);
    int             getNAbilitiesUnlocksAtLevel(int level_, bool group_);
    float           getMapVillageReturnPercentage(int number_of_village_);
    GDAbilityVO     getAbilityVO(std::string name_of_ability_, int level_);
    
    void getUnlockVillTable(int *unlockVillTable);
    
    int get_gemcal_for_coins(int coins_);
    int get_gemcal_for_foods(int foods_);
    int get_gemcal_for_building_finishnow(int seconds_);
    int get_gemcal_for_ability_finishnow(int seconds_);
    int get_gemcal_for_pet_wakeup(int seconds_);
    int get_gemcal_for_pet_sell(int sell_value, int level);
    
    int cal_instant_building_cost(GDBuildingVO bvo_);
    int cal_coins_reward(int garage_level,
                         int coins_reward_,
                         int win_streak_);
    int cal_fans_won(bool oppo_is_bot_);
    int cal_round_to_nearest_5(float input);
    float cal_round_to_nearest_half(float input);
    void cal_helper_horsepower_to_stun_time(float horsepower,
                                            float *timeTotal,
                                            float *times,
                                            float *timeEach);
    
    bool didParse() { return m_didParse; }
    bool loadSetting();
    
};

#endif /* defined(__Cheetah__GameContentService__) */
