//
//  PlayerState.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/30/56 BE.
//
//

#include "PlayerState.h"
USING_NS_CC;
using namespace std;

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

extern string uuid, token, gcid, device_token, auuid, race_id;
extern unsigned int currentTime;
extern double currentTimeClient;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIChecklistUpdate.h"

#include "../Extensions/CCAlert/CCAlert.h"

#include "../Scenes/SceneVille.h"
extern CCScene *g_currentScene;

#include "../Helpers/StringHelper.h"
#include "../Helpers/base64.h"

#include "../Settings.h"
#import "../Helpers/FlurryCpp.h"

#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/AnalyticsClientWrapper.h"
#include "../Helpers/CustomEventsHelper.h"
#include "../Helpers/AppRating/AppRating.h"

#include "../Helpers/FBAppEventsWrapper.h"
#include "../Helpers/AppsFlyerWrapper.h"

#include <iostream>
#include <cstring>
#include <string>

PlayerState *g_playerState = NULL;
PlayerState *g_opponentState = NULL;

PlayerState *PlayerState::getInstance() {
   return PlayerState::getPlayer();
}

PlayerState *PlayerState::getPlayer() {
    if (!g_playerState) {
        g_playerState = new PlayerState();
    }
    return g_playerState;
}

PlayerState *PlayerState::getOpponent() {
    if (!g_opponentState) {
        g_opponentState = new PlayerState();
    }
    return g_opponentState;
}

bool PlayerState::isCreated() {
    return g_playerState != NULL;
}

PlayerState::PlayerState() {
  
    fbFriendsRawData = "";
    m_didParseData = false;
    
    m_didMoveBuilding = false;
    rawData = "";
    itemsToAddToBarn.clear();
    
    racedAtLeastOnce = false;
    
    clear();
    
}

PlayerState::~PlayerState() {
    
    clear();
    
}

void PlayerState::_clearAllItems() {
 
    for (std::vector<IntKeyValue*>::iterator it=m_items.begin(); it != m_items.end(); ++it) {
        if (IntKeyValue *_item = (IntKeyValue*)(*it)) {
            delete _item;
            _item = NULL;
        }
    }
    m_items.clear();
    
}

void PlayerState::_clearItems() {
    
    for (std::vector<IntKeyValue*>::iterator it=Items.begin(); it != Items.end(); ++it) {
        if (IntKeyValue *_item = (IntKeyValue*)(*it)) {
            delete _item;
            _item = NULL;
        }
    }
    Items.clear();
    
}

void PlayerState::clear() {
    
    player = PlayerStateVO();
    
    _clearAllItems();
    _clearItems();
    
    HelperPetsSelectedIndex = -1;
    HelperPets.clear();
    LastRacedVillageIndex = -1;
    WorldMapVillages.clear();
    
    for (std::map<std::string, Ability*>::iterator it=Abilities.begin(); it!=Abilities.end(); ++it) {
        if (Ability* _ability = it->second) {
            _ability = NULL;
        }
    }
    Abilities.clear();
    
    for (std::vector<Pet*>::iterator it=Pets.begin(); it != Pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            _pet->removeFromParentAndCleanup(true);
            _pet = NULL;
        }
    }
    Pets.clear();
    
    for (std::vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
        if (Track* _obstacle = (Track*)(*it)) {
            _obstacle->removeFromParentAndCleanup(true);
            _obstacle = NULL;
        }
    }
    Obstacles.clear();
    
    for (std::vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        if (Track* _track = (Track*)(*it)) {
            _track->removeFromParentAndCleanup(true);
            _track = NULL;
        }
    }
    Tracks.clear();
    
    for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
        if (Decoration* _decoration = (Decoration*)(*it)) {
            _decoration->removeFromParentAndCleanup(true);
            _decoration = NULL;
        }
    }
    Decorations.clear();
    
    for (std::vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            _building->removeFromParentAndCleanup(true);
            _building = NULL;
        }
    }
    Buildings.clear();
    
    Checklist.clear();
    
    Builder = NULL;
    HostHabitat = NULL;
    HQBuilding = NULL;
    PetRunner = NULL;
    last_seen_mins_ago = -1;
    
    
    m_didParseData = false;
//    m_didAlert = false;
    
}

void PlayerState::expectNewData() {
    
    m_didParseData = false;

}

string PlayerState::getUserParam() {
    
    if (uuid == "" || token == "") {
        return "";
    }
    
    return "uuid=" + uuid + "&token=" + token;
    
}

string PlayerState::getGCID() {
    
    return gcid;
    
}

string PlayerState::getDeviceToken() {
    
    return device_token;
    
}

void PlayerState::setUUID(std::string newUUID) {

	player.uuid = newUUID;
	uuid = newUUID;
	CCUserDefault::sharedUserDefault()->setStringForKey("uuid", newUUID);
}

#pragma mark - Balance

bool PlayerState::deductGems(int gems) {
    
    if (gems <= 0) return false;
    
    player.balance.gem -= gems;
    if (player.balance.gem < 0) {
        player.balance.gem = 0;
    }
    
    return true;
    
}

bool PlayerState::deductCoins(int coins) {
    
    if (coins <= 0) return false;
    
    player.balance.gold -= coins;
    if (player.balance.gold < 0) {
        player.balance.gold = 0;
    }
    
    return true;
    
}

bool PlayerState::deductFood(int foods) {
    
    if (foods <= 0) return false;
    
    player.balance.food -= foods;
    if (player.balance.gold < 0) {
        player.balance.gold = 0;
    }
    
    return true;
    
}

#pragma mark - User

bool PlayerState::parseCurrentTime(Json* _root) {
    
    if (!_root) {
        return false;
    }

    CREATE_CHAR_BUFFER(_buf, 64);
    snprintf(_buf, _buf_size, "%d", currentTime);
    currentTime = (unsigned int)atol(Json_getString(_root, "currentTime", _buf));
    
    currentTimeClient = (double)currentTime;
    
    return true;
    
}

bool PlayerState::parseUserInfo(Json *_root) {
    
    if (!_root) {
        return false;
    }

    std::string playeravid = "something";
    
    player.uuid = Json_getString(_root, "uuid", player.uuid.c_str());
    CCLog("%s", player.uuid.c_str());
    player.fbid = Json_getString(_root, "fbid", player.fbid.c_str());
    //player.avid = Json_getString(_root, "avid", player.avid.c_str());
    player.avid = playeravid;
    player.gcid = Json_getString(_root, "gcid", player.gcid.c_str());
    player.token = Json_getString(_root, "access_token", player.token.c_str());
    
    last_seen_mins_ago = Json_getInt(_root, "last_seen_mins_ago", last_seen_mins_ago);
    
    return true;
    
}

void PlayerState::determineUser(Json* _root, bool opponent) {
    
    if (player.token != "" && !player.info.bot && !opponent) {
        
        // New user account, should logout Facebook
        if (uuid != player.uuid) {
            g_sceneManager->facebook->logout(NULL, NULL);
            CustomEventsHelper::Instance().resetAllSettings();
        }
        
        token = player.token;
        CCUserDefault::sharedUserDefault()->setStringForKey("token", token);
        uuid = player.uuid;
        CCUserDefault::sharedUserDefault()->setStringForKey("uuid", uuid);
        
#if IS_DEBUG_BUILD
        printf("......................\n");
        printf("%s\n", uuid.c_str());
        printf(".........uuid.........\n");
        printf("uuid=\"%s\";\n", uuid.c_str());
        printf("token=\"%s\";\n", token.c_str());
        printf("......................\n");
#endif
        
        itemDrops = "";
        coinsReward = 0;
        
    }else {
        
        if (!_root) {
            return;
        }
        
        auuid = player.uuid;
        if (Json_getItem(_root, "race_id")) race_id = Json_getString(_root, "race_id", NULL);
        coinsReward = Json_getInt(_root, "coins_reward", 0);
        CCLog_("Coins Reward: %d", coinsReward);
        if (Json_getItem(_root, "item_drops")) {
            itemDrops = Json_getString(_root, "item_drops", NULL);
            itemDrops = StringHelper::trim(itemDrops);
        }
        CCLog_("Item Drops: %s", itemDrops.c_str());
    }

}

bool PlayerState::parseUserProfile(Json* _root) {
    
    if (!_root) {
        return false;
    }

    // Parse User Info
    Json *_info = Json_getItem(_root, "info");
    if (!_info) {
        return false;
    }
    
    player.info.name = Json_getString(_info, "name", player.info.name.c_str());
    player.info.photo = Json_getString(_info, "photo", player.info.photo.c_str());
    player.info.email = Json_getString(_info, "email", player.info.email.c_str());
    
    return true;
    
}

bool PlayerState::parseUserState(Json * _root) {
    
    if (!_root) {
        return false;
    }
    
    Json* _state = Json_getItem(_root, "state");
    if (!_state) {
        return false;
    }
    
    CREATE_CHAR_BUFFER(_buf, 32);
    if (Json_getItem(_state, "free_gacha")) {
        player.info.free_gacha = Json_getInt(_state, "free_gacha", player.info.free_gacha);
    }else {
        //Deprecated _free_gacha no longer used in cassandra db version
        player.info.free_gacha = Json_getInt(_state, "_free_gacha", player.info.free_gacha);
    }
    
    snprintf(_buf, _buf_size, "%d", player.info.last_being_attacked);
    player.info.last_being_attacked = (unsigned int)StringHelper::toInt(Json_getString(_state, "last_being_attacked", _buf));
    
    snprintf(_buf, _buf_size, "%d", player.info.merchant_timestamp);
    player.info.merchant_timestamp = (unsigned int)StringHelper::toInt(Json_getString(_state, "merchant_timestamp", _buf));
    
    player.info.merchant_items = Json_getString(_state, "merchant_items", player.info.merchant_items.c_str());
    player.info.win_streak = Json_getInt(_state, "win_streak", player.info.win_streak);
    player.info.highest_win_streak = Json_getInt(_state, "highest_win_streak", player.info.highest_win_streak);
    player.info.total_races = Json_getInt(_state, "total_races", player.info.total_races);
    player.info.total_wins = Json_getInt(_state, "total_wins", player.info.total_wins);
    player.info.bot = (bool)Json_getInt(_state, "bot", player.info.bot);

    return true;
}

bool PlayerState::parseUserBalance(Json* _root) {
    
    if (!_root) {
        return false;
    }

    Json *_balance = Json_getItem(_root, "balance");
    if (!_balance) {
        return false;
    }
    
    player.balance.gold = Json_getInt(_balance, "gold", player.balance.gold);
    player.balance.gem = Json_getInt(_balance, "gem", player.balance.gem );
    player.balance.food = Json_getInt(_balance, "food", player.balance.food);
    
CCLog_("coin: %d, food: %d, gem: %d",
      player.balance.gold,
      player.balance.food,
      player.balance.gem);
    
    int _prev_fan = player.balance.fan;
    player.balance.fan = Json_getInt(_balance, "fan", player.balance.fan);
    player.balance._fan_changed = player.balance.fan - _prev_fan;
    
    return true;
    
}

bool PlayerState::parseUserRaceData(Json* _root) {

    if (!_root) {
        return false;
    }
    
    Json *_racing = Json_getItem(_root, "racing");
    if (!_racing) {
        return false;
    }

    int oldLevel = player.racing.level;

    std::string racinghost = "something";
    player.racing.exp = (unsigned int)Json_getInt(_racing, "exp", player.racing.exp);
    player.racing.level = Json_getInt(_racing, "level", player.racing.level);
    player.racing.start = Json_getInt(_racing, "start", player.racing.start);
    player.racing.direction = (bool)Json_getInt(_racing, "direction", (int)player.racing.direction);
    //player.racing.host = Json_getString(_racing, "host", player.racing.host.c_str());
    player.racing.host = racinghost;
    player.racing.ai_difficulty = (float)Json_getFloat(_racing, "ai_difficulty", player.racing.ai_difficulty);

    int newLevel = player.racing.level;
    
    if(oldLevel != 0 && newLevel > oldLevel) {
        CCLog_("old level: %d", newLevel);
        CCLog_("new level: %d", oldLevel);
        sendPlayerLevelUpEvent(newLevel);
    }
    
    return true;
    
}

void PlayerState::sendPlayerLevelUpEvent(int newLevel)
{
    CCLog_("sendPlayerLevelUpEvent");
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",newLevel), "newLevel");
    FlurryCpp::logEvent(FlurryEvent::PLAYER_LEVEL_UP, params);
    
    CSJson::Value eventParameters;
    eventParameters["newLevel"] = newLevel;
    AnalyticsClient::getInstance()->logEvent(FlurryEvent::PLAYER_LEVEL_UP, eventParameters);

    AppRating::Instance().onLevelUp(newLevel);
    
    CCDictionary* params2 = CCDictionary::create();
    int pNameInt = (int)_FBAppEventParameterNameLevel;
    std::stringstream pName;
    pName << pNameInt;
    params2->setObject(CCString::createWithFormat("%d", newLevel), pName.str().c_str());
    FBAppEventsWrapper::logEvent(_FBAppEventNameAchievedLevel,
                                 params2);
    
    if (newLevel == 10) {
        AppsFlyerWrapper::trackEvent(EVENT_PLAYER_LEVEL_10);
    } else if (newLevel == 20) {
        AppsFlyerWrapper::trackEvent(EVENT_PLAYER_LEVEL_20);
    } else if (newLevel == 30) {
        AppsFlyerWrapper::trackEvent(EVENT_PLAYER_LEVEL_30);
    } else if (newLevel == 40) {
        AppsFlyerWrapper::trackEvent(EVENT_PLAYER_LEVEL_40);
    }
}

void PlayerState::sendPetLevelUpEvent(Pet* pet)
{
    CCAssert(pet != NULL, "pet is null");
    CCLog_("sendPetLevelUpEvent");
    
    CCDictionary* params = CCDictionary::create();
    
    bool isLeader = pet->ppVo.puid == player.racing.host;
    int HP = pet->getHorsepower();
    int racePlayed = pet->ppVo.race_entered;
    int raceWon = pet->ppVo.wins;
    
    params->setObject(CCString::createWithFormat("%d", pet->getLevel()), "newLevel");
    params->setObject(isLeader ? ccs("true") : ccs("false"), "isLeader");
    params->setObject(CCString::createWithFormat("%d", HP), "HP");
    params->setObject(CCString::createWithFormat("%d", racePlayed), "racePlayed");
    params->setObject(CCString::createWithFormat("%d", raceWon), "raceWon");
    
    FlurryCpp::logEvent(FlurryEvent::PET_LEVEL_UP, params);
    
    CSJson::Value eventParameters;
    eventParameters["newLevel"] = pet->getLevel();
    eventParameters["isLeader"] = isLeader;
    eventParameters["HP"] = HP;
    eventParameters["racePlayed"] = racePlayed;
    eventParameters["raceWon"] = raceWon;
    AnalyticsClient::getInstance()->logEvent(FlurryEvent::PET_LEVEL_UP, eventParameters);
}

bool PlayerState::parseTrackData(Json* root) {
    
    if (!root) {
        return false;
    }
    
    Json* _tracks = Json_getItem(root, "track");
    if (!_tracks) {
        return false;
    }
    
    int n = Json_getSize(_tracks);
    for (int i=0; i<n; i++) {
        
        PlayerTrackVO ptvo = PlayerTrackVO();
        Json* track = Json_getItemAt(_tracks, i);
        
        ptvo.type = Json_getString(track, "type", ptvo.type.c_str());
        ptvo.no = Json_getInt(track, "no", ptvo.no);
        ptvo.index = Json_getInt(track, "index", ptvo.index);
        
        GDTrackVO tvo = g_gameContent->getTrackVO(ptvo.type);
        if (tvo.type != "") {
            
            bool _new_obj = true;
            if (tvo.track_type == PT_OBJECT_TRACK) {
                if (Track* t_track = getTrackAtIndex(ptvo.index)) {
                    _new_obj = false;
                    t_track->ptVo = ptvo;
                }
            }else if (tvo.track_type == PT_OBJECT_OBSTACLE) {
                if (Track* t_obstacle = getObstacleAtIndex(ptvo.index)) {
                    _new_obj = false;
                    t_obstacle->ptVo = ptvo;
                }
            }
            
            if (_new_obj) {
                if (Track* t_track = Track::createWithTrackVO(tvo, ptvo)) {
                    
                    t_track->retain();
                    if (tvo.track_type == PT_OBJECT_TRACK) {
                        Tracks.push_back(t_track);
                    }else if (tvo.track_type == PT_OBJECT_OBSTACLE) {
                        Obstacles.push_back(t_track);
                    }
                    
                }
            }
            
        }
        
    }

    return true;
    
}

bool PlayerState::parseBuildingData(Json* root) {
    
    if (!root) {
        return false;
    }
    
    Json *_buildings = Json_getItem(root, "building");
    if (!_buildings) {
        return false;
    }
    
    int n = Json_getSize(_buildings);
    for (int i=0; i<n; i++) {
        
        Json *_building = Json_getItemAt(_buildings, i);
        PlayerBuildingVO _pbvo = PlayerBuildingVO();
        _pbvo.parse(_building);
        
        bool _done = false;
        for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
            Building *t_building = (Building*)(*it);
            if (!t_building->isStateEqual(_pbvo))
                continue;
            t_building->syncState(_pbvo);
            _done = true;
        }
        if (_done) continue;
        
        newBuilding(_pbvo);
        
    }

   return true;
    
}

bool PlayerState::parseDecorationData(Json* _root) {
    
    if (!_root) {
        return false;
    }

    Json *_decorations = Json_getItem(_root, "decoration");
    if (!_decorations) {
        return false;
    }
    
    int n = Json_getSize(_decorations);
    for (int i=0; i<n; i++) {
        
        Json *_decoration = Json_getItemAt(_decorations, i);
        if (!_decoration) continue;
        
        PlayerDecorationVO pdvo = PlayerDecorationVO();
        
        pdvo.duid = Json_getString(_decoration, "duid", NULL);
        pdvo.type = Json_getString(_decoration, "type", NULL);
        pdvo.flip = (bool)Json_getInt(_decoration, "flip", NULL);
        
        Json *_maps = Json_getItem(_decoration, "map");
        int _nmaps = Json_getSize(_maps);
        
        for (int j=0; j<_nmaps; j++) {
            Json *_map = Json_getItemAt(_maps, j);
            int k = (int)_map->valueint;
            pdvo.map[j] = k;
        }
        
        pdvo.status = Json_getString(_decoration, "status", NULL);
        pdvo.updated = StringHelper::toInt(Json_getString(_decoration, "updated", NULL));
        
        bool _new_obj = true;
        for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
            Decoration *t_decoration = (Decoration*)(*it);
            if (t_decoration->pdVo.duid == pdvo.duid &&
                t_decoration->pdVo.duid != "") {
                _new_obj = false;
            }else {
                for (int i=0; i<_nmaps; i++) {
                    if (t_decoration->pdVo.map[i] == pdvo.map[i] && pdvo.map[i] != -1) {
                        _new_obj = false;
                        break;
                    }
                }
            }
            if (!_new_obj) {
                t_decoration->pdVo.duid = pdvo.duid;
                t_decoration->pdVo.status = pdvo.status;
                t_decoration->pdVo.updated = pdvo.updated;
                t_decoration->updateFlip();
                break;
            }
        }
        
        if (_new_obj) {
            GDDecorationVO _dvo = g_gameContent->getDecorationVO(pdvo.type);
            if (_dvo.type != "") {
                if (Decoration* t_decoration = Decoration::createWithDecorationVO(_dvo, pdvo)) {
                    t_decoration->retain();
                    Decorations.push_back(t_decoration);
                }
            }
        }
        
    }

    return true;
    
}

bool PlayerState::parsePetData(Json* _root) {
    
    if (!_root) {
        return false;
    }

    Json *_pets = Json_getItem(_root, "pet");
    if (!_pets) {
        return false;
    }
    
    int n = Json_getSize(_pets);
    for (int i=0; i<n; i++) {
        
        Json *_pet = Json_getItemAt(_pets, i);
        if (!_pet) continue;
        
        PlayerPetVO _ppvo = PlayerPetVO();

        _ppvo.puid = Json_getString(_pet, "puid", NULL);
        _ppvo.type = Json_getString(_pet, "type", NULL);
        _ppvo.level = (unsigned int)Json_getInt(_pet, "level", NULL);
        _ppvo.exp = Json_getFloat(_pet, "exp", NULL);
       
        _ppvo.top_speed = (unsigned int)Json_getInt(_pet, "top_speed", NULL);
        _ppvo.corner_speed = (unsigned int)Json_getInt(_pet, "corner_speed", NULL);
        _ppvo.jumping = (unsigned int)Json_getInt(_pet, "jumping", NULL);
        _ppvo.race_entered = (unsigned int)Json_getInt(_pet, "race_entered", NULL);
        _ppvo.wins = (unsigned int)Json_getInt(_pet, "wins", NULL);
        
        _ppvo.hunger = Json_getFloat(_pet, "hunger", NULL);
        _ppvo.hunger_timestamp = StringHelper::toInt(Json_getString(_pet, "hunger_timestamp", "0"));
        _ppvo.energy = Json_getFloat(_pet, "energy", 0.0f);
        _ppvo.energy_timestamp = StringHelper::toInt(Json_getString(_pet, "energy_timestamp", "0"));
        
        _ppvo.coins = Json_getInt(_pet, "coins", 0);
        _ppvo.coins_timestamp = StringHelper::toInt(Json_getString(_pet, "coins_timestamp", "0"));
        
        _ppvo.located_in = Json_getString(_pet, "located_in", "0");
        _ppvo.updated = StringHelper::toInt(Json_getString(_pet, "updated", NULL));

        
        bool _new_obj = true;
        for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it)
        {
            Pet *t_pet = (Pet*)(*it);
            if (t_pet->ppVo.puid != _ppvo.puid) continue;
            
            CREATE_CHAR_BUFFER(_buffer, 64);
            CREATE_CHAR_BUFFER(_connector, 16);

            unsigned int _old_top_speed = t_pet->ppVo.top_speed;
            unsigned int _old_corner_speed = t_pet->ppVo.corner_speed;
            unsigned int _old_jumping = t_pet->ppVo.jumping;
            
            bool levelUp = false;

            if (_ppvo.level > t_pet->ppVo.level)
            {

                levelUp = true;
                string _message = "";
                
                snprintf(_buffer, _buffer_size, g_gameContent->Word["pet_levelup"].c_str(), t_pet->pVo.title.c_str());
                _message.append(_buffer);
                t_pet->message = _message;
                
                if (g_sceneManager->sceneSelector == SCENE_VILLE)
                {
                    t_pet->playLevelUpEffect(false);
                }
                
            }
            else if(_ppvo.exp > t_pet->ppVo.exp)
            {
                if (g_sceneManager->sceneSelector == SCENE_VILLE)
                {
                    t_pet->playLevelUpEffect(true);
                }                
            }
            
            if (_ppvo.energy > t_pet->ppVo.energy)
            {
                t_pet->setAnimation(PET_TOUCH);
            }
            
            _new_obj = false;
            
            t_pet->reload(t_pet->ppVo.type != _ppvo.type);
            t_pet->ppVo = _ppvo;
            t_pet->updateHunger();
            t_pet->updateCoinsProduced();
            
            unsigned int _increased_top_speed = _ppvo.top_speed - _old_top_speed; //Json_getInt(_increased_skills, "top_speed", 0);
            unsigned int _increased_corner_speed = _ppvo.corner_speed - _old_corner_speed; //Json_getInt(_increased_skills, "corner_speed", 0);
            unsigned int _increased_jumping = _ppvo.jumping - _old_jumping; //Json_getInt(_increased_skills, "jumping", 0);
            
            string _message2 = "";
            strncpy(_connector, "", _connector_size);
            
            if (_increased_top_speed > 0) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["increased_top_speed"].c_str(), _connector, _increased_top_speed);
                _message2.append(_buffer);
                strncpy(_connector, g_gameContent->Word["levelup_connector"].c_str(), _connector_size);
            }
            if (_increased_corner_speed > 0) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["increased_corner_speed"].c_str(), _connector, _increased_corner_speed);
                _message2.append(_buffer);
                strncpy(_connector, g_gameContent->Word["levelup_connector"].c_str(), _connector_size);
            }
            if (_increased_jumping > 0) {
                snprintf(_buffer, _buffer_size, g_gameContent->Word["increased_jumping"].c_str(), _connector, _increased_jumping);
                _message2.append(_buffer);
                strncpy(_connector, g_gameContent->Word["levelup_connector"].c_str(), _connector_size);
            }
            
            if (_message2 != "") {
                t_pet->message2 = _message2;
            }
            
            if (g_sceneManager->sceneSelector != SCENE_RACE && g_sceneManager->sceneSelector != SCENE_RACE2) {
                t_pet->printMessage();
            }

            if(levelUp) {
                sendPetLevelUpEvent(t_pet);
            }
            
            break;
            
        }
        
        if (_new_obj) {
            GDPetVO _pvo = g_gameContent->getPetVO(_ppvo.type);
            if (_pvo.type != "") {
                if (Pet *t_pet = Pet::createWithPetVO(_pvo, _ppvo)) {
                    t_pet->retain();
                    Pets.push_back(t_pet);
                }
            }
        }
        
    }

    return true;
    
}

bool PlayerState::parseAbilityData(Json* _root) {
    
    if (!_root) {
        return false;
    }
    
    Json *_items = Json_getItem(_root, "abilities");
    if (!_items) {
        return false;
    }
    
    int n = Json_getSize(_items);
    for (int i=0; i<n; i++) {
        
        Json *_item = Json_getItemAt(_items, i);
        if (!_item) continue;
            
        string _type = Json_getString(_item, "type", NULL);
        string _status = Json_getString(_item, "status", NULL);
        int _level = Json_getInt(_item, "level", NULL);
        
        Ability *_ability = Abilities[_type];
        if (!_ability) {
            _ability = Ability::create();
            _ability->retain();
        }
        
        if (_ability->paVo.level < _level &&
            _ability->paVo.status == kAbilityIsUpgrading &&
            _status == "ready") {
            if (Building* _library = getBuilding(kBuildingLibrary, 0)) {
                _library->runUpgradeFinishEffect();
            }
        }
        
        _ability->paVo.status = _status;
        if (_ability->paVo.status == "ready" && _level > 0) {
            _ability->aVo = GDAbilityVO(g_gameContent->Abilites[_type][_level-1]);
        }else {
            _ability->aVo = GDAbilityVO(g_gameContent->Abilites[_type][_level]);
        }
        
        _ability->paVo.auid = Json_getString(_item, "auid", NULL);
        _ability->paVo.type = _type;
        _ability->paVo.level = _level;
        _ability->paVo.timestamp = (unsigned int)StringHelper::toInt(Json_getString(_item, "timestamp", NULL));

        _ability->setActive(player.token != "");
        _ability->setInitial();
        
        Abilities[_type] = _ability;
        
    }

    updateItemData();
    
    return true;
    
}

bool PlayerState::parseChestGemsData(Json* _root) {
    
    if (!_root) {
        return false;
    }
    
    Json *_items = Json_getItem(_root, "chestgems");
    if (!_items) {
        return false;
    }
    
    CREATE_CHAR_BUFFER(_buf, 64);
    int n = Json_getSize(_items);
    for (int i=0; i<n; i++) {
        Json *_item = Json_getItemAt(_items, i);
        if (!_item) continue;
            
        PlayerChestGemsVO _vo = PlayerChestGemsVO();
        _vo.uuid = Json_getString(_item, "uuid", _vo.uuid.c_str());
        if (_vo.uuid != player.uuid) continue;
        _vo.guid = Json_getString(_item, "guid", _vo.guid.c_str());
        _vo.locate = Json_getString(_item, "locate", _vo.locate.c_str());
        _vo.gems = Json_getInt(_item, "gems", _vo.gems);
        _vo.posx = Json_getFloat(_item, "posx", _vo.posx);
        _vo.posy = Json_getFloat(_item, "posy", _vo.posy);
        snprintf(_buf, _buf_size, "%d", _vo.timestamp);
        _vo.timestamp = (unsigned int)StringHelper::toInt(Json_getString(_item, "timestamp", _buf));
        addChestGems(_vo);
        
    }
    
    return true;
    
}

bool PlayerState::parseItemData(Json* _root) {
    
    if (!_root) {
        return false;
    }
    
    Json *_items = Json_getItem(_root, "item");
    if (!_items) {
        return false;
    }
    
    _clearAllItems();
    int n = Json_getSize(_items);
    for (int i=0; i<n; i++) {
        Json *_item = Json_getItemAt(_items, i);
        IntKeyValue *_ikv = new IntKeyValue();
        _ikv->key = Json_getString(_item, "type", NULL);
        _ikv->key = StringHelper::trim(_ikv->key);
        _ikv->value = Json_getInt(_item, "amount", NULL);
        m_items.push_back(_ikv);
    }
    updateItemData();

    return true;
    
}

void PlayerState::updateBuildingsTimer(float delta_) {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            _building->updateTimer(delta_);
        }
    }
    
}

void PlayerState::updateItemData() {
    
    _clearItems();
    int _item_capacity = getTotalItemStorageCapacity();
    
    for (vector<IntKeyValue*>::iterator it=m_items.begin(); it!=m_items.end(); ++it) {
        if (IntKeyValue* _ikv = (IntKeyValue*)(*it)) {
            
             if (_item_capacity <= 0) {
                 return;
             }
            
            IntKeyValue* _nikv = new IntKeyValue();
            _nikv->key = _ikv->key;
            _nikv->value = _ikv->value;
            
             if (_nikv->value > _item_capacity) {
                 _nikv->value = _item_capacity;
             }
             _item_capacity -= _nikv->value;
            
            Items.push_back(_nikv);
            
        }
    }
    
}

string PlayerState::uncompressData(const char *json_str) {

    //JsonAllocator allocator;
    Json* _root = Json_create(json_str);
    if (!_root) return "";
    string compressedBase64EncodedStr = Json_getString(_root, "compressed", "");
    Json_dispose(_root);
    
    if (compressedBase64EncodedStr == "") return "";
    
    string compressed_data_buffer = base64_decode(compressedBase64EncodedStr);
    string uncompressed_data = StringHelper::decompress_string(compressed_data_buffer);
    return uncompressed_data;
    
}

bool PlayerState::parseUserData(const char *json_str) {
    
    return _parseUserData(json_str, false);
    
}

bool PlayerState::parseOppoData(const char *json_str) {
    
    return _parseUserData(json_str, true);
    
}

bool PlayerState::_parseUserData(const char *json_str, bool opponent) {

#if DEBUG_ENABLE_PROFILERS
    CCProfiler::sharedProfiler()->createAndAddTimerWithName("Profiler_PlayerState");
    CCProfilingBeginTimingBlock("Profiler_PlayerState");
#endif

    string uncompressed_data = uncompressData(json_str);
    CCLog("%s", uncompressed_data.c_str());
    if (uncompressed_data != "") {
        return _parseUserData(uncompressed_data.c_str(), opponent);
    }
    
    clear();

    // parse string json only once
    //JsonAllocator allocator;
    
    Json* root = new Json;
    
    root = Json_create(json_str);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse User Info
    parseUserState(root);
    
    parseUserInfo(root);
    determineUser(root, opponent);
    parseUserProfile(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Balance Info
    parseUserBalance(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Racing Data
    parseUserRaceData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse CheckList Data
    parseChecklistData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Track Data
    parseTrackData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse ChestGems Data
    parseChestGemsData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Building Data
    parseBuildingData(root);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Decoration Data
    parseDecorationData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Ability Data
    parseAbilityData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Pet Data
    parsePetData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Item Data
    parseItemData(root);

    // delete json object
    Json_dispose(root);

    m_didParseData = true;
    
#if DEBUG_ENABLE_PROFILERS
    CCProfilingEndTimingBlock("Profiler_PlayerState");
    CCProfiler::sharedProfiler()->displayTimers();
#endif

    return true;
}

bool PlayerState::parseRaceReplay(const char *json_str_) {
    
    string uncompressed_data = uncompressData(json_str_);
    if (uncompressed_data != "") {
        return parseRaceReplay(uncompressed_data.c_str());
    }
    
    clear();

    // parse string json only once
    //JsonAllocator allocator;
    Json* root = Json_create(json_str_);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse User Info
    parseUserState(root);
    
    parseUserInfo(root);
    determineUser(root, true);
    parseUserProfile(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Racing Data
    parseUserRaceData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Track Data
    parseTrackData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Building Data
    parseBuildingData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Decoration Data
    parseDecorationData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Pet Data
    parsePetData(root);
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Replay Data
    parseReplayData(root);

    // delete json object
    Json_dispose(root);

    m_didParseData = true;

    return true;
}

bool PlayerState::parseHistoryItemData(Json*_root) {
    
    if (!_root) {
        return false;
    }
    
    CREATE_CHAR_BUFFER(_buf, 32);
    
    Json *_items = Json_getItem(_root, "history");
    if (!_items) {
        return false;
    }
    
    HistoryItems.clear();

    int n = Json_getSize(_items);
    for (int i=0; i<n; i++) {
        
        Json *_item = Json_getItemAt(_items, i);
        HistoryItemVO _hivo = HistoryItemVO();
        
        _hivo.race_id = Json_getString(_item, "race_id", _hivo.race_id.c_str());
        _hivo.created = Json_getString(_item, "created", _hivo.created.c_str());
        snprintf(_buf, _buf_size, "%d", _hivo.timestamp);
        _hivo.timestamp = (unsigned int)StringHelper::toInt(Json_getString(_item, "timestamp", _buf));

        if (Json *_guest = Json_getItem(_item, "guest")) {
            _hivo.guest.uuid = Json_getString(_guest, "uuid", _hivo.guest.uuid.c_str());
            _hivo.guest.level = Json_getInt(_guest, "level", _hivo.guest.level);
            _hivo.guest.name = Json_getString(_guest, "name", _hivo.guest.name.c_str());
            _hivo.guest.photo = Json_getString(_guest, "photo", _hivo.guest.photo.c_str());
            _hivo.guest.pet = Json_getString(_guest, "pet", _hivo.guest.pet.c_str());
            _hivo.guest.time = (float)Json_getFloat(_guest, "time", _hivo.guest.time);
            _hivo.guest.fan = Json_getInt(_guest, "fan", _hivo.guest.fan);
        }
        if (Json *_host = Json_getItem(_item, "host")) {
            _hivo.host.uuid = Json_getString(_host, "uuid", _hivo.host.uuid.c_str());
            _hivo.host.level = Json_getInt(_host, "level", _hivo.host.level);
            _hivo.host.name = Json_getString(_host, "name", _hivo.host.name.c_str());
            _hivo.host.photo = Json_getString(_host, "photo", _hivo.host.photo.c_str());
            _hivo.host.pet = Json_getString(_host, "pet", _hivo.host.pet.c_str());
            _hivo.host.time = (float)Json_getFloat(_host, "time", _hivo.host.time);
            _hivo.host.fan = Json_getInt(_host, "fan", _hivo.host.fan);
        }
        if (Json *_result = Json_getItem(_item, "result")) {
            _hivo.result.host_win = (bool)Json_getInt(_result, "host_win", _hivo.result.host_win);
            _hivo.result.bonus = Json_getInt(_result, "bonus", _hivo.result.bonus);
            _hivo.result.coins_reward = Json_getInt(_result, "coins_reward", _hivo.result.coins_reward);
            _hivo.result.gems_reward = Json_getInt(_result, "gems_reward", _hivo.result.gems_reward);
            _hivo.result.claimed = (bool)Json_getInt(_result, "claimed", _hivo.result.claimed);
        }
        
        HistoryItems.push_back(_hivo);
        
    }
    return true;
    
}

bool PlayerState::parseSellItemResult(Json*_root) {
    
    if (!_root) {
        return false;
    }
    
    if (Json_getItem(_root, "sold_item") && Json_getItem(_root, "sold_amount")) {
        
        GDItemVO _itemVo = g_gameContent->getItemVO(Json_getString(_root, "sold_item", NULL));
        if (_itemVo.type != "") {
            int _item_sold_amount = Json_getInt(_root, "sold_amount", NULL);
            
            CREATE_CHAR_BUFFER(_buffer, 64);
            
            if (g_gameContent->Word["item_sold_msg_amount_first"] != "") {
                snprintf(_buffer, _buffer_size,
                        g_gameContent->Word["item_sold_msg_amount_first"].c_str(),
                        _item_sold_amount,
                        ((_item_sold_amount > 1) ? (_itemVo.title_plural.c_str()) : (_itemVo.title.c_str())));
            } else if (g_gameContent->Word["item_sold_msg_title_first"] != "") {
                snprintf(_buffer, _buffer_size,
                        g_gameContent->Word["item_sold_msg_title_first"].c_str(),
                        ((_item_sold_amount > 1) ? (_itemVo.title_plural.c_str()) : (_itemVo.title.c_str())),
                        _item_sold_amount);
            }
            
            g_sceneManager->notification->addMessage(MSGN_TYPE_INFO, _buffer);
        }
        
        return true;
        
    }
    
    return false;
    
}

bool PlayerState::parseWorldMapVillages(Json*_root) {

    if (!_root) {
        return false;
    }
    
    if (Json* _villages = Json_getItem(_root, "villages")) {
        
        WorldMapVillages.clear();
        
        int _nvillages = Json_getSize(_villages);
        for (int i=0; i<_nvillages; i++) {
            if (Json* _village = Json_getItemAt(_villages, i)) {
                PlayerWorldMapVillageVO _vo;
                _vo.village_id = Json_getInt(_village, "village_id", 0);
                _vo.uuid = Json_getString(_village, "uuid", "");
                _vo.name = Json_getString(_village, "name", "");
                _vo.photo = Json_getString(_village, "photo", "");
                _vo.horse_power = Json_getInt(_village, "horse_power", 0);
                _vo.status = Json_getString(_village, "status", "");
                _vo.timestamp = (unsigned int)StringHelper::toInt(Json_getString(_village, "timestamp", NULL));
                _vo.wait = (unsigned int)Json_getInt(_village, "wait", 0);
                _vo.host_type = Json_getString(_village, "host_type", "");
                _vo.bot = (unsigned int)Json_getInt(_village, "bot", 0);
                WorldMapVillages[_vo.village_id] = _vo;
            }
        }
    }
    
    return true;
    
}

void PlayerState::parseReplayData(Json*_root) {
    
    if (!_root) {
        return;
    }
    
    if (Json *_guest = Json_getItem(_root, "guest")) {
        
        opponentInfo.name = Json_getString(_guest, "name", NULL);
        opponentInfo.photo = Json_getString(_guest, "photo", NULL);
        
        if (Json *_pet = Json_getItem(_guest, "pet")) {
            
            opponentPet.puid = Json_getString(_pet, "puid", NULL);
            opponentPet.type = Json_getString(_pet, "type", NULL);
            opponentPet.level = (unsigned int)Json_getInt(_pet, "level", NULL);
            opponentPet.top_speed = (unsigned int)Json_getInt(_pet, "top_speed", NULL);
            opponentPet.corner_speed = (unsigned int)Json_getInt(_pet, "corner_speed", NULL);
            opponentPet.jumping = (unsigned int)Json_getInt(_pet, "jumping", NULL);
            opponentPet.race_entered = (unsigned int)Json_getInt(_pet, "race_entered", NULL);
            opponentPet.wins = (unsigned int)Json_getInt(_pet, "wins", NULL);
            opponentPet.located_in = Json_getString(_pet, "located_in", NULL);
            opponentPet.updated = (unsigned int)Json_getInt(_pet, "updated", NULL);
        }

        opponentTime = Json_getFloat(_guest, "time", NULL);
        opponentPet.helper_pet = Json_getString(_guest, "helperpet_type", "");
    }
    
    if (Json *_host_info = Json_getItem(_root, "info")) {
        playerInfo.name = Json_getString(_host_info, "name", NULL);
        playerInfo.photo = Json_getString(_host_info, "photo", NULL);
    }
    
    if (Json *_pets = Json_getItem(_root, "pet")) {
        if (Json *_pet = Json_getItemAt(_pets, 0)) {
            playerPet.puid = Json_getString(_pet, "puid", NULL);
            playerPet.type = Json_getString(_pet, "type", NULL);
            playerPet.level = (unsigned int)Json_getInt(_pet, "level", NULL);
            playerPet.top_speed = (unsigned int)Json_getInt(_pet, "top_speed", NULL);
            playerPet.corner_speed = (unsigned int)Json_getInt(_pet, "corner_speed", NULL);
            playerPet.jumping = (unsigned int)Json_getInt(_pet, "jumping", NULL);
            playerPet.race_entered = (unsigned int)Json_getInt(_pet, "race_entered", NULL);
            playerPet.wins = (unsigned int)Json_getInt(_pet, "wins", NULL);
            playerPet.located_in = Json_getString(_pet, "located_in", NULL);
            playerPet.updated = (unsigned int)Json_getInt(_pet, "updated", NULL);
        }
    }
    
    playerTime = Json_getFloat(_root, "time", NULL);
    
    if (Json *_replays = Json_getItem(_root, "replay")) {
        replayGuestRaw = Json_getString(_replays, "guest", "");
        replayHostRaw = Json_getString(_replays, "host", "");
        replayHelperPet = Json_getString(_replays, "helperpet", "");
    }
    
    itemDrops = Json_getString(_root, "item_drops", NULL);
    itemDrops = StringHelper::trim(itemDrops);
    
    CCLog_("Parsed race replay!");
    
}

bool PlayerState::parseChecklistData(Json*_root) {
    
    if (!_root) {
        return false;
    }
    
    Json *_checklists = Json_getItem(_root, "checklist");
    if (!_checklists) {
        return false;
    }
    
    // Do not Checklist.clear() as we don not want to
    // uncheck any checklist in the game
    
    int n = Json_getSize(_checklists);
    for (int i=0; i<n; i++) {
        Json* _checklist = Json_getItemAt(_checklists, i);
        if (!_checklist || !_checklist->name) continue;
        std::string _checkkey = _checklist->name;
        Checklist[_checkkey] = (bool)_checklist->valueint;
    }
    
    return true;
    
}

void PlayerState::onMapDataRetrieved(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = (PlayerState*)sender;
    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    _playerState->parseWorldMapVillages(_root);
    _playerState->parseChestGemsData(_root);
    _playerState->parseHistoryItemData(_root);
    Json_dispose(_root);
    
}

void PlayerState::removePetData(Json*_root) {
    
    if (!_root) {
        return;
    }
    
    if (Json_getItem(_root, "remove_puid")) {
        string remove_puid = Json_getString(_root, "remove_puid", NULL);
        if (remove_puid != "") {
            int i = 0;
            for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it) {
                if (Pet *_pet = (Pet*)(*it)) {
                    if (_pet->ppVo.puid == remove_puid) {
                        CCLog_("pet=%s deleted!", _pet->ppVo.puid.c_str());
                        
                        _pet->removeFromParentAndCleanup(true);
                        _pet = NULL;
                        
                        Pets.erase(Pets.begin() + i);
                        
                        break;
                    }
                }
                i++;
            }
            
        }
    }
}

void PlayerState::removeDecorationData(Json*_root) {
    
    if (!_root) {
        return;
    }
    
    if (Json_getItem(_root, "removed_duid")) {
        string removed_duid = Json_getString(_root, "removed_duid", NULL);
        if (removed_duid != "") {
            int i = 0;
            for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
                if (Decoration *_decoration = (Decoration*)(*it)) {
                    if (_decoration->pdVo.duid == removed_duid) {
                        CCLog_("decoration=%s deleted!", _decoration->pdVo.duid.c_str());
                        
                        _decoration->removeMe();
                       
                        Decorations.erase(Decorations.begin() + i);
                        CCLog_("Number of Decoration = %d", (int)Decorations.size());
                        
                        g_gameAudio->playEffect(AUDIO_CUT_TREE);
                        
                        break;
                    }
                }
                i++;
            }
        }
    }
}

string PlayerState::getTrackData() {
    
    CREATE_CHAR_BUFFER(buf_, 1024);
    
    string buffer;
    buffer.clear();
    
    // Packing track data
    buffer.append("[");
    
    bool t_anytrack = false;
    
    for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        
        if (Track* track = (Track*)(*it)) {
            
            PlayerTrackVO ptvo = track->ptVo;
            
            snprintf(buf_, buf__size, "{\"type\":\"%s\",\"no\":%d,\"index\":%d},", ptvo.type.c_str(), ptvo.no, ptvo.index);
            
            buffer.append(buf_);
            
            t_anytrack = true;
        }
        
    }
    
    for (vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
        
        if (Track* obstacle = (Track*)(*it)) {
            
            PlayerTrackVO ptvo = obstacle->ptVo;
            
            snprintf(buf_, buf__size, "{\"type\":\"%s\",\"no\":%d,\"index\":%d},", ptvo.type.c_str(), ptvo.no, ptvo.index);
            
            buffer.append(buf_);
            
            t_anytrack = true;
        }
        
    }
    
    if (buffer.size() > 0 && t_anytrack) buffer.resize(buffer.size()-1);
    buffer.append("]");
    
    return buffer;
    
}

bool PlayerState::saveUserDataToLocal() {

    CREATE_CHAR_BUFFER(buf_, 1024);
    
    string buffer;
    buffer.clear();
    
    //open
    buffer.append("{");
    snprintf(buf_, buf__size, "\"uuid\":\"%s\",", player.uuid.c_str());
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"fbid\":\"%s\",", player.fbid.c_str());
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"avid\":\"%s\",", player.avid.c_str());
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"token\":\"%s\",", player.token.c_str());
    
    buffer.append("\"info\":{");
    snprintf(buf_, buf__size, "\"name\":\"%s\",", player.info.name.c_str());
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"photo\":\"%s\"", player.info.photo.c_str());
    buffer.append(buf_);
    buffer.append("},");
    
    buffer.append("\"balance\":{");
    snprintf(buf_, buf__size, "\"gold\":%d,", player.balance.gold);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"gem\":%d,", player.balance.gem);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"food\":%d", player.balance.food);
    buffer.append(buf_);
    buffer.append("},");
    
    // Packing racing data
    buffer.append("\"racing\":{");
    snprintf(buf_, buf__size, "\"level\":%d,", player.racing.level);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"status\":%d,", (int)player.racing.status);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"start\":%d,", player.racing.start);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"direction\":%d,", (int)player.racing.direction);
    buffer.append(buf_);
    snprintf(buf_, buf__size, "\"host\":\"%s\"", player.racing.host.c_str());
    buffer.append(buf_);
    buffer.append("},");
    
    // Packing track data
    buffer.append("\"track\":[");
    bool t_anytrack = false;
    for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        
        if (Track* track = (Track*)(*it)) {
    
            PlayerTrackVO ptvo = track->ptVo;
            
            snprintf(buf_, buf__size, "{\"type\":\"%s\",\"no\":%d,\"index\":%d},", ptvo.type.c_str(), ptvo.no, ptvo.index);
            
            buffer.append(buf_);
            
            t_anytrack = true;
        }
        
    }
    
    for (vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
        
        if (Track* obstacle = (Track*)(*it)) {
            
            PlayerTrackVO ptvo = obstacle->ptVo;
            
            snprintf(buf_, buf__size, "{\"type\":\"%s\",\"no\":%d,\"index\":%d},", ptvo.type.c_str(), ptvo.no, ptvo.index);
            
            buffer.append(buf_);
            
            t_anytrack = true;
        }
        
    }
    
    if (buffer.size() > 0 && t_anytrack) buffer.resize(buffer.size()-1);
    buffer.append("],");
    
    //Packing building data
    buffer.append("\"building\":[");
    bool t_anybuilding = false;
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        
        if (Building* building = (Building*)(*it)) {
            
            PlayerBuildingVO pbvo = building->pbVo;
            
            CREATE_CHAR_BUFFER(map_, (BUILDING_SPACE_GRID_NUM*5)+2);
            snprintf(map_, map__size, "[");
            bool added = false;
            for (int i=0; i<BUILDING_SPACE_GRID_NUM; i++) {
                snprintf(map_, map__size, "%s%d,", map_, pbvo.map[i]);
                added = true;
            }
            string map__;
            map__.clear();
            map__.append(map_);
            if (added && map__.size() > 0) map__.resize(map__.size()-1);
        
            map__.append("]");
            
            snprintf(buf_, buf__size, "{\"buid\":\"%s\",\"type\":\"%s\",\"flip\":%d,\"map\":%s,\"status\":\"%s\",\"updated\":\"%d\"},", pbvo.buid.c_str(), pbvo.type.c_str(), (int)pbvo.flip, map__.c_str(), pbvo.status.c_str(), pbvo.timestamp);
            
            buffer.append(buf_);
            
            t_anybuilding = true;
        }
        
        
    }
    
    if (buffer.size() > 0 && t_anybuilding) buffer.resize(buffer.size()-1);
    buffer.append("]");
    
    // close
    buffer.append("}");
    
    //printf("-----------------------------------------------------------\n");
    //printf("write: %s\n", buffer.c_str());
    
    CCUserDefault::sharedUserDefault()->setStringForKey("userdata", "");
    CCUserDefault::sharedUserDefault()->setStringForKey("userdata", buffer.c_str());
    
    return true;
 
    
}

bool PlayerState::didParseData() {
    
    return m_didParseData;
    
}

void PlayerState::checklist(std::string checkkey_) {

    Checklist[checkkey_] = true;
    BackendService::getInstance()->send(APIChecklistUpdate::create(checkkey_,
                                                                   this,
                                                                   PlayerState::onChecklistUpdated));
    
}

void PlayerState::onChecklistUpdated(void* sender, string json_str) {
    
    PlayerState* _this = (PlayerState*)sender;
    
    //JsonAllocator allocator;
    Json *_root = Json_create(json_str.c_str());
    _this->parseChecklistData(_root);
    Json_dispose(_root);
    
}


///////////////////////////////////////////////////////
// Objects Functions

bool PlayerState::newBuilding(PlayerBuildingVO vo) {
    
    GDBuildingVO _bvo = GameContentService::getInstance()->getBuildingVO(vo.type,
                                                                         vo.level);
    if (_bvo.type == "") return false;

    Building* _new_building = Building::createWithBuildingVO(_bvo, vo);
    if (!_new_building) return false;
    
    if (player.token == "") _new_building->setActive(false);
    _new_building->setInitialProduct();
    _new_building->updateCountdown();

    return addBuilding(_new_building);
    
}

bool PlayerState::addBuilding(Building* building) {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building* _building = (Building*)(*it);
        if (building != _building) continue;
        return false;
    }
    
    building->retain();
    Buildings.push_back(building);
    return true;

}

Building* PlayerState::getBuilder() {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        
        Building* _building = (Building*)(*it);
        if (_building->pbVo.status == BUILDING_STATUS_DELIVERED ||
            _building->pbVo.status == BUILDING_STATUS_COMPLETE) continue;
        
        Builder = _building;
        return Builder;
    
    }
    
    return NULL;
    
}

void PlayerState::moveBuilding(Building *building_) {
    
    if (building_) {
        building_->didMove = true;
        m_didMoveBuilding = true;
    }
    
}

string PlayerState::getChangedBuildingsJSON() {
    
    string _json_raw = "";
    
    if (m_didMoveBuilding) {
        m_didMoveBuilding = false;
        
        _json_raw.append("[");
        
        int i = 0;
        for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
            if (Building* _building = (Building*)(*it)) {
                if (_building->didMove) {
                    if (i != 0) {
                        _json_raw.append(",");
                    }
                    _json_raw.append(_building->getMoveJsonRendered());
                    i++;
                }
            }
        }
        
        _json_raw.append("]");
        
    }
    
    return _json_raw;
    
}

bool PlayerState::gainExp(GDBuildingVO vo) {
    
    player.racing.exp += vo.village_exp_gain;
    
    GDExperienceLevelVO nxt_exp_level_vo = GameContentService::getInstance()->ExperienceLevel[player.racing.level];
    if (nxt_exp_level_vo.level == 0) return false;
    
    bool _isLevelUp = false;
    float required_exp = nxt_exp_level_vo.required_exp;
    while (player.racing.exp >= required_exp) {
        player.racing.exp -= required_exp;
        player.racing.level ++;
        _isLevelUp = true;
        nxt_exp_level_vo = GameContentService::getInstance()->ExperienceLevel[player.racing.level];
        if (nxt_exp_level_vo.level == 0) break;
        required_exp = nxt_exp_level_vo.required_exp;
    }

    return _isLevelUp;
    
}

bool PlayerState::addTrack(Track* track_) {
    
    bool t_add = true;
    
    if (track_->tVo.track_type == PT_OBJECT_TRACK) {
        
        for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
            
            if (Track* _track = (Track*)(*it)) {
                if (track_ == _track) {
                    t_add = false;
                }
            }
            
        }
        
        if (t_add) {
            Tracks.push_back(track_);
            
        }
        
    }else if (track_->tVo.track_type == PT_OBJECT_OBSTACLE) {
        
        for (vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
            if (Track* _obstacle = (Track*)(*it)) {
                if (track_ == _obstacle) {
                    t_add = false;
                }
            }
        }
        
        if (t_add) {
            Obstacles.push_back(track_);
        }
        
    }
    
    return t_add;
    
}

bool PlayerState::removeTrack(Track *track) {
    
    if (!track) {
        CCLog_("(PlayerState::addTrack) Warning: track is uninitialized!");
        return false;
    }
    
    GDTrackVO vo = g_gameContent->getTrackVO(track->ptVo.type);
    if (vo.type == "") {
        CCLog_("(PlayerState::addTrack) Error: track=%s doesn't exist!", track->ptVo.type.c_str());
        return false;
    }
        
    int i=0;
    if (vo.track_type == PT_OBJECT_TRACK) {
        
        for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
            if (Track* track_ = (Track*)(*it)) {
                if (track_ == track) {
                    
                    Tracks[i] = Tracks.back();
                    Tracks.pop_back();
                    
                    return true;
                }
            }
            i++;
        }
        
    }else if (vo.track_type == PT_OBJECT_OBSTACLE) {
        
        for (vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
            if (Track* obstacle_ = (Track*)(*it)) {
                if (obstacle_ == track) {
                    
                    Obstacles[i] = Obstacles.back();
                    Obstacles.pop_back();
                    
                    return true;
                }
            }
            i++;
        }
        
    }
    
    return false;
    
}

bool PlayerState::addDecoration(Decoration *decoration_) {
    
    bool t_add = true;
    for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
        
        if (Decoration* _decoration = (Decoration*)(*it)) {
            if (decoration_ == _decoration) {
                t_add = false;
            }
        }
        
    }
    
    if (t_add) {
        decoration_->retain();
        Decorations.push_back(decoration_);
    }
    
    return t_add;
    
}

bool PlayerState::removeDecoration(Decoration *decoration_) {
    
    decoration_->release();
    decoration_->removeFromParent();

    return false;
    
}

void PlayerState::moveDecoration(Decoration *decoration_) {
    
    if (decoration_) {
        decoration_->didMove = true;
        m_didMoveDecoration = true;
    }
    
}

string PlayerState::getChangedDecorationsJSON() {
    
    string _json_raw = "";
    
    if (m_didMoveDecoration) {
        m_didMoveDecoration = false;
        
        _json_raw.append("[");
        
        int i = 0;
        for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
            if (Decoration* _decoration = (Decoration*)(*it)) {
                if (_decoration->didMove) {
                    if (i != 0) {
                        _json_raw.append(",");
                    }
                    _json_raw.append(_decoration->getMoveJsonRendered());
                    i++;
                }
            }
        }
        
        _json_raw.append("]");
        
    }
    
    return _json_raw;
    
}

#pragma mark - HelperPets

void PlayerState::updateHelperPetsList() {
    
    if (PlayerState::getInstance()->isFacebookLoggedIn() || HelperPets.size() == 0) return;
        
    vector<int> index_to_remove;
    
    int i=0;
    for (vector<PlayerHelperPetVO>::iterator it=HelperPets.begin(); it!=HelperPets.end(); ++it) {
        PlayerHelperPetVO _helperpet_vo = *it;
        if (_helperpet_vo.isFriend) index_to_remove.push_back(i);
        i++;
    }
    
    while (index_to_remove.size() > 0) {
        HelperPets.erase(HelperPets.begin()+int(index_to_remove[index_to_remove.size()-1]));
        index_to_remove.pop_back();
    }
        
}

#pragma mark - ChestGems

bool PlayerState::addChestGems(PlayerChestGemsVO vo_) {
    
    bool result = false;
    
    if (vo_.guid == "") {
        return false;
    }
    
    CCDictionary *params = CCDictionary::create();
    params->setObject(CCString::createWithFormat("%d",vo_.gems), "gem");
    params->setObject(ccs(vo_.locate), "source");
    FlurryCpp::logEvent(EVENT_GEM_EARN_TREASURE_BOX, params);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = vo_.locate;
    eventParameters["gem"] = vo_.gems;
    AnalyticsClient::getInstance()->logEvent(EVENT_GEM_EARN_TREASURE_BOX, eventParameters);
    
    ChestGems* _chestgems = (ChestGems*)ChestGemsDict[vo_.guid];
    if (!_chestgems) {
        _chestgems = ChestGems::create();
        _chestgems->retain();
        ChestGemsDict[vo_.guid] = _chestgems;
        result = true;
    }
    
    _chestgems->cgVo.uuid = vo_.uuid;
    _chestgems->cgVo.guid = vo_.guid;
    _chestgems->cgVo.locate = vo_.locate;
    _chestgems->cgVo.gems = vo_.gems;
    _chestgems->cgVo.posx = vo_.posx;
    _chestgems->cgVo.posy = vo_.posy;
    _chestgems->cgVo.timestamp = vo_.timestamp;

    return result;
    
}

bool PlayerState::removeChestGems(ChestGems *chestgems_) {
    
    ChestGemsDict.erase(chestgems_->cgVo.guid);
    
    chestgems_->release();
//    chestgems_->removeFromParentAndCleanup(true);
    
    return true;
}

#pragma mark - Achievement

bool PlayerState::prePrecessAchievementClaim(GDAchievementsVO avo) {
    
    if (Checklist[avo.type.c_str()]) return false;
    
    Checklist[avo.type.c_str()] = true;
    player.balance.gem += avo.gems_reward;

    return true;
    
}

#pragma mark - Mechant Item

bool PlayerState::preBuyItemFromMerchant(std::string item_type) {
    bool result = false;
    string items = player.info.merchant_items;
    string merchantItems = "";
    vector<string> item_arr = StringHelper::split((const char*) items.c_str(), ',');
    if(item_arr.size()>0)
    {
        for(int i=0;i<item_arr.size();i++)
        {
            if(item_arr[i] == item_type)
            {
                item_arr[i] =  "-"+ item_arr[i] ;
            }
            merchantItems.append(item_arr[i]+",");
        }
        if(merchantItems.length() > 0)
        {
            merchantItems = merchantItems.substr(0,merchantItems.length()-1);
            player.info.merchant_items =merchantItems;
            result = true;
        }
    }
    return result;
}
bool PlayerState::merchantHasItemForSell()
{
    string items = player.info.merchant_items;
    bool hasItemSell = false;
    vector<string> item_arr = StringHelper::split((const char*) items.c_str(), ',');
    if(item_arr.size()>0)
    {
        for(int i=0;i<item_arr.size();i++)
        {
            int indexMinus = (int)item_arr[i].find("-");
            if(indexMinus != 0)
            {
                hasItemSell = true;
                break;
            }
        }
    }

    return hasItemSell;
}
bool PlayerState::merchantHasOnlyOneLastItemForSell()
{
    string items = player.info.merchant_items;
    bool isLastitem = false;
    int countSoldItem = 0;
    vector<string> item_arr = StringHelper::split((const char*) items.c_str(), ',');
    if(item_arr.size()>0)
    {
        for(int i=0;i<item_arr.size();i++)
        {
            int indexMinus = (int)item_arr[i].find("-");
            if(indexMinus == 0)
            {
                countSoldItem++;
            }
        }
    }
    if(countSoldItem == item_arr.size()-1 )
    {
        isLastitem = true;
    }
    return isLastitem;
}

#pragma mark - User Status

bool PlayerState::isOnline() {
    
    if (last_seen_mins_ago < 0) return false;
    bool is_online = (last_seen_mins_ago < g_gameContent->GameConfig["MINUTE_LAST_SEEN_AS_ONLINE"]);
    CCLog_("last seen: %d  < %d = %d",  last_seen_mins_ago, int(g_gameContent->GameConfig["MINUTE_LAST_SEEN_AS_ONLINE"]), (int)is_online);
    return is_online;
    
}

bool PlayerState::isBot() {
    
    return player.info.bot;
    
}

bool PlayerState::isFacebookLoggedIn() {
    
    return (player.fbid != "" &&
            g_sceneManager->facebook->isLoggedIn());
    
}

bool PlayerState::isBeingRaced() {
    
    int last_being_attacked_to_now_in_sec = getSecondsSinceLastBeingRaced();
    
    CCLog_("last_being_attacked: %d", player.info.last_being_attacked);
    CCLog_("        currentTime: %d", currentTime);
    CCLog_("                  =: %d)", last_being_attacked_to_now_in_sec);
    CCLog_("%d < %f", last_being_attacked_to_now_in_sec, g_gameContent->GameConfig["AVG_RACE_DURATION_IN_SEC"]);
    
    //return (last_being_attacked_to_now_in_sec < g_gameContent->GameConfig["AVG_RACE_DURATION_IN_SEC"]);
    return false;
    
}

int PlayerState::getSecondsSinceLastBeingRaced() {
    
    return (currentTime - player.info.last_being_attacked);
    
}

int PlayerState::getSecondsLeftToPlayAgain() {
    
    return int(g_gameContent->GameConfig["AVG_RACE_DURATION_IN_SEC"]) - getSecondsSinceLastBeingRaced();
    
}

int PlayerState::getNBuilding(string type) {
    
    int count = 0;
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* building = (Building*)(*it)) {
            if (building->pbVo.type == type) {
                count++;
            }
        }
    }
    
    return count;
    
}

int PlayerState::getNBuildingWithPets(string type) {
    
    int count = 0;
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building* building = (Building*)(*it);
        if (building->pbVo.type == type &&
            building->pet != NULL) {
            count++;
        }
    }
    
    return count;
    
}

Building *PlayerState::getBuilding(string buid_) {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building *_building = (Building*)(*it)) {
            if (_building->pbVo.buid == buid_) {
                return _building;
            }
        }
    }
    
    return NULL;
    
}

Building *PlayerState::getBuilding(string type_, int index_) {
    
    int _index = 0;
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building *_building = (Building*)(*it);
        if (_building->pbVo.type != type_) continue;
        if (index_ == _index) return _building;
        _index++;
    }
    
    return NULL;
    
}

int PlayerState::getSumOfAllResourcesProducedByAllBuildingsOfThisType(string type_) {
    
    int sumResources = 0;
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building *_building = (Building*)(*it);
        if (_building->pbVo.type != type_) continue;
        
        sumResources += _building->getResourceProduced();
    }
    
    return sumResources;
}

Building *PlayerState::getAvailableHabitat() {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building *_building = (Building*)(*it);
        if (_building == NULL)
            continue;
        
        if (!(_building->pbVo.type == kBuildingHabitat ||
              _building->pbVo.type == kBuildingHabitatPremium))
            continue;
        
        CCLog_("_building->pbVo.buid:%s",_building->pbVo.buid.c_str());
        CCLog_("_building->pbVo.puid:%s",_building->pbVo.puid.c_str());
        CCLog_("_building->pbVo.status:%s",_building->pbVo.status.c_str());
        CCLog_("_building->pet:%d",_building->pet != NULL);
        CCLog_("----------------------------");
        
        if (_building->pet == NULL &&
            _building->pbVo.status == BUILDING_STATUS_DELIVERED) {
            
            return _building;
        }
    }
    
    return NULL;
    
}

Building *PlayerState::getAvailableTrainingGym() {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building *_building = (Building*)(*it)) {
            if (_building->pbVo.type == kBuildingTraining && !_building->spine_pet_running) {
                return _building;
            }
        }
    }
    
    return NULL;
    
}

void PlayerState::calAllBuildingCountdown() {
    
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            _building->updateCountdown();
        }
    }
    
}

Track* PlayerState::getTrackAtIndex(int index) {

    for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        Track* track = (Track*)(*it);
        if (track->ptVo.index != index) continue;
        return track;
    }
    
    return NULL;
    
}

Track* PlayerState::getTrackToStart(int index) {
    
    Track* track = getTrackAtIndex(index);
    
    if (track &&
        (track->ptVo.no == 1 || track->ptVo.no == 3)) {
        return track;
    }
    
    for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        track = (Track*)(*it);
        if (track->ptVo.no != 1 && track->ptVo.no != 3) continue;
        return track;
    }

    if (!Tracks.empty())
    {
        return Tracks[0];
    }

    return NULL;
}

Track* PlayerState::getObstacleAtIndex(int index) {
    
    for (vector<Track*>::iterator it=Obstacles.begin(); it!=Obstacles.end(); ++it) {
        if (Track* obstacle = (Track*)(*it)) {
            if (obstacle->ptVo.index == index) {
                return obstacle;
            }
        }
    }
    
    return NULL;
    
}

Pet *PlayerState::getPet(string puid_) {
    
    for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
//            printf("%s==%s\n",_pet->ppVo.puid.c_str(), puid_.c_str());
            if (_pet->ppVo.puid == puid_) {
                return _pet;
            }
        }
    }
    
    return NULL;
    
}

Pet *PlayerState::getPetWithHighestHorsePower() {
    
    Pet *petToReturn = NULL;
    
    for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it) {
        if (Pet *_pet = (Pet*)(*it)) {
            if (!petToReturn) {
                petToReturn = _pet;
            }
            
            if (this->getHorsePowerFromPet(_pet) > this->getHorsePowerFromPet(petToReturn)) {
                petToReturn = _pet;
            }
        }
    }
    
    return petToReturn;
    
}

int PlayerState::getHorsePowerFromPet(Pet *pet) {
    
    return pet->ppVo.top_speed + pet->ppVo.corner_speed + pet->ppVo.jumping;
}

int PlayerState::getNPet() {
    
    return (int)Pets.size();
    
}

int PlayerState::getNTree() {
    
    int _count = 0;
    for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
        if (Decoration *_decoration = (Decoration*)(*it)) {
            string tree_type = _decoration->pdVo.type.substr(0,4);
            if(tree_type == "tree") {
                _count++;
            }
        }
    }
    CCLog_("Trees count:%d",_count);
    return _count;
    
}

int PlayerState::getNDecoration(string type) {
    
    int _count = 0;
    for (vector<Decoration*>::iterator it=Decorations.begin(); it!=Decorations.end(); ++it) {
        Decoration *_decoration = *it;
        if (_decoration->pdVo.type == type) _count ++;
    }
    return _count;

}

int PlayerState::getNKeptDecoration(string type) {
    
    int _count = 0;
    for (vector<Decoration*>::iterator it = Decorations.begin(); it != Decorations.end(); ++it) {
        Decoration* _decoration = *it;
        if (_decoration->dVo.type == type && _decoration->pdVo.status == kDecorationStatusKept) _count ++;
    }
    return _count;
    
}

Decoration* PlayerState::getKeptDecoration(string type) {
    
    for (vector<Decoration*>::iterator it = Decorations.begin(); it != Decorations.end(); ++it) {
        Decoration* _decoration = *it;
        if (_decoration->dVo.type != type ||
            _decoration->pdVo.status != kDecorationStatusKept) continue;
        return _decoration;
    }
    return NULL;
    
}

bool PlayerState::isDecorationMaximumReached(std::string type) {

    int n = getNDecoration(type);
    GDDecorationVO vo = g_gameContent->getDecorationVO(type);
    
    return n >= vo.maximum;
    
}

//int PlayerState::getNUnclaimedHistoryItem() {
//    
//    int n = 0;
//    for (vector<HistoryItemVO>::iterator it=HistoryItems.begin(); it!=HistoryItems.end(); ++it) {
//        HistoryItemVO _item = *it;
//        if (!_item.result.host_win || _item.result.claimed) continue;
//        n++;
//    }
//    return n;
//    
//}

bool PlayerState::isThisANewHistoryItem(std::string race_id) {
    
    return !CCUserDefault::sharedUserDefault()->getBoolForKey(race_id.c_str(), false);
    
}

int PlayerState::getNewHistoryBadgeNumber() {
    
    int n = 0;
    for (vector<HistoryItemVO>::iterator it=HistoryItems.begin(); it!=HistoryItems.end(); ++it) {
        HistoryItemVO _item = *it;
        if (!isThisANewHistoryItem(_item.race_id)) continue;
        n++;
    }
    return n;
    
}

void PlayerState::clearNewHistoryBadgeNumber() {
    
    for (vector<HistoryItemVO>::iterator it=HistoryItems.begin(); it!=HistoryItems.end(); ++it) {
        HistoryItemVO _item = *it;
        CCUserDefault::sharedUserDefault()->setBoolForKey(_item.race_id.c_str(), true);
    }
    
}

void PlayerState::checkAndPrintIfItemBarnIsFull() {
    
    if (getNItems() >= getTotalItemStorageCapacity()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_WARNING, g_gameContent->Word["warning_item_over_capacity_msg"].c_str());
    }
    
}

bool PlayerState::checkAndReturnIfItemBarnIsFull()
{
    if (getNItems() >= getTotalItemStorageCapacity())
    {
        return true;
    }
    else
    {
        return false;
    }
}

int PlayerState::getNHomelessPet() {
    
    int count = 0;
    
    for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it) {
        Pet *_pet = (Pet*)(*it);
        if (_pet->ppVo.located_in != "") continue;
        count ++;
    }
    
    return count;
    
}

int PlayerState::getNItem(string type_) {
    
    for (vector<IntKeyValue*>::iterator it=Items.begin(); it!=Items.end(); ++it) {
        if (IntKeyValue *_item = (IntKeyValue*)(*it)) {
            if (_item->key == type_) {
                return _item->value;
            }
        }
    }
    
    return 0;
    
}

int PlayerState::getNItems() {
    
    int count = 0;
    for (vector<IntKeyValue*>::iterator it=Items.begin(); it!=Items.end(); ++it) {
        if (IntKeyValue *_item = (IntKeyValue*)(*it)) {
            count += _item->value;
        }
    }
    return count;
    
}

Ability* PlayerState::getUpgradingAbility() {
    
    return getUpgradingAbility(0);
    
}

Ability* PlayerState::getUpgradingAbility(int index_) {
    
    int _count = 0;
    for (map<string, Ability*>::iterator it=Abilities.begin(); it!=Abilities.end(); ++it) {
        if (Ability* _ability = it->second) {
            if (_ability->paVo.status == kAbilityIsUpgrading && _count == index_) {
                if (_count == index_) {
                    return _ability;
                }
                _count++;
            }
            
        }
    }
    
    return NULL;
}

void PlayerState::calculateTrackpointsUsed() {
    
    player.racing._track_points = 0;
    for (vector<Track*>::iterator it=Tracks.begin(); it!=Tracks.end(); ++it) {
        Track* _track = (Track*)(*it);
        player.racing._track_points += _track->tVo.point_used;
    }
    
    player.racing._track_points_limit = g_gameContent->GameConfig["HQ_BASE_TRACKPOINTS"]; //28
    Building* _building_association = getBuilding(kBuildingAssociation, 0);
    if (!_building_association)
        return;
    //_building_association level1: +4
    
    GDBuildingVO _building_association_vo =
    GameContentService::getInstance()->getBuildingVO(_building_association->pbVo.type,
                                                     _building_association->pbVo.level);

    if (_building_association->pbVo.status == BUILDING_STATUS_DELIVERED ||
        _building_association->pbVo.status == BUILDING_STATUS_UPGRADE) {
        player.racing._track_points_limit += _building_association_vo.param_1;
        return;
    }
    
}

void PlayerState::addGold(int amount_) {
    
    player.balance.gold += getMaxCoinCanCollect(amount_);
    
}

void PlayerState::addFood(int amount_) {
    
    player.balance.food += getMaxFoodCanCollect(amount_);
    
}

int PlayerState::getTotalCoinStorageCapacity() {
    
    int _total_coin_capacity = (int)g_gameContent->GameConfig["HQ_BASE_COIN_STORAGE"];
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        Building* _building = (Building*)(*it);
        bool is_active_coin_storage =   (_building->bVo.building_type == BUILDING_TYPE_COIN_STORAGE) &&
                                        (_building->pbVo.status == BUILDING_STATUS_DELIVERED ||
                                         _building->pbVo.status == BUILDING_STATUS_UPGRADE);
        if (is_active_coin_storage) {
            _total_coin_capacity += _building->bVo.param_1; // param1 is coin storage capacity of each building
        }
    }
    return _total_coin_capacity;
    
}

int PlayerState::getTotalFoodStorageCapacity() {
    
    int _total_food_capacity = (int)g_gameContent->GameConfig["HQ_BASE_FOOD_STORAGE"];
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            if (_building->bVo.building_type == BUILDING_TYPE_FOOD_STORAGE &&
                (_building->pbVo.status == BUILDING_STATUS_DELIVERED ||
                 _building->pbVo.status == BUILDING_STATUS_UPGRADE)) {
                _total_food_capacity += _building->bVo.param_1; // param1 is food storage capacity of each building
            }
        }
    }
    return _total_food_capacity;
    
}

int PlayerState::getTotalItemStorageCapacity() {
    
    int _total_item_capacity = 0;//(int)g_gameContent->GameConfig["HQ_BASE_ITEM_STORAGE"];
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            if (_building->bVo.building_type == BUILDING_TYPE_INVENTORY &&
                (_building->pbVo.status == BUILDING_STATUS_DELIVERED ||
                 _building->pbVo.status == BUILDING_STATUS_UPGRADE)) {
                    _total_item_capacity += _building->bVo.param_1; // param1 is item storage capacity of each building
                }
        }
    }
    return _total_item_capacity;
    
}

int PlayerState::getMaxCoinCanCollect(int amount_) {
    
    int _total_coin_capacity = getTotalCoinStorageCapacity();
    if (player.balance.gold + amount_ > _total_coin_capacity) {
        return _total_coin_capacity - player.balance.gold;
    }
    return amount_;
    
}

int PlayerState::getMaxFoodCanCollect(int amount_) {
    
    int _total_food_capacity = getTotalFoodStorageCapacity();
    if (player.balance.food + amount_ > _total_food_capacity) {
        return _total_food_capacity - player.balance.food;
    }
    return amount_;
    
}

int PlayerState::getVisitorsPerHour() {
    
    return g_gameContent->GameConfig["BASE_VISITORS_AT_0_FANS"]
            +(player.balance.fan * g_gameContent->GameConfig["FANS_TO_VISITORS_PER_HOUR_RATIO"]);
    
}

int PlayerState::getFoodsProductionPerHour() {
    
    float fph = 0;
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            if (_building->bVo.building_type == BUILDING_TYPE_PRODUCER &&
                _building->pbVo.status == BUILDING_STATUS_DELIVERED) {
                fph += _building->bVo.param_2 * 3600.0f;
            }
        }
    }
    
    return (int)round(fph);
    
}
Ability* PlayerState::getAbilityForKey(std::string key)
{
    for (vector<string>::iterator it=g_gameContent->AbilitiesOrder.begin(); it!=g_gameContent->AbilitiesOrder.end(); ++it) {
        if (Ability* _ability = this->Abilities[*it]) {
            if (_ability->paVo.type == key) {
                return _ability;
                break;
            }
        }
    }
    return NULL;
}

bool PlayerState::hasAbilityForKey(std::string key)
{
    Ability *ability = getAbilityForKey(key);
    if (ability) {
        return ability->paVo.level > 0;
    }
    return false;
}

void PlayerState::cleanAllEffects()
{
    for (vector<Building*>::iterator it=Buildings.begin(); it!=Buildings.end(); ++it) {
        if (Building* _building = (Building*)(*it)) {
            _building->cleanEffect();
        }
    }
    for (vector<Pet*>::iterator it=Pets.begin(); it!=Pets.end(); ++it) {
        if (Pet* _pet = (Pet*)(*it)) {
            _pet->cleanEffect();
        }
    }
}

