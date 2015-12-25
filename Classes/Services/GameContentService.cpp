//
//  GameContentService.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 8/29/56 BE.
//
//

#include "GameContentService.h"
USING_NS_CC;
using namespace std;

#include "../AppMacros.h"
#include <spine/Json.h>

using namespace cocos2d::extension;

extern string gamedataVersion;
extern float scaleFactor;
extern TargetDevice targetDevice;
extern unsigned int currentTime;

#include "BackendService.h"
#include "APIs/APIGameDataGet.h"
#include "APIs/APILiveDataGet.h"

#include "../Helpers/StringHelper.h"

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;
#include "SimpleAudioEngine.h"

#include "../Helpers/base64.h"
#include "../Helpers/UIHelper.h"

#include "../Scenes/SceneBoost.h"

GameContentService *g_gameContent = NULL;

GameContentService* GameContentService::getInstance() {
    
    if (!g_gameContent) {
        g_gameContent = new GameContentService();
    }
    
    return g_gameContent;
    
}

void GameContentService::destroyInstance() {
    
    if (!g_gameContent) return;
    
    delete g_gameContent;
    g_gameContent = NULL;
    
}

GameContentService::GameContentService() {

    userRecord = NULL;
    playerRecord = NULL;
    opponentRecord = NULL;
    
    unique_key = CCUserDefault::sharedUserDefault()->getStringForKey("unique_key");
    if (unique_key == "") unique_key = "undefined";
    
    m_didParse = false;
    
    loadGameDataFromLocal();
    
}

GameContentService::~GameContentService() {
    
    this->_clearAll();
    
}

bool GameContentService::loadGameDataFromLocal(const char* type) {
    
    CREATE_CHAR_BUFFER(_filename, 64);
    snprintf(_filename, _filename_size, "%s.json", type);
    
    unsigned long _fileSize = 0;
    unsigned char *_fileContents = NULL;
    
    std::string _fullPath = CCFileUtils::sharedFileUtils()->fullPathFromRelativeFile(_filename,
                                                                                     kGameContentServiceDir);
    _fileContents = CCFileUtils::sharedFileUtils()->getFileData(_fullPath.c_str(), "r", &_fileSize);
    
    CCLog_("Load %s", _filename);
    
    bool result = parseGameData((char*)_fileContents, true);
    
    delete _fileContents;
    _fileContents = NULL;

    return result;
    
}

bool GameContentService::loadGameDataFromLocal() {
    
    vector<string> _list;
    GAMEDATA_LIST(_list);
    bool _result = false;
    
    for (vector<string>::iterator it=_list.begin(); it!=_list.end(); ++it) {
        _result = loadGameDataFromLocal((*it).c_str());
    }

    CCLog_("Load Game Data From Local Completed!: %d", _result);

    return _result;
    
}

void GameContentService::pullLiveGameConfig() {
    
    BackendService::getInstance()->send(APILiveDataGet::create(kGameContentServiceLiveGameConfig,
                                                               this,
                                                               GameContentService::onLiveGameConfigRetrieved));
    
}

void GameContentService::onLiveGameConfigRetrieved(void *sender, std::string resp_str) {

    //JsonAllocator _allocator;
    Json* _root = Json_create(resp_str.c_str());
    
    if (Json_getItem(_root, "compressed")) {
        string compressed = Json_getString(_root, "compressed", "");
        string uncompressed = uncompress(compressed);
        Json_dispose(_root);
        return onLiveGameConfigRetrieved(sender, uncompressed.c_str());
    }
    
    CCLog_("%s", resp_str.c_str());
    
    if (Json* _data = Json_getItem(_root, kGameContentServiceLiveGameConfig)) {
        int n = Json_getSize(_data);
        for (int i=0; i<n; i++) {
            Json* _item = Json_getItemAt(_data, i);
            string _key = _item->name;
            ((GameContentService*)sender)->LiveGameConfig[_key] = atof(_item->valuestring);
        }
    }
    
    Json_dispose(_root);
    
}

void GameContentService::removeAssetContent(SceneList previousScene_) {

    CCSpriteFrameCache *_cacher = CCSpriteFrameCache::sharedSpriteFrameCache();
    
    CREATE_CHAR_BUFFER(_filename, 128);
    
    CCLog_("Removing Sprite Frames 0");
    
    //clean
    _cacher->removeSpriteFramesFromFile("icon_building.plist");
    _cacher->removeSpriteFramesFromFile("bg_gacha_new.plist");
    _cacher->removeSpriteFramesFromFile("common/particle_effect/animal_trade_effect.plist");
    
    CCLog_("Removing Sprite Frames 1");

    switch (targetDevice) {
        default:
        case TD_IPHONE:
        case TD_IPHONE_RETINA:
        case TD_IPAD: {
            for (int i=0; i<1; i++) {
                snprintf(_filename, _filename_size, "item%d.plist", i);
                _cacher->removeSpriteFramesFromFile(_filename);
                snprintf(_filename, _filename_size, "gacha_pet_%d.plist", i);
                _cacher->removeSpriteFramesFromFile(_filename);
            }
            _cacher->removeSpriteFramesFromFile("common/world/medium/worldmap.plist");
            _cacher->removeSpriteFramesFromFile("common/tutorial/medium/tutorial.plist");
        }break;
        case TD_IPAD_RETINA: {
            for (int i=0; i<2; i++) {
                snprintf(_filename, _filename_size, "item%d.plist", i);
                _cacher->removeSpriteFramesFromFile(_filename);
                snprintf(_filename, _filename_size, "gacha_pet_%d.plist", i);
                _cacher->removeSpriteFramesFromFile(_filename);
            }
            _cacher->removeSpriteFramesFromFile("common/world/large/worldmap.plist");
            _cacher->removeSpriteFramesFromFile("common/tutorial/large/tutorial.plist");
        }break;
    }
    
    switch (targetDevice) {
        default:
        case TD_IPHONE:
        case TD_IPHONE_RETINA: {
            _cacher->removeSpriteFramesFromFile("common/particles/particles-medium.plist");
        }break;
        case TD_IPAD:
        case TD_IPAD_RETINA: {
            _cacher->removeSpriteFramesFromFile("common/particles/particles-large.plist");
        }
    }
    
    CCLog_("Removing Sprite Frames 2");

    //TODO: removeUnusedTextures only when switching back from WorldMap scene & SceneBoost scene
    
    
    if (previousScene_ == SCENE_BOOST) {
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(SPLASH_SOUND_2);
        CCLog_("Removing Sprite Frames 3");
    }
    
    if (previousScene_ == SCENE_WORLDMAP ||
        previousScene_ == SCENE_BOOST)
    {
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
        CCLog_("Removing Sprite Frames 4");
    }

//    NR_TRACE_METHOD_STOP;
}

void GameContentService::loadAssetContent(SceneList _scene) {

#if DEBUG_ENABLE_PROFILERS
    CCProfiler::sharedProfiler()->createAndAddTimerWithName("Profiler_loadAssetContent");
    CCProfilingBeginTimingBlock("Profiler_loadAssetContent");
#endif

    CCSpriteFrameCache *_cacher = CCSpriteFrameCache::sharedSpriteFrameCache();

    CREATE_CHAR_BUFFER(_filename, 128);
    
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    
    //add
    switch (_scene) {
        case SCENE_VILLE: {
            
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
            _cacher->addSpriteFramesWithFile("common/particle_effect/animal_trade_effect.plist");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_PVRTC4);
#endif

            _cacher->addSpriteFramesWithFile("bg_gacha_new.plist");
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
            
            _cacher->addSpriteFramesWithFile("icon_building.plist");
            
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "item%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                        snprintf(_filename, _filename_size, "gacha_pet_%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                    _cacher->addSpriteFramesWithFile("common/tutorial/medium/tutorial.plist");
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("item0.plist");
                    _cacher->addSpriteFramesWithFile("item1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_0.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_2.plist");
                    
                    _cacher->addSpriteFramesWithFile("common/tutorial/large/tutorial.plist");
                }break;
            }

        }break;
        case SCENE_VISIT:
        case SCENE_VISIT2: {
            
#ifdef DEBUG_ANIMAL_TRADING_ON_VISIT
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
            _cacher->addSpriteFramesWithFile("common/particle_effect/animal_trade_effect.plist");
            
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
#endif
            
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "item%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                        snprintf(_filename, _filename_size, "gacha_pet_%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                    _cacher->addSpriteFramesWithFile("common/tutorial/medium/tutorial.plist");
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("item0.plist");
                    _cacher->addSpriteFramesWithFile("item1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_0.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_2.plist");
                    
                    _cacher->addSpriteFramesWithFile("common/tutorial/large/tutorial.plist");
                }break;
            }
            
        }break;
        case SCENE_MATCH:
        case SCENE_MATCH2: {
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "item%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                        snprintf(_filename, _filename_size, "gacha_pet_%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                    _cacher->addSpriteFramesWithFile("common/tutorial/medium/tutorial.plist");
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/world/medium/worldmap.plist");
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("item0.plist");
                    _cacher->addSpriteFramesWithFile("item1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_0.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_1.plist");
                    _cacher->addSpriteFramesWithFile("gacha_pet_2.plist");
                    
                    _cacher->addSpriteFramesWithFile("common/tutorial/large/tutorial.plist");
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/world/medium/worldmap.plist");
                }break;
            }
        }break;
        case SCENE_RACE:
        case SCENE_RACE2: {
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "item%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                    _cacher->addSpriteFramesWithFile("common/tutorial/medium/tutorial.plist");
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("item0.plist");
                    _cacher->addSpriteFramesWithFile("item1.plist");
                    
                    _cacher->addSpriteFramesWithFile("common/tutorial/large/tutorial.plist");
                }break;
            }
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA: {
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/particles/particles-medium.plist");
                }break;
                case TD_IPAD:
                case TD_IPAD_RETINA: {
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/particles/particles-large.plist");
                }
            }
        }break;
        case SCENE_WORLDMAP: {
            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    _cacher->addSpriteFramesWithFile("common/tutorial/medium/tutorial.plist");
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/world/medium/worldmap.plist");
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("common/tutorial/large/tutorial.plist");
                    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
                    _cacher->addSpriteFramesWithFile("common/world/large/worldmap.plist");
                }break;
            }
        }break;
        default: // ALL SCENES
        case SCENE_BOOST: {

            // cache all new ui sprites
            UIHelper::cacheSprites();

            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_PVRTC4);
#endif

            _cacher->addSpriteFramesWithFile("common/bg/bg_surrounding_0.plist");
            _cacher->addSpriteFramesWithFile("common/bg/bg_surrounding_1.plist");
            _cacher->addSpriteFramesWithFile("common/bg/bg_surrounding_2.plist");
            _cacher->addSpriteFramesWithFile("common/bg/bg_surrounding_3.plist");

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            //
            // this is necessary to avoid visible seam when connecting track
            //
            CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
#endif

            switch (targetDevice) {
                default:
                case TD_IPHONE:
                case TD_IPHONE_RETINA:
                case TD_IPAD: {
                    _cacher->addSpriteFramesWithFile("common/ui/ui-map-medium0.plist");
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "common/tile/tile-medium%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                }break;
                case TD_IPAD_RETINA: {
                    _cacher->addSpriteFramesWithFile("common/ui/ui-map-large0.plist");
                    for (int i=0; i<1; i++) {
                        snprintf(_filename, _filename_size, "common/tile/tile-large%d.plist", i);
                        _cacher->addSpriteFramesWithFile(_filename);
                    }
                }break;
            }

            
        }break;
    }
    
    
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    
#if DEBUG_ENABLE_PROFILERS
    CCProfilingEndTimingBlock("Profiler_loadAssetContent");
#endif
}

string GameContentService::correctFileNameWithPath(const char *pszFileName) {
    
    string fullPath = CCFileUtils::sharedFileUtils()->fullPathForFilename(pszFileName);
    CCDictionary *dict = CCDictionary::createWithContentsOfFileThreadSafe(fullPath.c_str());
    CCDictionary* metadataDict = (CCDictionary*)dict->objectForKey("metadata");
    string texturePath("");
    texturePath = metadataDict->valueForKey("textureFileName")->getCString();
    dict->release();
    return texturePath;
    
}

string GameContentService::uncompress(std::string compressedBase64EncodedStr_) {
    
    string compressed_data_buffer = base64_decode(compressedBase64EncodedStr_);
    string uncompressed_data = StringHelper::decompress_string(compressed_data_buffer);
    return uncompressed_data;
    
}

bool GameContentService::parseGameData(const char *json_) {
    
    return parseGameData(json_, false);
    
}

bool GameContentService::parseGameData(const char *json_, bool from_local_) {

//    NR_TRACE_METHOD_START(0);

    //JsonAllocator allocator;
    Json *_root = Json_create(json_);
    if (!_root) {
        return false;
    }
    
//    if (Json_getItem(_root, "unique_key")) {
//        string _unique_key_str = Json_getString(_root, "unique_key", "");
//        if (unique_key != _unique_key_str) {
//            unique_key = _unique_key_str;
//            CCUserDefault::sharedUserDefault()->setStringForKey("unique_key", unique_key);
//            return true;
//        }else {
//            return _loadGameDataFromCache();
//        }
//    }
//    
//    if (Json *_list = Json_getItem(_root, "gamedata_list")) {
//        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("gamedata_list", json_);
//        int n = Json_getSize(_list);
//        for (int i=0; i<n; i++) {
//            if (Json *_type = Json_getItemAt(_list, i)) {
//                string _type_str = _type->valuestring;
//                if (BackendService::isCreated() && _type_str != "") {
//                    BackendService::getInstance()->send(APIGameDataGet::create(_type_str,
//                                                                               CCDirector::sharedDirector()->getRunningScene(),
//                                                                               SceneBoost::onAllGameDatasRetrieved));
//                }
//            }
//        }
//        Json_dispose(_root);
//        return false;
//    }
//    
//    if (Json_getItem(_root, "compressed")) {
//        string compressed = Json_getString(_root, "compressed", "");
//        string uncompressed = uncompress(compressed);
//        return parseGameData(uncompressed.c_str(), from_local_);
//    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Game Config
    if (Json *_configs = Json_getItem(_root, "game_config")) {
        CCLog_("parseGameData -> GameConfig");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("game_config", json_);
        GameConfig.clear();
        int _nconfig = Json_getSize(_configs);
        for (int i=0; i<_nconfig; i++) {
            if (Json *_config = Json_getItemAt(_configs, i)) {
                string key = Json_getString(_config, "key", NULL);
                float value = Json_getFloat(_config, "value", NULL);
                GameConfig[key] = value;
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Word
    if (Json *_words = Json_getItem(_root, "word")) {
        CCLog_("parseGameData -> Word");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("word", json_);
        Word.clear();
        int _nword = Json_getSize(_words);
        for (int i=0; i<_nword; i++) {
            if (Json *_word = Json_getItemAt(_words, i)) {
                string key = Json_getString(_word, "type", NULL);
                string title = Json_getString(_word, "title", NULL);
                Word[key] = title;
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Hints
    if (Json_getItem(_root, "hints")) {
        CCLog_("parseGameData -> Hints");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("hints", json_);
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Cloud
    if (Json *_cloudgroup = Json_getItem(_root, "cloud")) {
        CCLog_("parseGameData -> Cloud");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("cloud", json_);
        Cloud.clear();
        int _numGroup = Json_getSize(_cloudgroup);
        for (int i=0; i<_numGroup; i++) {
            if (Json *_cloud = Json_getItemAt(_cloudgroup, i)) {
                GDCloudGroupVO _vo;
                _vo.group = Json_getInt(_cloud, "id", 0);
                _vo.unlock_level = Json_getInt(_cloud, "unlocks_at_level", 0);
                _vo.price = Json_getInt(_cloud, "price", 0);
                if (Json *_unlocks = Json_getItem(_cloud, "unlock")) {
                    int _n_unlocks = Json_getSize(_unlocks);
                    for (int j=0; j<5; j++) {
                        if(j<_n_unlocks) {
                            Json *_vill = Json_getItemAt(_unlocks, j);
                            int k = (int)_vill->valueint;
                            _vo.unlock_vill[j] = k;
                        } else {
                            _vo.unlock_vill[j] = -1;
                        }
                    }
                }
                if (Json *_clouds = Json_getItem(_cloud, "cloud")) {
                    int _nclouds = Json_getSize(_clouds);
                    for (int j=0; j<16; j++) {
                        if(j<_nclouds) {
                            Json *_cloud = Json_getItemAt(_clouds, j);
                            int k = (int)_cloud->valueint;
                            _vo.link_cloud[j] = k;
                        }
                        else _vo.link_cloud[j] = -1;
                    }
                }
                Cloud.push_back(_vo);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Achievements
    if (Json *_achievements = Json_getItem(_root, "achievements")) {
        CCLog_("parseGameData -> Achievement");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("achievements", json_);
        Achievement.clear();
        int _nachievement = Json_getSize(_achievements);
        for (int i=0; i<_nachievement; i++) {
            GDAchievementsVO _vo = GDAchievementsVO();
            Json *_achievement = Json_getItemAt(_achievements, i);
            _vo.type            = Json_getString(_achievement, "type", NULL);
            _vo.group           = Json_getString(_achievement, "group", NULL);
            _vo.title           = Json_getString(_achievement, "title", NULL);
            _vo.description     = Json_getString(_achievement, "description", NULL);
            _vo.gems_reward     = (unsigned int)Json_getInt(_achievement, "gems_reward", NULL);
            _vo.param_1         = (unsigned int)Json_getInt(_achievement, "param_1", NULL);
            _vo.param_2         = (unsigned int)Json_getInt(_achievement, "param_2", NULL);
            Achievement.push_back(_vo);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse InApp
    if (Json *_inapps = Json_getItem(_root, "iap_product")) {
        CCLog_("parseGameData -> IAP Product");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("iap_product", json_);
        Inapp.clear();
        int _ninapp = Json_getSize(_inapps);
        for (int i=0; i<_ninapp; i++) {
            if (Json* _inapp = Json_getItemAt(_inapps, i)) {
                if (_inapp->name) {
                    GDIAPVO _vo = GDIAPVO();
                    _vo.product = _inapp->name;
                    _vo.gems_value = _inapp->child->valueint;
                    Inapp.push_back(_vo);
                }
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Experience Level
    if (Json *_exp_levels = Json_getItem(_root, "experience_level")) {
        CCLog_("parseGameData -> Experience Level");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("experience_level", json_);
        ExperienceLevel.clear();
        int _nexp_level = Json_getSize(_exp_levels);
        for (int i=0; i<_nexp_level; i++) {
            if (Json *_exp_level = Json_getItemAt(_exp_levels, i)) {
                GDExperienceLevelVO _vo;
                _vo.level = Json_getInt(_exp_level, "level", 0);
                _vo.required_exp = Json_getInt(_exp_level, "required_exp", 0);
                _vo.race_fee = Json_getInt(_exp_level, "race_fee", 0);
                ExperienceLevel.push_back(_vo);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Pet
    if (Json *_pets = Json_getItem(_root, "pet")) {
        CCLog_("parseGameData -> Pet");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("pet", json_);
        Pet.clear();
        int _npet = Json_getSize(_pets);
        for (int i=0; i<_npet; i++) {
            GDPetVO _vo = GDPetVO();
            Json* _pet = Json_getItemAt(_pets, i);
            _vo.type                    = Json_getString(_pet, "type", NULL);
            _vo.title                   = Json_getString(_pet, "title", NULL);
            _vo.rarity_stars            = Json_getInt(_pet, "rarity_stars", NULL);
            _vo.max_level               = Json_getInt(_pet, "max_level", NULL);
            _vo.base_top_speed          = Json_getInt(_pet, "base_top_speed", NULL);
            _vo.base_corner_speed       = Json_getInt(_pet, "base_corner_speed", NULL);
            _vo.base_jumping            = Json_getInt(_pet, "base_jumping", NULL);
            _vo.evolution_top_speed     = Json_getInt(_pet, "evolution_top_speed", NULL);
            _vo.evolution_corner_speed  = Json_getInt(_pet, "evolution_corner_speed", NULL);
            _vo.evolution_jumping       = Json_getInt(_pet, "evolution_jumping", NULL);
            _vo.track_preference        = Json_getString(_pet, "track_preference", NULL);
            string _delimiter = ",", _delimiter2 = "=";
            size_t _pos = 0, _pos2 = 0;
            
            // parsing next_evolutions
            string _next_evolutions = Json_getString(_pet, "next_evolution", NULL);
            _pos = 0;
            while ((_pos = _next_evolutions.find(_delimiter)) != string::npos) {
                string _next_evolution = _next_evolutions.substr(0, _pos);
                _pos2 = 0;
                FloatKeyValue _npe = FloatKeyValue();
                while ((_pos2 = _next_evolution.find(_delimiter2)) != string::npos) {
                    _npe.key = _next_evolution.substr(0, _pos2);
                    _npe.key = StringHelper::trim(_npe.key);
                    _next_evolution.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.value = atof(_next_evolution.c_str());
                _vo.next_evolution.push_back(_npe);
                _next_evolutions.erase(0, _pos + _delimiter.length());
            }
            if (_next_evolutions != "") {
                string _next_evolution = _next_evolutions;
                _pos2 = 0;
                FloatKeyValue _npe = FloatKeyValue();
                while ((_pos2 = _next_evolution.find(_delimiter2)) != string::npos) {
                    _npe.key = _next_evolution.substr(0, _pos2);
                    _npe.key = StringHelper::trim(_npe.key);
                    _next_evolution.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.value = atof(_next_evolution.c_str());
                _vo.next_evolution.push_back(_npe);
            }
            _vo.exp_multiplier_for_exp_curve = Json_getInt(_pet, "emec", NULL);
            
            // parsing items_needed_for_fusion
            string _inffs = Json_getString(_pet, "inff", NULL);
            _pos = 0;
            while ((_pos = _inffs.find(_delimiter)) != string::npos) {
                string _inff = _inffs.substr(0, _pos);
                _pos2 = 0;
                IntKeyValue _inf = IntKeyValue();
                while ((_pos2 = _inff.find(_delimiter2)) != string::npos) {
                    _inf.key = _inff.substr(0, _pos2);
                    _inf.key = StringHelper::trim(_inf.key);
                    _inff.erase(0, _pos2 + _delimiter2.length());
                }
                _inf.value = StringHelper::toInt(_inff.c_str());
                _vo.items_needed_for_fusion.push_back(_inf);
                _inffs.erase(0, _pos + _delimiter.length());
            }
            if (_inffs != "") {
                string _inff = _inffs;
                _pos2 = 0;
                IntKeyValue _inf = IntKeyValue();
                while ((_pos2 = _inff.find(_delimiter2)) != string::npos) {
                    _inf.key = _inff.substr(0, _pos2);
                    _inf.key = StringHelper::trim(_inf.key);
                    _inff.erase(0, _pos2 + _delimiter2.length());
                }
                _inf.value = StringHelper::toInt(_inff.c_str());
                _vo.items_needed_for_fusion.push_back(_inf);
            }
            
            // parsing items_needed_to_evolve
            string _intes = Json_getString(_pet, "inte", NULL);
            _pos = 0;
            while ((_pos = _intes.find(_delimiter)) != string::npos) {
                string _inte = _intes.substr(0, _pos);
                _pos2 = 0;
                IntKeyValue _ine = IntKeyValue();
                while ((_pos2 = _inte.find(_delimiter2)) != string::npos) {
                    _ine.key = _inte.substr(0, _pos2);
                    _ine.key = StringHelper::trim(_ine.key);
                    _inte.erase(0, _pos2 + _delimiter2.length());
                }
                _ine.value = StringHelper::toInt(_inte.c_str());
                _vo.items_needed_to_evolve.push_back(_ine);
                _intes.erase(0, _pos + _delimiter.length());
            }
            if (_intes != "") {
                string _inte = _intes;
                _pos2 = 0;
                IntKeyValue _ine = IntKeyValue();
                while ((_pos2 = _inte.find(_delimiter2)) != string::npos) {
                    _ine.key = _inte.substr(0, _pos2);
                    _ine.key = StringHelper::trim(_ine.key);
                    _inte.erase(0, _pos2 + _delimiter2.length());
                }
                _ine.value = StringHelper::toInt(_inte.c_str());
                _vo.items_needed_to_evolve.push_back(_ine);
            }
            
            _vo.coins_needed_for_fusion = Json_getInt(_pet, "cnff", NULL);
            _vo.coins_needed_to_evolve = Json_getInt(_pet, "cnte", NULL);
            
            // parsing item_drops
            string _itds = Json_getString(_pet, "item_drops", NULL);
            _pos = 0;
            while ((_pos = _itds.find(_delimiter)) != string::npos) {
                string _itd = _itds.substr(0, _pos);
                _itd = StringHelper::trim(_itd);
                _vo.item_drops.push_back(_itd);
                _itds.erase(0, _pos + _delimiter.length());
            }
            _vo.sell_value          = Json_getInt(_pet, "sell_value", NULL);
            _vo.hunger_rate         = Json_getFloat(_pet, "hunger_rate", NULL);
            _vo.mvhm                = Json_getFloat(_pet, "mvhm", NULL);
            _vo.energy_rate         = Json_getFloat(_pet, "energy_rate", NULL);
            
            _vo.cprmin              = Json_getFloat(_pet, "cprmin", 0.0f);
            _vo.cprmax              = Json_getFloat(_pet, "cprmax", 0.0f);
            _vo.mvcmin              = Json_getInt(_pet, "mvcmin", 0);
            _vo.mvcmax              = Json_getInt(_pet, "mvcmax", 0);
            
            _vo.unlocks_at_level    = Json_getInt(_pet, "unlocks_at_level", NULL);
            _vo.src                 = Json_getString(_pet, "src", NULL);
            _vo.gacha_distribution  = Json_getInt(_pet, "gacha_distribution", NULL);
            _vo.sound_src           = Json_getString(_pet, "sound_src", NULL);
            
            Pet.push_back(_vo);
            
        }   
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Feed Times
    if (Json *_feed_times = Json_getItem(_root, "feed_times")) {
        CCLog_("parseGameData ->Feed Times");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("feed_times", json_);
        FeedTimes.clear();
        int n = Json_getSize(_feed_times);
        for (int i=0; i<n; i++) {
            Json *_feed_time = Json_getItemAt(_feed_times, i);
            int key = Json_getInt(_feed_time, "key", 0);
            float val = Json_getFloat(_feed_time, "value", 0);
            FeedTimes[key] = val;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Initial Pet
    if (Json *_inital_pets = Json_getItem(_root, "initial_pet")) {
        CCLog_("parseGameData -> Initial Pet");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("initial_pet", json_);
        InitialPet.clear();
        int _ninitial_pet = Json_getSize(_inital_pets);
        for (int i=0; i<_ninitial_pet; i++) {
            if (Json *_initial_pet = Json_getItemAt(_inital_pets, i)) {
                InitialPet.push_back(Json_getString(_initial_pet, "type", NULL));
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Abilities
    if (Json *_nodes = Json_getItem(_root, "abilities")) {
        CCLog_("parseGameData -> Abilities");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("abilities", json_);
        Abilites.clear();
        AbilitiesOrder.clear();
        int _nnodes = Json_getSize(_nodes);
        for (int i=0; i<_nnodes; i++) {
            GDAbilityVO _vo = GDAbilityVO();
            Json *_node = Json_getItemAt(_nodes, i);
            _vo.type                    = Json_getString(_node, "type", NULL);
            _vo.level                   = (unsigned int)Json_getInt(_node, "level", NULL);
            _vo.cost_coins              = (unsigned int)Json_getInt(_node, "cost_coins", NULL);
            _vo.time                    = (unsigned int)Json_getInt(_node, "time", NULL);
            _vo.starting_cost           = (unsigned int)Json_getInt(_node, "starting_cost", NULL);
            _vo.increasing_cost         = (unsigned int)Json_getInt(_node, "increasing_cost", NULL);
            _vo.adding_increasing_cost  = (unsigned int)Json_getInt(_node, "adding_increasing_cost", NULL);
            _vo.stun_duration           = Json_getFloat(_node, "stun_duration", NULL);
            _vo.src                     = Json_getString(_node, "src", NULL);
            _vo.unlocks_at_level        = (unsigned int)Json_getInt(_node, "unlocks_at_level", NULL);
            _vo.village_exp_gain        = (unsigned int)Json_getInt(_node, "village_exp_gain", NULL);
            _vo.param_1                 = Json_getFloat(_node, "param_1", NULL);
            _vo.param_2                 = Json_getFloat(_node, "param_2", NULL);
            _vo.param_3                 = Json_getFloat(_node, "param_3", NULL);
            _vo.title                   = Json_getString(_node, "title", NULL);
            _vo.description             = Json_getString(_node, "description", NULL);
            Abilites[_vo.type].push_back(_vo);
            if (_vo.level == 1) {
                AbilitiesOrder.push_back(_vo.type);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Item
    if (Json *_items = Json_getItem(_root, "item")) {
        CCLog_("parseGameData -> Item");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("item", json_);
        Item.clear();
        int _nitem = Json_getSize(_items);
        for (int i=0; i<_nitem; i++) {
            if (Json *_item = Json_getItemAt(_items, i)) {
                GDItemVO _vo = GDItemVO();
                _vo.type            = Json_getString(_item, "type", "");
                _vo.rarity_point    = Json_getInt(_item, "rarity_point", 0);
                _vo.gem_value       = Json_getInt(_item, "gem_value", 0);
                _vo.coins_value     = Json_getInt(_item, "coins_value", 0);
                _vo.buy_value       = Json_getInt(_item, "buy_value", 0);
                _vo.src             = Json_getString(_item, "src", "");
                _vo.src             = StringHelper::trim(_vo.src);
                _vo.title           = Json_getString(_item, "title", "");
                _vo.title_plural    = Json_getString(_item, "title_plural", "");
                _vo.description     = Json_getString(_item, "description", "");
                _vo.description2    = Json_getString(_item, "description2", "");
                Item.push_back(_vo);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Initial Item
    if (Json *_initial_items = Json_getItem(_root, "initial_item")) {
        CCLog_("parseGameData -> Initial Item");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("initial_item", json_);
        InitialItem.clear();
        int _ninitial_item = Json_getSize(_initial_items);
        for (int i=0; i<_ninitial_item; i++) {
            if (Json *_initial_item = Json_getItemAt(_initial_items, i)) {
                GDInitialItemVO _iivo = GDInitialItemVO();
                _iivo.type = Json_getString(_initial_item, "type", NULL);
                // parsing items_needed_to_evolve
                string _items = Json_getString(_initial_item, "item", NULL);
                int _pos = 0, _pos2 = 0;
                string _delimiter = ",";
                string _delimiter2 = "=";
                while ((_pos = _items.find(_delimiter)) != string::npos) {
                    string _item = _items.substr(0, _pos);
                    _pos2 = 0;
                    IntKeyValue _ii = IntKeyValue();
                    while ((_pos2 = _item.find(_delimiter2)) != string::npos) {
                        _ii.key = _item.substr(0, _pos2);
                        _ii.key = StringHelper::trim(_ii.key);
                        _item.erase(0, _pos2 + _delimiter2.length());
                    }
                    _ii.value = StringHelper::toInt(_item.c_str());
                    _iivo.items.push_back(_ii);
                    _items.erase(0, _pos + _delimiter.length());
                }
                if (_items != "") {
                    string _item = _items;
                    _pos2 = 0;
                    IntKeyValue _ii = IntKeyValue();
                    while ((_pos2 = _item.find(_delimiter2)) != string::npos) {
                        _ii.key = _item.substr(0, _pos2);
                        _ii.key = StringHelper::trim(_ii.key);
                        _item.erase(0, _pos2 + _delimiter2.length());
                    }
                    _ii.value = StringHelper::toInt(_item.c_str());
                    _iivo.items.push_back(_ii);
                }
                InitialItem.push_back(_iivo);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Building
    if (Json *_buildings = Json_getItem(_root, "building")) {
        CCLog_("parseGameData -> Building");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("building", json_);
        Building.clear();
        int _nbuilding = Json_getSize(_buildings);
        for (int i=0; i<_nbuilding; i++) {
            GDBuildingVO _vo = GDBuildingVO();
            Json *_building = Json_getItemAt(_buildings, i);
            _vo.type             = Json_getString(_building, "type", NULL);
            _vo.level            = (unsigned int)Json_getInt(_building, "level", NULL);
            _vo.building_type    = Json_getString(_building, "building_type", NULL);
            _vo.title            = Json_getString(_building, "title", NULL);
            _vo.description      = Json_getString(_building, "description", NULL);
            _vo.cost_coins       = (unsigned int)Json_getInt(_building, "cost_coins", NULL);
            _vo.cost_gems        = (unsigned int)Json_getInt(_building, "cost_gems", NULL);
            _vo.cost_multiplier  = (bool)Json_getInt(_building, "cost_multiplier", NULL);
            _vo.time             = (unsigned int)Json_getInt(_building, "time", NULL);
            _vo.in_shop          = (bool)Json_getInt(_building, "in_shop", NULL);
            _vo.grid_col         = (unsigned int)Json_getInt(_building, "grid_col", NULL);
            _vo.grid_row         = (unsigned int)Json_getInt(_building, "grid_row", NULL);
            _vo.movable          = (bool)Json_getInt(_building, "movable", NULL);
            _vo.stack            = Json_getString(_building, "stack", "normal");
            _vo.src              = Json_getString(_building, "src", NULL);
            _vo.src_width        = (unsigned int)Json_getInt(_building, "src_width", NULL);
            _vo.src_height       = (unsigned int)Json_getInt(_building, "src_height", NULL);
            _vo.unlocks_at_level = (unsigned int)Json_getInt(_building, "unlocks_at_level", NULL);
            _vo.village_exp_gain = (unsigned int)Json_getInt(_building, "village_exp_gain", NULL);
            _vo.param_1          = Json_getFloat(_building, "param_1", NULL);
            _vo.param_2          = Json_getFloat(_building, "param_2", NULL);
            _vo.param_3          = Json_getFloat(_building, "param_3", NULL);
            _vo.param_4          = Json_getFloat(_building, "param_4", NULL);
            _vo.param_5          = Json_getFloat(_building, "param_5", NULL);
            _vo.param_6          = Json_getFloat(_building, "param_6", NULL);
            Building.push_back(_vo);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Track
    if (Json *_tracks = Json_getItem(_root, "track")) {
        CCLog_("parseGameData -> Track");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("track", json_);
        Track.clear();
        int _ntrack = Json_getSize(_tracks);
        for (int i=0; i<_ntrack; i++) {
            GDTrackVO _vo = GDTrackVO();
            Json *_track = Json_getItemAt(_tracks, i);
            _vo.type             = Json_getString(_track, "type", NULL);
            _vo.track_type       = Json_getString(_track, "track_type", NULL);
            _vo.title            = Json_getString(_track, "title", NULL);
            _vo.description      = Json_getString(_track, "description", NULL);
            _vo.point_used       = (unsigned int)Json_getInt(_track, "point_used", NULL);
            _vo.unlocks_at_level  = (unsigned int)Json_getInt(_track, "unlocks_at_level", NULL);
            _vo.src              = Json_getString(_track, "src", NULL);
            _vo.param_1          = Json_getFloat(_track, "param_1", NULL);
            _vo.param_2          = Json_getFloat(_track, "param_2", NULL);
            _vo.param_3          = Json_getFloat(_track, "param_3", NULL);
            _vo.param_4          = Json_getFloat(_track, "param_4", NULL);
            Track.push_back(_vo);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Decoration
    if (Json *_decorations = Json_getItem(_root, "decoration")) {
        CCLog_("parseGameData -> Decoration");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("decoration", json_);
        Decoration.clear();
        int _ndecoration = Json_getSize(_decorations);
        for (int i=0; i<_ndecoration; i++) {
            GDDecorationVO _vo = GDDecorationVO();
            Json* _decoration = Json_getItemAt(_decorations, i);
            _vo.type             = Json_getString(_decoration, "type", _vo.type.c_str());
            _vo.decoration_type  = Json_getString(_decoration, "decoration_type", _vo.decoration_type.c_str());
            _vo.title            = Json_getString(_decoration, "title", _vo.title.c_str());
            _vo.description      = Json_getString(_decoration, "description", _vo.description.c_str());
            _vo.cost_coins       = (unsigned int)Json_getInt(_decoration, "cost_coins", _vo.cost_coins);
            _vo.cost_gems        = (unsigned int)Json_getInt(_decoration, "cost_gems", _vo.cost_gems);
            _vo.in_shop          = Json_getInt(_decoration, "in_shop", _vo.in_shop);
            _vo.grid_col         = (unsigned int)Json_getInt(_decoration, "grid_col", _vo.grid_col);
            _vo.grid_row         = (unsigned int)Json_getInt(_decoration, "grid_row", _vo.grid_row);
            _vo.movable          = (bool)Json_getInt(_decoration, "movable", _vo.movable);
            _vo.src              = Json_getString(_decoration, "src", _vo.src.c_str());
            _vo.src_width        = (unsigned int)Json_getInt(_decoration, "src_width", _vo.src_width);
            _vo.src_height       = (unsigned int)Json_getInt(_decoration, "src_height", _vo.src_height);
            _vo.base_popularity  = (unsigned int)Json_getInt(_decoration, "base_popularity", _vo.src_height);
            _vo.unlocks_at_level = (unsigned int)Json_getInt(_decoration, "unlocks_at_level", _vo.unlocks_at_level);
            _vo.maximum          = Json_getInt(_decoration, "maximum", _vo.maximum);
            _vo.ground           = (bool)Json_getInt(_decoration, "ground", _vo.ground);
            Decoration.push_back(_vo);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Building Max Amount
    if (Json *_ablns = Json_getItem(_root, "building_limit_number")) {
        CCLog_("parseGameData -> BuildingLimitNumber");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("building_limit_number", json_);
        BuildingLimitNumber.clear();
        int _nabln = Json_getSize(_ablns);
        for (int i=0; i<_nabln; i++) {
            map<string, int> bln;
            if (Json *_blns = Json_getItemAt(_ablns, i)) {
                int _nbln = Json_getSize(_blns);
                for (int j=0; j<_nbln; j++) {
                    if (Json* _bln = Json_getItemAt(_blns, j)) {
                        string _key = _bln->name;
                        int _val = _bln->valueint;
                        bln[_key] = _val;
                    }
                    
                }
            }
            BuildingLimitNumber.push_back(bln);
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Game Start Bar
    if (Json *_game_start_bars = Json_getItem(_root, "game_start_bar")) {
        CCLog_("parseGameData -> GameStartBar");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("game_start_bar", json_);
        GameStartBar.clear();
        int _ngame_start_bar = Json_getSize(_game_start_bars);
        for (int i=0; i<_ngame_start_bar; i++) {
            
            GDGameStartBarVO _vo = GDGameStartBarVO();
            Json *_game_start_bar = Json_getItemAt(_game_start_bars, i);
            
            _vo.type            = Json_getString(_game_start_bar, "type", NULL);
            _vo.speed           = Json_getFloat(_game_start_bar, "speed", NULL);
            _vo.min_fans        = (unsigned int)Json_getInt(_game_start_bar, "min_fans", NULL);
            
            //GOOD START
            
            string _delimiter = ",", _delimiter2 = "-";
            size_t _pos = 0, _pos2 = 0;
            
            // parsing _game_start_bars
            string _temp_string_for_parsing = Json_getString(_game_start_bar, "good_start", NULL);
            _pos = 0;
            while ((_pos = _temp_string_for_parsing.find(_delimiter)) != string::npos) {
                string _good_starts = _temp_string_for_parsing.substr(0, _pos);
                _pos2 = 0;
                GSBRange _npe = GSBRange();
                while ((_pos2 = _good_starts.find(_delimiter2)) != string::npos) {
                    _npe.from = atof(_good_starts.substr(0, _pos2).c_str());
                    _good_starts.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.to = atof(_good_starts.c_str());
                _vo.good_start.push_back(_npe);
                
                _temp_string_for_parsing.erase(0, _pos + _delimiter.length());
            }
            if (_temp_string_for_parsing != "") {
                string _next_evolution = _temp_string_for_parsing;
                _pos2 = 0;
                GSBRange _npe = GSBRange();
                while ((_pos2 = _next_evolution.find(_delimiter2)) != string::npos) {
                    _npe.from = atof(_next_evolution.substr(0, _pos2).c_str());
                    _next_evolution.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.to = atof(_next_evolution.c_str());
                _vo.good_start.push_back(_npe);
            }
            
            //NORMAL START
            
            // parsing next_evolutions
            _temp_string_for_parsing = Json_getString(_game_start_bar, "normal_start", NULL);
            _pos = 0;
            while ((_pos = _temp_string_for_parsing.find(_delimiter)) != string::npos) {
                string _normal_starts = _temp_string_for_parsing.substr(0, _pos);
                _pos2 = 0;
                GSBRange _npe = GSBRange();
                while ((_pos2 = _normal_starts.find(_delimiter2)) != string::npos) {
                    _npe.from = atof(_normal_starts.substr(0, _pos2).c_str());
                    _normal_starts.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.to = atof(_normal_starts.c_str());
                _vo.normal_start.push_back(_npe);
                
                _temp_string_for_parsing.erase(0, _pos + _delimiter.length());
            }
            if (_temp_string_for_parsing != "") {
                string _next_evolution = _temp_string_for_parsing;
                _pos2 = 0;
                GSBRange _npe = GSBRange();
                while ((_pos2 = _next_evolution.find(_delimiter2)) != string::npos) {
                    _npe.from = atof(_next_evolution.substr(0, _pos2).c_str());
                    _next_evolution.erase(0, _pos2 + _delimiter2.length());
                }
                _npe.to = atof(_next_evolution.c_str());
                _vo.normal_start.push_back(_npe);
            }
            
            GameStartBar.push_back(_vo);
        }
        
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Map Village Return
    if (Json *_map_village_return = Json_getItem(_root, "map_village_return")) {
        CCLog_("parseGameData -> MapVillageReturn");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("map_village_return", json_);
        MapVillageReturn.clear();
        int _nmap_village_return = Json_getSize(_map_village_return);
        for (int i=0; i<_nmap_village_return; i++) {
            if (Json *_village_return = Json_getItemAt(_map_village_return, i)) {
                GDMapVillageReturnVO _vo = GDMapVillageReturnVO();
                _vo.number_of_village = Json_getInt(_village_return, "number_of_village", 0);
                _vo.return_percentage = Json_getFloat(_village_return, "return_percentage", 0.0f);
                MapVillageReturn.push_back(_vo);
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Parse Gem Exchange Rate
    if (Json *_gem_exchange_rate_json = Json_getItem(_root, "gem_exchange_rate")) {
        CCLog_("parseGameData -> GemExchangeRate");
        if (!from_local_) CCUserDefault::sharedUserDefault()->setStringForKey("gem_exchange_rate", json_);
        GemExchangeRate.clear();
        int n = Json_getSize(_gem_exchange_rate_json);
        for (int i=0; i<n; i++) {
            Json *_exchange_rate_json = Json_getItemAt(_gem_exchange_rate_json, i);
            GDGemExchangeRateVO _vo = GDGemExchangeRateVO();
            _vo.type = Json_getString(_exchange_rate_json, "type", _vo.type.c_str());
            _vo.ranges = Json_getInt(_exchange_rate_json, "ranges", _vo.ranges);
            _vo.gems = Json_getInt(_exchange_rate_json, "gems", _vo.gems);
            GemExchangeRate.push_back(_vo);
        }
    }

    Json_dispose(_root);

//    NR_TRACE_METHOD_STOP;
    
    if (getPercentageFinished() == 1.0f) {
        m_didParse = true;
        if (from_local_) {
            return true; // should
        }else {
//            BackendService::getInstance()->send(APIGameDataGet::create("unique_key",
//                                                                       CCDirector::sharedDirector()->getRunningScene(),
//                                                                       SceneBoost::onAllGameDatasRetrieved));
        }
    }
    
    return false;
    
}

float GameContentService::getPercentageFinished() {
    
    float count = 0.0f;
    float maxCount = 0.0f;
    
    if (GameConfig.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Word.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Cloud.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Achievement.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Inapp.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (ExperienceLevel.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Pet.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (FeedTimes.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (InitialPet.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Abilites.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Item.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (InitialItem.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Building.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Track.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (Decoration.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (BuildingLimitNumber.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (GameStartBar.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (MapVillageReturn.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    if (GemExchangeRate.size() > 0) {
        count += 1.0f;
    }
    maxCount += 1.0f;
    
    CCAssert(maxCount != 0.0f, "MaxCount must not be zero");
    
    return count/maxCount;
}

void GameContentService::_clearAll() {
    
    Achievement.clear();
    Inapp.clear();
    Cloud.clear();
    
    GameConfig.clear();
    Word.clear();
    ExperienceLevel.clear();
    Pet.clear();
    FeedTimes.clear();
    InitialPet.clear();
    Abilites.clear();
    AbilitiesOrder.clear();
    Building.clear();
    Track.clear();
    Decoration.clear();
    HQUpgrade.clear();
    for (vector<map<string, int> >::iterator it=BuildingLimitNumber.begin(); it!=BuildingLimitNumber.end(); ++it) {
        map<string, int> bln = (*it);
        bln.clear();
    }
    BuildingLimitNumber.clear();
    Item.clear();
    InitialItem.clear();
    MapVillageReturn.clear();
    
}

//bool GameContentService::_loadGameDataFromCache() {
//    
//    bool result = false;
//    string _list_json_str = CCUserDefault::sharedUserDefault()->getStringForKey("gamedata_list");
//    
//    if (Json *_root_json = Json_create(_list_json_str.c_str())) {
//        
//        if (Json *_list_json = Json_getItem(_root_json, "gamedata_list")) {
//            int n = Json_getSize(_list_json);
//            for (int i=0; i<n; i++) {
//                if (Json *_type = Json_getItemAt(_list_json, i)) {
//                    string _type_str = _type->valuestring;
//                    if (_type_str != "") {
//                        string _json_str = CCUserDefault::sharedUserDefault()->getStringForKey(_type_str.c_str());
//                        if (parseGameData(_json_str.c_str(), true)) {
//                            result = true;
//                        }
//                    }
//                }
//            }
//            
//        }
//        
//        Json_dispose(_root_json);
//    }
//    
//    return result;
//    
//}

GDAchievementsVO GameContentService::getAchievementVO(std::string type) {
    
    for (vector<GDAchievementsVO>::iterator it=Achievement.begin(); it!=Achievement.end(); ++it) {
        GDAchievementsVO vo = *it;
        if (vo.type != type) continue;
        return vo;
    }
    CCAssert(false, "Given type of AchievementVO does not exist!");
    return GDAchievementsVO();
    
}

GDExperienceLevelVO GameContentService::getNextExperienceLevelVO(int level_) {
    
    GDExperienceLevelVO _vo = ExperienceLevel[level_];
    if (_vo.level == level_+1) {
        return _vo;
    }
    return GDExperienceLevelVO();
    
}

GDPetVO GameContentService::getPetVO(string type_) {
    
    for (vector<GDPetVO>::iterator it=Pet.begin(); it!=Pet.end(); ++it) {
        GDPetVO _vo = (GDPetVO)(*it);
        if (_vo.type == type_) {
            return _vo;
        }
    }
    return GDPetVO();
    
}

GDBuildingVO GameContentService::getBuildingVO(string type_, int level_) {
    
    for (vector<GDBuildingVO>::iterator it=Building.begin(); it!=Building.end(); ++it) {
        GDBuildingVO _vo = (GDBuildingVO)(*it);
        if (_vo.type == type_ && _vo.level == level_) {
            return _vo;
        }
    }
    
    return GDBuildingVO();
    
}

int GameContentService::getBuildingMaxLevel(string type_) {
    
    int _maxlevel = 0;
    
    for (vector<GDBuildingVO>::iterator it=Building.begin(); it!=Building.end(); ++it) {
        GDBuildingVO _vo = (GDBuildingVO)(*it);
        if (_vo.type == type_) {
            if (_maxlevel < _vo.level) {
                _maxlevel = _vo.level;
            }
        }
    }
    
    return _maxlevel;
    
}

GDTrackVO GameContentService::getTrackVO(string type_) {
    
    for (vector<GDTrackVO>::iterator it=Track.begin(); it!=Track.end(); ++it) {
        GDTrackVO _vo = (GDTrackVO)(*it);
        if (_vo.type == type_) {
            return _vo;
        }
    }
    
    return GDTrackVO();
    
}

GDDecorationVO GameContentService::getDecorationVO(string type_) {
    
    for (vector<GDDecorationVO>::iterator it=Decoration.begin(); it!=Decoration.end(); ++it) {
        GDDecorationVO _vo = (GDDecorationVO)(*it);
        if (_vo.type == type_) {
            return _vo;
        }
    }
    
    return GDDecorationVO();
    
}

int GameContentService::getNDecorationVOUnlocksAtLevel(int level_) {
    
    int count = 0;
    for (vector<GDDecorationVO>::iterator it=Decoration.begin(); it!=Decoration.end(); ++it) {
        GDDecorationVO _vo = (GDDecorationVO)(*it);
        if (_vo.unlocks_at_level == level_) {
            count ++;
        }
    }
    
    return count;
    
}

int GameContentService::getNDecorationVOJustUnlocksAtLevel(int level_) {
    
    return getNDecorationVOUnlocksAtLevel(level_) - getNDecorationVOUnlocksAtLevel(level_-1);
    
}

int GameContentService::getBuildingMax(string type_, int level_) {
   
    if (BuildingLimitNumber.size() >= level_) {
        map<string, int> bln = BuildingLimitNumber[level_-1];
        if (bln[type_]) {
            return bln[type_];
        }
    }
    
    return 0; //undefine type return 0 as default
    
}

GDItemVO GameContentService::getItemVO(string type_) {
    
    //prevent game crash with whitespace
    type_ = StringHelper::trim(type_);
    
    for (vector<GDItemVO>::iterator it=Item.begin(); it!=Item.end(); ++it) {
        GDItemVO _vo = (GDItemVO)(*it);
        if (_vo.type == type_ && _vo.type != "") {
            return _vo;
        }
    }
    
    return GDItemVO();
    
}

string GameContentService::getRandomItemExcept(const char *operator_, int rarity_, std::vector<string> except_items_) {
    
    string output = "yarn";
    int n = Item.size();
    
    if (n > 0) {
        int j = rand()%n; //random index
        int i = 0; //count
        while (i < n) {
            i++;
            GDItemVO _item = Item[j];
            if (_item.type == "") {
                j++; if (j >= n) j = 0;
                continue;
            }
            if (strcmp(operator_, ">") == 0) {
                if (_item.rarity_point <= rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }else if (strcmp(operator_, "<") == 0) {
                if (_item.rarity_point >= rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }else if (strcmp(operator_, ">=") == 0) {
                if (_item.rarity_point < rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }else if (strcmp(operator_, "<=") == 0) {
                if (_item.rarity_point > rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }else if (strcmp(operator_, "!=") == 0) {
                if (_item.rarity_point == rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }else if (strcmp(operator_, "==") == 0) {
                if (_item.rarity_point != rarity_) {
                    j++; if (j >= n) j = 0;
                    continue;
                }
            }
            for (vector<string>::iterator it=except_items_.begin(); it!=except_items_.end(); ++it) {
                string _key = *it;
                if (_key == _item.type) {
                    j++;if (j >= n) j = 0;
                    continue;
                }
            }
            output = _item.type;
            break;
        }
    }
    
    return output;
    
}

GDInitialItemVO GameContentService::getInitialItemVO(std::string type_) {
    
    for (vector<GDInitialItemVO>::iterator it=InitialItem.begin(); it != InitialItem.end(); ++it) {
        GDInitialItemVO _iivo = (GDInitialItemVO)(*it);
        if (_iivo.type == type_ && _iivo.type != "") {
            return _iivo;
        }
    }
    
    return GDInitialItemVO();
    
}

int GameContentService::getNTrackVOUnlocksAtLevel(int level_) {
    
    int _count = 0;
    for (vector<GDTrackVO>::iterator it=Track.begin(); it!=Track.end(); ++it) {
        GDTrackVO _tvo = (GDTrackVO)(*it);
        if (_tvo.type != "" && _tvo.track_type == "track" && _tvo.unlocks_at_level == level_) {
            _count ++;
        }
    }
    return _count;
}

GDTrackVO GameContentService::getTrackVOUnlocksAtLevel(int level_) {
    
    return getTrackVOUnlocksAtLevel(level_, 0);
    
}

GDTrackVO GameContentService::getTrackVOUnlocksAtLevel(int level_, int index_) {
    
    int _count = 0;
    for (vector<GDTrackVO>::iterator it=Track.begin(); it!=Track.end(); ++it) {
        GDTrackVO _tvo = (GDTrackVO)(*it);
        if (_tvo.type != "" && _tvo.track_type == "track" && _tvo.unlocks_at_level == level_) {
            if (_count == index_) {
                return _tvo;
            }
            _count++;
        }
    }
    return GDTrackVO();
    
}

int GameContentService::getNAbilitiesUnlocksAtLevel(int level_, bool group_) {

    int count1 = 0;
    map<string, int> count2;
    
    for (vector<string>::iterator it=AbilitiesOrder.begin(); it!=AbilitiesOrder.end(); ++it) {
        string _type = *it;
        for (vector<GDAbilityVO>::iterator it2=Abilites[*it].begin(); it2!=Abilites[*it].end(); ++it2) {
            GDAbilityVO _avo = *it2;
            if (_avo.type != "" && level_ >= _avo.unlocks_at_level) {
                count1 ++;
                count2[_avo.type] ++;
            }
        }
    }
    
    if (group_) {
        return (int)count2.size();
    }
    
    return count1;
    
}

float GameContentService::getMapVillageReturnPercentage(int number_of_village_) {
    
    for (vector<GDMapVillageReturnVO>::reverse_iterator it=MapVillageReturn.rbegin(); it!=MapVillageReturn.rend(); ++it) {
        GDMapVillageReturnVO _vo = *it;
        if (number_of_village_ >= _vo.number_of_village) {
            return _vo.return_percentage;
        }
    }
    
    return 0.0f;
    
}

GDAbilityVO GameContentService::getAbilityVO(std::string name_of_ability_, int level_) {
    
    return Abilites[name_of_ability_][level_-1];
    
}


void GameContentService::getUnlockVillTable(int *unlockVillTable) {
    
    

    unlockVillTable[0] = 0; //home
    unlockVillTable[1] = 9;
    unlockVillTable[2] = 48;
    unlockVillTable[3] = 67;
    unlockVillTable[4] = 69;
    unlockVillTable[5] = 19;
    unlockVillTable[6] = 38;
    unlockVillTable[7] = 17;
    unlockVillTable[8] = 15;
    unlockVillTable[9] = 25;
    
    unlockVillTable[10] = 120;
    unlockVillTable[11] = 116;
    unlockVillTable[12] = 97;
    unlockVillTable[13] = 44;
    unlockVillTable[14] = 33;
    unlockVillTable[15] = 11;
    unlockVillTable[16] = 13;
    unlockVillTable[17] = 32;
    unlockVillTable[18] = 105;
    unlockVillTable[19] = 62;
    
    unlockVillTable[20] = 52;
    unlockVillTable[21] = 34;
    unlockVillTable[22] = 84;
    unlockVillTable[23] = 141;
    unlockVillTable[24] = 138;
    unlockVillTable[25] = 77;
    unlockVillTable[26] = 28;
    unlockVillTable[27] = 58;
    unlockVillTable[28] = 57;
    unlockVillTable[29] = 88;
    
    unlockVillTable[30] = 105;
    unlockVillTable[31] = 118;
    unlockVillTable[32] = 37;
    unlockVillTable[33] = 39;
    unlockVillTable[34] = 61;
    unlockVillTable[35] = 65;
    unlockVillTable[36] = 115;
    unlockVillTable[37] = 74;
    unlockVillTable[38] = 71;
    unlockVillTable[39] = 95;
    
    unlockVillTable[40] = 108;
    unlockVillTable[41] = 151;
    unlockVillTable[42] = 23;
    unlockVillTable[43] = 164;
    unlockVillTable[44] = 80;
    unlockVillTable[45] = 42;
    unlockVillTable[46] = 54;
    unlockVillTable[47] = 55;
    unlockVillTable[48] = 31;
    
    
    //---------------------------
    unlockVillTable[49] = 85;
    
    unlockVillTable[50] = 64;
    unlockVillTable[51] = 99;
    unlockVillTable[52] = 66;
    unlockVillTable[53] = 149;
    unlockVillTable[54] = 65;
    unlockVillTable[55] = 142;
    unlockVillTable[56] = 190;
    unlockVillTable[57] = 94;
    unlockVillTable[58] = 189;
    unlockVillTable[59] = 250;
    
    unlockVillTable[60] = 119;
    unlockVillTable[61] = 53;
    unlockVillTable[62] = 30;
    unlockVillTable[63] = 87;
    unlockVillTable[64] = 192;
    unlockVillTable[65] = 93;
    unlockVillTable[66] = 128;
    unlockVillTable[67] = 127;
    unlockVillTable[68] = 175;
    unlockVillTable[69] = 126;
    
    unlockVillTable[70] = 125;
    unlockVillTable[71] = 233;
    unlockVillTable[72] = 299;
    unlockVillTable[73] = 234;
    unlockVillTable[74] = 300;
    unlockVillTable[75] = 235;
    unlockVillTable[76] = 129;
    unlockVillTable[77] = 57;
    unlockVillTable[78] = 14;
    unlockVillTable[79] = 188;
    
    unlockVillTable[80] = 316;
    unlockVillTable[81] = 314;
    unlockVillTable[82] = 390;
    unlockVillTable[83] = 315;
    unlockVillTable[84] = 186;
    unlockVillTable[85] = 246;
    unlockVillTable[86] = 246;
    unlockVillTable[87] = 134;
    unlockVillTable[88] = 92;
    unlockVillTable[89] = 91;
    
    unlockVillTable[90] = 132;
    unlockVillTable[91] = 179;
    unlockVillTable[92] = 180;
    unlockVillTable[93] = 297;
    unlockVillTable[94] = 233;
    unlockVillTable[95] = 300;
    unlockVillTable[96] = 372;
    unlockVillTable[97] = 232;
    unlockVillTable[98] = 174;
    unlockVillTable[99] = 124;
    
    unlockVillTable[100] = 165;
    unlockVillTable[101] = 220;
    unlockVillTable[102] = 218;
    unlockVillTable[103] = 279;
    unlockVillTable[104] = 213;
    unlockVillTable[105] = 156;
    unlockVillTable[106] = 269;
    unlockVillTable[107] = 161;
    unlockVillTable[108] = 217;
    unlockVillTable[109] = 160;
    
    unlockVillTable[110] = 162;
    unlockVillTable[111] = 110;
    unlockVillTable[112] = 154;
    unlockVillTable[113] = 267;
    unlockVillTable[114] = 336;
    unlockVillTable[115] = 333;
    unlockVillTable[116] = 202;
    unlockVillTable[117] = 156;
    unlockVillTable[118] = 215;
    unlockVillTable[119] = 159;
    
    unlockVillTable[120] = 162;
    unlockVillTable[121] = 281;
    unlockVillTable[122] = 353;
    unlockVillTable[123] = 350;
    unlockVillTable[124] = 209;
    unlockVillTable[125] = 497;
    unlockVillTable[126] = 265;
    unlockVillTable[127] = 204;
    unlockVillTable[128] = 100;
    unlockVillTable[129] = 249;
    
    unlockVillTable[130] = 237;
    unlockVillTable[131] = 206;
    unlockVillTable[132] = 115;
    unlockVillTable[133] = 147;
    unlockVillTable[134] = 124;
    unlockVillTable[135] = 83;
    unlockVillTable[136] = 50;
    unlockVillTable[137] = 245;
    unlockVillTable[138] = 73;
    unlockVillTable[139] = 204;
    
    unlockVillTable[140] = 103;
    unlockVillTable[141] = 430;
    unlockVillTable[142] = 208;
    unlockVillTable[143] = 154;
    unlockVillTable[144] = 69;
    unlockVillTable[145] = 112;
    unlockVillTable[146] = 215;
    unlockVillTable[147] = 82;
    unlockVillTable[148] = 142;
    unlockVillTable[149] = 136;
    
    unlockVillTable[150] = 90;
    unlockVillTable[151] = 371;
    unlockVillTable[152] = 42;
    unlockVillTable[153] = 21;
    unlockVillTable[154] = 75;
}

#pragma mark - Calculation functions

int GameContentService::_calcGemPrice(int amount_, std::string type_) {
    
    if (amount_ < 0) {
        CCLog_("Error: Amount can't be less than 0");
        return 0;
    }
    
    if (amount_ == 0) {
        CCLog_("Warning: Amount shouldn't be 0");
        return 0;
    }
    
    for (vector<GDGemExchangeRateVO>::iterator it=GemExchangeRate.begin(); it!=GemExchangeRate.end(); ++it) {
        GDGemExchangeRateVO _vo = *it;
        if (_vo.type != type_) continue;
        if (amount_ <= _vo.ranges) {
            return _vo.gems;
        }
        break;
    }
    
    for (vector<GDGemExchangeRateVO>::reverse_iterator it=GemExchangeRate.rbegin(); it!=GemExchangeRate.rend(); ++it) {
        GDGemExchangeRateVO _vo = *it;
        if (_vo.type != type_) continue;
        if (amount_ >= _vo.ranges) {
            return _vo.gems;
        }
        break;
    }
    
    int n = GemExchangeRate.size();
    for (int i=0; i<n; i++) {
        
        if (GemExchangeRate[i].type != type_) continue;
        
        GDGemExchangeRateVO next_rate = GemExchangeRate[i + 1];
        if (amount_ > next_rate.ranges) continue;
        
        GDGemExchangeRateVO current_rate = GemExchangeRate[i];
    
        //IMPORTANT: must use double instead of int in calculaion below otherwise it's gonna unmatch with server (node.js)
        double resource_min = (amount_ - current_rate.ranges);//900 - 100 = 800
        double resource_range = (next_rate.ranges - current_rate.ranges); //1000 - 100 = 900
        double gem_range = (next_rate.gems - current_rate.gems); //5 - 1 =  4
        
        return round(resource_min / (resource_range / gem_range) + (double)current_rate.gems);
        
    }
    
    CCLog_("Error: calculation._calcGemPrice failed!");
    return 0;
    
}

int GameContentService::get_gemcal_for_coins(int coins_) {
    return _calcGemPrice(coins_ * GameConfig["GEM_PRICE_COIN_MULTIPLIER"], "resource");
}

int GameContentService::get_gemcal_for_foods(int foods_) {
    return _calcGemPrice(foods_ * GameConfig["GEM_PRICE_FOOD_MULTIPLIER"], "resource");
}

int GameContentService::get_gemcal_for_building_finishnow(int seconds_) {
    return _calcGemPrice(seconds_ * GameConfig["GEM_PRICE_FINISHNOW_BUILDING_MULTIPLIER"], "time");
}

int GameContentService::get_gemcal_for_ability_finishnow(int seconds_) {
    return _calcGemPrice(seconds_ * GameConfig["GEM_PRICE_FINISHNOW_ABILITY_MULTIPLIER"], "time");
}

int GameContentService::get_gemcal_for_pet_wakeup(int seconds_) {
    return _calcGemPrice(seconds_ * GameConfig["GEM_PRICE_WAKEUP_MULTIPLIER"], "time");
}

int GameContentService::get_gemcal_for_pet_sell(int sell_value, int level) {
    return int(sell_value * (GameConfig["ANIMAL_SELL_PARAM_1"] + (level * GameConfig["ANIMAL_SELL_PARAM_2"])));
}

int GameContentService::cal_instant_building_cost(GDBuildingVO bvo_) {
    
    int _cost_to_build = get_gemcal_for_coins(bvo_.cost_coins);
    int _cost_to_finish = get_gemcal_for_building_finishnow(bvo_.time);
    int _cost_in_total = _cost_to_build + _cost_to_finish;
    
    return _cost_in_total;
    
}

int GameContentService::cal_coins_reward(int garage_level_, int coins_reward_, int win_streak_) {

    int _coins_reward = coins_reward_;
    float _bonus = 1.0f;
    
    int _garage_level = garage_level_;
    GDBuildingVO _garage_vo = garage_level_==0?GDBuildingVO():getBuildingVO(kBuildingGarage, garage_level_);
    float win_streak_bonus_base = GameConfig["WINNING_STREAK_BONUS_PERCENTAGE_BASE"];
    float win_streak_bonus_max = GameConfig["WINNING_STREAK_BONUS_PERCENTAGE_MAX_DEFAULT"];
    if (_garage_vo.type != "" && _garage_level > 0) win_streak_bonus_max = _garage_vo.param_4;

    _bonus = 1 + (win_streak_bonus_base * (win_streak_ - 1));
    if (_bonus > win_streak_bonus_max) _bonus = win_streak_bonus_max;
    _bonus = round(_bonus*100.0)*0.01;
    if (_bonus < 1) _bonus = 1;
    _coins_reward = (int)(_coins_reward * _bonus);

    return _coins_reward;
    
}

int GameContentService::cal_fans_won(bool oppo_is_bot_) {
    
    if (oppo_is_bot_) {
        return GameConfig["WON_TOWN_FROM_AI"];
    }
    return GameConfig["WON_TOWN_FROM_PLAYER"];
    
}

int GameContentService::cal_round_to_nearest_5(float input) {
    return (roundf(((input * 2.0f) * 0.1f)) * 10.0f) * 0.5f;
}

float GameContentService::cal_round_to_nearest_half(float input) {
    return roundf((input * 2.0f)) * 0.5f;
}

void GameContentService::cal_helper_horsepower_to_stun_time(float horsepower, float *timeTotal, float *times, float *timeEach) {
    
    float min1 = 1.6f; //at least
    float min2 = 1.6f;//2.0f; //at least for more than 1 time
    float a = 2.0f;//1.0f; //minimum time
    float b = 0.3f;//0.15f; //+ increasing time
    float max = 4.0f; //if above this value, divided by 2
    
    *timeTotal = a + (b * floorf(horsepower * 0.1f));
    
    if (*timeTotal >= max) {
        *timeTotal = (*timeTotal - (*timeTotal - max)) + ((*timeTotal - max) * 0.5f);
    }
    
    if (*timeTotal >= (min2 * 2.0f)) {
        *times = 2.0f;
        *timeEach = *timeTotal / *times;
    } else {
        *times = 1.0f;
        *timeEach = *timeTotal;
    }
    
    //round to 0.x
    *timeTotal = roundf(*timeTotal * 10.0f) * 0.1f;
    *timeEach = roundf(*timeEach * 10.0f) * 0.1f;
    
    if (*timeEach <= min1) *timeEach = min1;
    if (*timeTotal <= min1) *timeTotal = min1;
}
