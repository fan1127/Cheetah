//
//  GameAudioService.cpp
//  Cheetah
//
//  Created by Teerapat on 28/3/57 BE.
//
//

#include "cocos2d.h"
USING_NS_CC;
#include "GameAudioService.h"
#include "../AppMacros.h"

//#define DISABLE_AUDIO_FOR_DEBUG

GameAudioService::GameAudioService() {
    
    m_sound_on = !CCUserDefault::sharedUserDefault()->getBoolForKey(kSettingSoundOff);
    m_music_on = !CCUserDefault::sharedUserDefault()->getBoolForKey(kSettingMusicOff);

#if IS_DEBUG_BUILD
#ifdef DISABLE_AUDIO_FOR_DEBUG
    m_sound_on = false;
    m_music_on = false;
#endif
#endif
}

GameAudioService::~GameAudioService()
{
    this->unloadAllEffect();
}

void GameAudioService::preloadEffects(PreloadEffectType scene)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    // no need to preload on ios

    return;
#endif

    switch (scene)
    {
        case Generic:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_TYPING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ERROR);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_UI_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GENERAL_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOSE_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ABILITY_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_TRACK_CLICK);
        }
        break;

        case TutorialRace:
        {
            preloadEffects(GameAudioService::Race);
        }
        break;

        case TutorialVillage:
        {
            preloadEffects(GameAudioService::Village);
        }
        break;

        case Village:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_COLLECT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CHEST_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FIREWORK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_COIN_HIT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FOOD_HIT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MOVE_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CUT_TREE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_UNDERCONSTRUCTION);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FINISH_NOW);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FINISH_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MAGIC_STAR);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RANCH_LEVEL_UP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MERCHANT_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MERCHANT_BUY);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FEED_ANIMAL);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FOOD_PRODUCTION);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ANIMAL_PUSH);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_DROP_ANIMAL_TO_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FUSE_EVOLVE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FUSE_EVOLVE_2);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_PET_LEVEL_UP);
        }
        break;

        case Map:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MAP_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOUD_CLOSE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOUD_OPEN);
        }
        break;

        case Race:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_POWER_UP_01);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_POWER_UP_02);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ENERGY_REGEN);

            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_CHEERS);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN1);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN2);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_LOSE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_JUMP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_START);

            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_SPEED);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_BANANA);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_STOMP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_WAVE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_WAVE_FLY);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_HIT_FALL);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SLIP_BANANA);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_CHEERS);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN1);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN2);
        }
        break;

        case Gacha:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_PRIZE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_TICK);
        }
        break;

        default:
        {
            CCAssert(false, "Wrong effect to preload!");
        }
        break;
    }
}

void GameAudioService::unloadEffects(PreloadEffectType scene)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

    // no need to preload on ios

    return;
#endif

    switch (scene)
    {
        case Generic:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_NONE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_TYPING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ERROR);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_UI_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GENERAL_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOSE_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ABILITY_CLICK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_TRACK_CLICK);
        }
        break;

        case TutorialRace:
        {
            unloadEffects(GameAudioService::Race);
        }
        break;

        case TutorialVillage:
        {
            unloadEffects(GameAudioService::Village);
        }
        break;

        case Village:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_COLLECT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CHEST_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FIREWORK);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_COIN_HIT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FOOD_HIT);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MOVE_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CUT_TREE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_UNDERCONSTRUCTION);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FINISH_NOW);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FINISH_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MAGIC_STAR);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RANCH_LEVEL_UP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MERCHANT_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MERCHANT_BUY);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FEED_ANIMAL);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FOOD_PRODUCTION);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ANIMAL_PUSH);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_DROP_ANIMAL_TO_BUILDING);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FUSE_EVOLVE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FUSE_EVOLVE_2);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_PET_LEVEL_UP);
        }
        break;

        case Map:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MAP_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOUD_CLOSE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOUD_OPEN);
        }
        break;

        case Race:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_POWER_UP_01);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_POWER_UP_02);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ENERGY_REGEN);

            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_CHEERS);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN1);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN2);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_LOSE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_JUMP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_START);

            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_SPEED);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_BANANA);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_STOMP);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_WAVE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_WAVE_FLY);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_HIT_FALL);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SLIP_BANANA);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_CHEERS);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN1);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN2);
        }
        break;

        case Gacha:
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_OPEN);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_PRIZE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_TICK);
        }
        break;

        default:
        {
            CCAssert(false, "Wrong effect to preload!");
        }
        break;
    }
}

void GameAudioService::preloadAllEffect()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_AMBIENCE_FARM);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FOOD_PRODUCTION);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_COLLECT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_TRACK_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_UI_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GENERAL_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOSE_CLICK);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ABILITY_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ENERGY_REGEN);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_HIT);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FEED_ANIMAL);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_POWER_UP_01);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_POWER_UP_02);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_PIG_GRUNT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CUT_TREE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_UNDERCONSTRUCTION);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FINISH_NOW);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_COIN_HIT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FOOD_HIT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_ANIMAL_PUSH);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CHEST_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FIREWORK);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_SPEED);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_BANANA);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_STOMP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_WAVE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SKILL_WAVE_FLY);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_HIT_FALL);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_SLIP_BANANA);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_CHEERS);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN1);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_WIN2);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_LOSE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_JUMP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RACE_START);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MAP_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MOVE_BUILDING);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FINISH_BUILDING);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MAGIC_STAR);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_RANCH_LEVEL_UP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MERCHANT_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_MERCHANT_BUY);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_PET_LEVEL_UP);

//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_OPEN);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_PRIZE);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_GACHA_TICK);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOUD_CLOSE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_CLOUD_OPEN);
    
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FUSE_EVOLVE);
//    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(AUDIO_FUSE_EVOLVE_2);
}
                                  
void GameAudioService::unloadAllEffect() {
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_AMBIENCE_FARM);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FOOD_PRODUCTION);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_COLLECT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_TRACK_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_UI_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GENERAL_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOSE_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ABILITY_CLICK);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ENERGY_REGEN);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_HIT);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FEED_ANIMAL);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_POWER_UP_01);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_POWER_UP_02);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_PIG_GRUNT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CUT_TREE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_UNDERCONSTRUCTION);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FINISH_NOW);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_COIN_HIT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FOOD_HIT);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_ANIMAL_PUSH);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CHEST_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FIREWORK);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_SPEED);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_BANANA);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_STOMP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_WAVE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SKILL_WAVE_FLY);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_HIT_FALL);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_SLIP_BANANA);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_CHEERS);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN1);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_WIN2);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_LOSE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_JUMP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RACE_START);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MAP_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MOVE_BUILDING);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FINISH_BUILDING);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MAGIC_STAR);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_RANCH_LEVEL_UP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_PET_LEVEL_UP);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MERCHANT_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_MERCHANT_BUY);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_OPEN);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_PRIZE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_GACHA_TICK);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOUD_CLOSE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_CLOUD_OPEN);
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FUSE_EVOLVE);
    CocosDenshion::SimpleAudioEngine::sharedEngine()->unloadEffect(AUDIO_FUSE_EVOLVE_2);
    
}

void GameAudioService::playEffectFadeOutAndFadeIn(const char *audio_name_, bool playLoop, float fadeTime) {
    
    if (m_sound_on) {
        
        CCString ambianceFile = audio_name_;
        if(!m_music_on && ambianceFile.compare(AUDIO_AMBIENCE_FARM) == 0) {
            
        } else {
            isUpdate = true;
            stepFadeOut=-CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()/(60.0*4.0);
            stateFade = 3;
            _audio_name_ = audio_name_;
            _Loop = playLoop;
            _fadeTime = fadeTime;
        }
        
    }
    
}

unsigned int GameAudioService::playEffectFadeIn(const char *audio_name_, bool Loop, float fadeTime) {
    
    if(m_sound_on) {
        
        CCString ambianceFile = audio_name_;
        if(!m_music_on && ambianceFile.compare(AUDIO_AMBIENCE_FARM) == 0) {
            return 0;
        }
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0f);
        unsigned int playBackID = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(audio_name_,Loop);
        
        isUpdate = true;
        stepFadeIn = 1.0/(fadeTime*60.0);
        stateFade = 1;
        
        return playBackID;
    }
    
    return 0;
    
}

void GameAudioService::clearMusicFadeOut(float fadeTime) {
    
    isUpdate = true;
    stepMusicFadeOut = -CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()/(float)(fadeTime*30.0);
    stateMusicFade = 2;
    
//    CCLog_("Step music fade out = %f",stepMusicFadeOut);
}

void GameAudioService::clearEffectFadeOut(float fadeTime) {
    
    isUpdate = true;
    stepFadeOut = -CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()/(fadeTime*60.0);
    
    stateFade = 2;
    
}

void GameAudioService::stopEffect(unsigned int nSoundId) {
    
    stopEffect(nSoundId);
//    nSoundId=0;
    
}

void GameAudioService::resumeEffects()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

void GameAudioService::pauseEffects()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

void GameAudioService::stopAllEffect() {
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
    CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(1.0f);
    
    stateFade=0;
    stepFadeOut=0;
    stepFadeIn=0;
    if(stepMusicFadeOut==0) {
        isUpdate=false;
    }
        
}

void GameAudioService::setEffectsVolume(float volume) {
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(volume);
}

void GameAudioService::update() {
 
    if(isUpdate) {
        
        if(m_sound_on) {
            if(stateFade==1) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()<VOLUME_MAX_SFX) {
                    if (stepFadeIn != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()+stepFadeIn);
                    }
                } else {
                    stepFadeIn=0;
                    if(stepMusicFadeOut==0) isUpdate=false;
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(VOLUME_MAX_SFX);
                }
            } else if(stateFade==2) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()>0) {
                    if (stepFadeOut != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()+stepFadeOut);
                    }
                } else {
                    stepFadeOut=0;
                    if(stepMusicFadeOut==0) isUpdate=false;
                    
                    stopAllEffect();
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(VOLUME_MAX_SFX);
                }
            }
            else if(stateFade==3) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()>0) {
                    if (stepFadeOut != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getEffectsVolume()+stepFadeOut);
                    }
                } else {
                    stopAllEffect();
                    playEffectFadeIn(_audio_name_,_Loop,_fadeTime);
                }
            }
        }

        
        if(m_music_on) {
            if(stateMusicFade==1) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()<VOLUME_MAX_MUSIC) {
                    if (stepMusicFadeIn != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()+stepMusicFadeIn);
                    }
                } else {
                    stepMusicFadeIn=0;
                    if(stateFade==0) isUpdate=false;
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(VOLUME_MAX_MUSIC);
                }
            } else if(stateMusicFade==2) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()>0) {
                    if (stepMusicFadeOut != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()+stepMusicFadeOut);
                    }
                } else {
                    stepMusicFadeOut=0;
                    if(stateFade==0) isUpdate=false;
                    stopMusic();
                    CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(VOLUME_MAX_MUSIC);
                }
            } else if(stateMusicFade==3) {
                if(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()>0) {
                    if (stepMusicFadeOut != 0)
                    {
                        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()+stepMusicFadeOut);
                    }
                } else {
                    if(_delayMusic>0) {
                        _delayMusic-=(0.3/60.0);
                    } else {
                        playMusicFadeIn(_audio_name_Music,_LoopMusic,_fadeTimeMusic);
                    }
                }
            }
        }
        
    }
    
}

unsigned int GameAudioService::playEffect(const char *audio_name_, bool Loop) {
    
    if (m_sound_on) {
        
        CCString ambianceFile = audio_name_;
        if(!m_music_on && ambianceFile.compare(AUDIO_AMBIENCE_FARM)==0) {
            return 0;
        }
        
        unsigned int playBackID = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(audio_name_,Loop);
        return playBackID;
    
    }
    
    return 0;
    
}

unsigned int GameAudioService::playEffect(const char *audio_name_) {
    
    if(m_sound_on) {
        
        CCString ambianceFile = audio_name_;
        if(!m_music_on && ambianceFile.compare(AUDIO_AMBIENCE_FARM)==0) {
            return 0;
        }
        unsigned int playBackID = CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(audio_name_);
        return playBackID;
    
    }
    
    return 0;
    
}

//================= Music =============

void GameAudioService::playMusicFadeOutAndDelayBeforeFadeIn(const char *audio_name_, bool Loop, float fadeOut, float delay, float fadeIn) {
    
    if(m_music_on) {
        
        isUpdate = true;
        stepMusicFadeOut=-CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()/(60.0*fadeOut);
        
        stateMusicFade = 3;
        
        _audio_name_Music = audio_name_;
        _LoopMusic = Loop;
        _fadeTimeMusic = fadeIn;
        _delayMusic = delay;
    
    }
    
}


void GameAudioService::playMusicFadeOutAndFadeIn(const char *audio_name_, bool Loop, float fadeTime) {
    
    if(m_music_on) {
        
        isUpdate = true;
        stepMusicFadeOut=-CocosDenshion::SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume()/(60.0*3.0);
        
        stateMusicFade = 3;
        _delayMusic=0;
        
        _audio_name_Music = audio_name_;
        _LoopMusic = Loop;
        _fadeTimeMusic = fadeTime;
        
    }
    
}

void GameAudioService::playMusicFadeIn(const char *audio_name_, bool Loop, float fadeTime) {

    if(m_music_on) {
    
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(audio_name_, Loop);
        
        isUpdate = true;
        stepMusicFadeIn = 1.0/(fadeTime*60.0);
        stateMusicFade = 1;
        _delayMusic=0;
    
    }
    
}

void GameAudioService::playMusicFadeIn(const char *audio_name_, float fadeTime) {
    
    if(m_music_on) {
        
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(audio_name_, true);
        
        isUpdate = true;
        stepMusicFadeIn = 1.0/(fadeTime*60.0);
        stateMusicFade = 1;
        _delayMusic=0;
        
    }
    
}

void GameAudioService::playMusic(const char *audio_name_, bool Loop) {
    
    if(m_music_on) {
        _delayMusic=0;
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(audio_name_, Loop);
    }
    
}

void GameAudioService::playMusic(const char *audio_name_) {
    
    if(m_music_on) {
        _delayMusic=0;
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(audio_name_, true);
    }
    
}

void GameAudioService::stopMusic() {
    
    _delayMusic=0;
    stateMusicFade=0;
    stepMusicFadeOut=0;
    stepMusicFadeIn=0;
    
    if(stateFade==0) isUpdate=false;
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    
}

//===============================

void GameAudioService::updateAudioSetting() {
    
    if(m_sound_on) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(VOLUME_MAX_SFX);
    } else {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopAllEffects();
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.0f);
    }
    
    if(m_music_on) {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(VOLUME_MAX_MUSIC);
    } else {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
        CocosDenshion::SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.0f);
    }
    
    CCUserDefault::sharedUserDefault()->setBoolForKey("sound_off", !m_sound_on);
    CCUserDefault::sharedUserDefault()->setBoolForKey("music_off", !m_music_on);
    
}

void GameAudioService::toggleSound() {
    
    m_sound_on = !m_sound_on;
    
}

void GameAudioService::toggleMusic() {
    
    m_music_on = !m_music_on;
    
}

bool GameAudioService::isSoundOn() {
    
    return m_sound_on;
    
}

bool GameAudioService::isMusicOn() {
    
    return m_music_on;
    
}
