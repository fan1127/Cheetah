//
//  GameAudioService.h
//  Cheetah
//
//  Created by Teerapat on 28/3/57 BE.
//
//

#ifndef __Cheetah__GameAudioService__
#define __Cheetah__GameAudioService__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"


#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#define AUDIO_EXT ".m4a"

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#define AUDIO_EXT ".ogg"

#endif

#define AUDIO_FILE(name, ext) name "" ext

/****************************** Former Audio Variable (obsolete) *****************************************
#define AUDIO_TRACK_CLICK       AUDIO_FILE("sfx/Organic Nav 04.ogg" //Open Shop Panel
#define AUDIO_UI_CLICK          AUDIO_FILE("sfx/Pop 22.ogg" //Click/Tap sound (1) for most buttons
#define AUDIO_GENERAL_CLICK     AUDIO_FILE("sfx/General Click 01.ogg" //Click/Tap sound (2) for most buttons, and when tapping most buildings
#define AUDIO_CLOSE_CLICK       AUDIO_FILE("sfx/close_click2.mp3" //Click/Tap sound when tapping to close all dialogs in the game

#define AUDIO_ABILITY_CLICK     AUDIO_FILE("sfx/agloob01.ogg" //Click/Tap sound when tapping on the ability (Sound plays as soon as finger touches, without waiting to release the finger)
#define AUDIO_ENERGY_REGEN      AUDIO_FILE("sfx/race/powerup_recharge.ogg" //When your pet completes a lap, pet energy regens, and this sound is played

#define AUDIO_NONE              ""
#define AUDIO_TYPING            AUDIO_FILE("sfx/typing.ogg" //Sound when player types their name of the ranch
#define AUDIO_ERROR             AUDIO_FILE("sfx/error.ogg" //During the tutorial, player has to tap Go button to start the race on the green area, this sound is played if player tap the Go button but not on the green area. When this happens, player has to tap the Go button again until it hits the green area.

#define AUDIO_COLLECT           AUDIO_FILE("sfx/vill/Chime Flourish 02.ogg" //Sound when collecting coins from Ticket Booth or collecting food from Feed Mills
#define AUDIO_CUT_TREE          AUDIO_FILE("sfx/vill/foliage tree branch movement 01.ogg" //Sound when cutting a tree
#define AUDIO_FINISH_NOW        AUDIO_FILE("sfx/vill/magical sweep 03.ogg" //Sound when using Finish Now or Instant Build options
#define AUDIO_COIN_HIT          AUDIO_FILE("sfx/vill/dinner bell 03, ringing.ogg" //Sound when claiming Gems from Achievements and other places, sound when gems are added to UI (from purchasing IAP, and others)
#define AUDIO_FOOD_HIT          AUDIO_FILE("sfx/vill/punching bag a hit bare fist 10.ogg" //Sound when Food bags are added to UI

#define AUDIO_FOOD_PRODUCTION   AUDIO_FILE("sfx/building/Cereal Pour 02.ogg" //Sound when tapping Feed Mills
#define AUDIO_UNDERCONSTRUCTION AUDIO_FILE("sfx/building/hammering nail 09.ogg" //Sound when tapping buildings that are under construction
#define AUDIO_MOVE_BUILDING     AUDIO_FILE("sfx/building/moving_building.ogg" //Sound when player move buildings. (Plays once per tile moved)
#define AUDIO_FINISH_BUILDING   AUDIO_FILE("sfx/building/magic_finish.ogg" //Sound tapping to open building cover when buildings are completed (When building completes construction, it becomes covered at first. Player tap the cover to open up the cover and reveal the building.)
#define AUDIO_MAGIC_STAR        AUDIO_FILE("sfx/building/magic_star.ogg" //This sound is played during the animation when building opens up its cover
#define AUDIO_DROP_ANIMAL_TO_BUILDING AUDIO_FILE("sfx/building/drop animal into barn.ogg" //This sound is played as animals are sent to barn. (Animation in the game shows an icon of the animal dropped to the barn from the air)

#define AUDIO_FUSE_EVOLVE       AUDIO_FILE("sfx/Evolve.ogg" //This sound is in the first part during Gacha animation
#define AUDIO_FUSE_EVOLVE_2     AUDIO_FILE("sfx/Finish evolving sound2.ogg" //This sound is in the second part during Gacha animation

#define AUDIO_PIG_GRUNT         AUDIO_FILE("sfx/animalx/pig.ogg" //This sound is played when tapping a Pig-type animal
#define AUDIO_FEED_ANIMAL       AUDIO_FILE("sfx/animalx/lip smack.ogg" //Sound when feeding an animal
#define AUDIO_ANIMAL_PUSH       AUDIO_FILE("sfx/animal_push_rance.ogg" //Sound when animal is dropping to animal pen and flys off from the animal pen

#define AUDIO_POWER_UP_01       AUDIO_FILE("sfx/race/Power up ascending 01.ogg" //Sound when the cursor of the gauge is moving near the end on the left/right side (Gauge is when player has to tap Go button at the start of the race
#define AUDIO_POWER_UP_02       AUDIO_FILE("sfx/race/Power up ascending 02.ogg" //Sound when the cursor of the gauge is moving near the end on the opposite side of power_up_01 (Gauge is when player has to tap Go button at the start of the race
#define AUDIO_SKILL_HIT         AUDIO_FILE("sfx/race/when_hit.ogg" //Sound when animal is hit by any skill
#define AUDIO_SKILL_SPEED       AUDIO_FILE("sfx/race/energy whoosh 21.ogg" //Sound when animal is using the dashing skill
#define AUDIO_SKILL_BANANA      AUDIO_FILE("sfx/race/egg splat 05.ogg" //Sound when animal drops banana
#define AUDIO_SKILL_STOMP       AUDIO_FILE("sfx/race/oscillator worm fall multi octave 01.ogg" //Sound when animal is using the stomping skill
#define AUDIO_SKILL_WAVE        AUDIO_FILE("sfx/race/energy whoosh 19.ogg" //Sound when animal launches a shockwave at other animals
#define AUDIO_HIT_FALL          AUDIO_FILE("sfx/race/bed, pillow smash 01.ogg" //Sound when animal falls on the ground after being hit by skills
#define AUDIO_SLIP_BANANA       AUDIO_FILE("sfx/race/body fall ground single 05.ogg" //Sound when animal slips on the bananas and fall down
#define AUDIO_RACE_JUMP         AUDIO_FILE("sfx/race/jump.ogg" //The jumping sound when animal jumps to stomp


#define AUDIO_RACE_CHEERS       AUDIO_FILE("sfx/race/crowd 01 cheers 01.ogg" //During the races, fans who watch the race nearby randomly cheer as you play
#define AUDIO_RACE_WIN1         AUDIO_FILE("sfx/race/crowd 01 reaction, yeah.ogg" //Sound when winning played in parallel with WIN2 && Use for ranch level up effects
#define AUDIO_RACE_WIN2         AUDIO_FILE("sfx/race/fireworks display 04.ogg" //Sound when winning played in parallel with WIN1
#define AUDIO_RACE_LOSE         AUDIO_FILE("sfx/race/dog whine, yelp 04.ogg" //When when you lose the game
#define AUDIO_RACE_START        AUDIO_FILE("sfx/race/gunfire.ogg" //Sound when you tap the Go button

#define AUDIO_MAP_OPEN          AUDIO_FILE("sfx/opencloud.ogg" //Sound when cloud is openned at the world map screen
#define AUDIO_CLOUD_CLOSE       AUDIO_FILE("sfx/air swoosh 01.ogg"
#define AUDIO_CLOUD_OPEN        AUDIO_FILE("sfx/air swoosh 02.ogg"

#define AUDIO_GACHA_PRIZE       AUDIO_FILE("sfx/gacha/gachaPrize.ogg"
#define AUDIO_GACHA_TICK        AUDIO_FILE("sfx/gacha/gachaTick.mp3"

#define AUDIO_CHEST_OPEN        AUDIO_FILE("sfx/chestwood.ogg" //Sound when openning a chest of gems
#define AUDIO_FIREWORK          AUDIO_FILE("sfx/firework1.ogg" //Sound when your animal enters a new lap

#define AUDIO_TUTORIAL_BUBBLE   AUDIO_FILE("sfx/Tutorial Bubble6.ogg" //Sound when tutorial speech bubble is displayed

#define AUDIO_AMBIENCE_FARM     AUDIO_FILE("music/ambience_map.ogg" //Ambient sound for the farm and the map
#define MUSIC_MAP               AUDIO_FILE("music/ambience_map.ogg" //Ambient sound for the farm and the map
#define MUSIC_RACE              AUDIO_FILE("music/Brightly Fancy.ogg" //Music during a race
#define MUSIC_MATCH              AUDIO_FILE("music/"
//#define MUSIC_RACE              AUDIO_FILE("music/Tafi Maradi.mp3"
#define MUSIC_VIL               AUDIO_FILE("music/avalable_scene_01_rev.ogg" //Music in the village
#define MUSIC_VISIT             AUDIO_FILE("music/Savannah Sketch Cut.ogg" //Music when visiting friends and other people's farm
#define MUSIC_GACHA             AUDIO_FILE("music/Jaunty Gumption.ogg" //Music in the Gacha screen

//#define SPLASH_SOUND_1 AUDIO_FILE("sfx/success elegant 3.ogg"
#define SPLASH_SOUND_2 AUDIO_FILE("sfx/Take a Chance.ogg" //Sound during the splash screen
*********************************************************************************************************/

#define kSettingSoundOff        "sound_off"
#define kSettingMusicOff        "music_off"

//************* New Audio Test *************
#define VOLUME_MAX_MUSIC        0.3f
#define VOLUME_MAX_SFX          1.0f

//Generic UI
#define AUDIO_NONE              ""
#define AUDIO_TYPING            AUDIO_FILE("sfx/ui/typing",AUDIO_EXT) //Sound when player types their name of the ranch
#define AUDIO_ERROR             AUDIO_FILE("sfx/ui/error", AUDIO_EXT) //During the tutorial, player has to tap Go button to start the race on the green area, this sound is played if player tap the Go button but not on the green area. When this happens, player has to tap the Go button again until it hits the green area.
#define AUDIO_UI_CLICK          AUDIO_FILE("sfx/ui/ui_click", AUDIO_EXT) //Click/Tap sound (1) for most buttons
#define AUDIO_GENERAL_CLICK     AUDIO_FILE("sfx/ui/general_click", AUDIO_EXT) //Click/Tap sound (2) for most buttons, and when tapping most buildings
#define AUDIO_CLOSE_CLICK       AUDIO_FILE("sfx/ui/close_click", AUDIO_EXT) //Click/Tap sound when tapping to close all dialogs in the game
#define AUDIO_TUTORIAL_BUBBLE   AUDIO_FILE("sfx/ui/text_bubble", AUDIO_EXT) //Sound when tutorial speech bubble is displayed
#define AUDIO_ABILITY_CLICK     AUDIO_FILE("sfx/ui/ability_click", AUDIO_EXT) //Click/Tap sound when tapping on the ability (Sound plays as soon as finger touches, without waiting to release the finger)
#define AUDIO_TRACK_CLICK       AUDIO_FILE("sfx/ui/ability_click", AUDIO_EXT) //Open Shop Panel //Unused???

//Resource Collect
#define AUDIO_COLLECT           AUDIO_FILE("sfx/ui/collect", AUDIO_EXT) //Sound when collecting coins from Ticket Booth or collecting food from Feed Mills
#define AUDIO_CHEST_OPEN        AUDIO_FILE("sfx/ui/chest_open", AUDIO_EXT) //Sound when openning a chest of gems
#define AUDIO_FIREWORK          AUDIO_FILE("sfx/ui/firework", AUDIO_EXT) //Sound when your animal enters a new lap
#define AUDIO_COIN_HIT          AUDIO_FILE("sfx/ui/coin_gauge", AUDIO_EXT) //Sound when claiming Gems from Achievements and other places, sound when gems are added to UI (from purchasing IAP, and others)
#define AUDIO_FOOD_HIT          AUDIO_FILE("sfx/ui/food_gauge", AUDIO_EXT) //Sound when Food bags are added to UI

//Map
#define AUDIO_MAP_OPEN          AUDIO_FILE("sfx/ui/open_map", AUDIO_EXT) //Sound when cloud is openned at the world map screen
#define AUDIO_CLOUD_CLOSE       AUDIO_FILE("sfx/ui/cloud_close", AUDIO_EXT)
#define AUDIO_CLOUD_OPEN        AUDIO_FILE("sfx/ui/cloud_open", AUDIO_EXT)

//Ranch
#define AUDIO_MOVE_BUILDING     AUDIO_FILE("sfx/building/moving_building", AUDIO_EXT) //Sound when player move buildings. (Plays once per tile moved)

#define AUDIO_CUT_TREE          AUDIO_FILE("sfx/building/cut_tree", AUDIO_EXT) //Sound when cutting a tree
#define AUDIO_UNDERCONSTRUCTION AUDIO_FILE("sfx/building/construction", AUDIO_EXT) //Sound when tapping buildings that are under construction
#define AUDIO_FINISH_NOW        AUDIO_FILE("sfx/building/finish_now", AUDIO_EXT) //Sound when using Finish Now or Instant Build options
#define AUDIO_FINISH_BUILDING   AUDIO_FILE("sfx/building/building_finish", AUDIO_EXT) //Sound tapping to open building cover when buildings are completed (When building completes construction, it becomes covered at first. Player tap the cover to open up the cover and reveal the building.)
#define AUDIO_MAGIC_STAR        AUDIO_FILE("sfx/building/star_finish", AUDIO_EXT) //This sound is played during the animation when building opens up its cover
#define AUDIO_RANCH_LEVEL_UP    AUDIO_FILE("sfx/building/ranch_level_up", AUDIO_EXT) //Sound when ranch level up
#define AUDIO_MERCHANT_OPEN     AUDIO_FILE("sfx/building/merchant_open", AUDIO_EXT) //Sound when visit Merchant
#define AUDIO_MERCHANT_BUY      AUDIO_FILE("sfx/building/merchant_buy", AUDIO_EXT) //Sound when buy item from Merchant

//Building
#define AUDIO_FEED_ANIMAL       AUDIO_FILE("sfx/building/eating_food", AUDIO_EXT) //Sound when feeding an animal
#define AUDIO_FOOD_PRODUCTION   AUDIO_FILE("sfx/building/food_production", AUDIO_EXT) //Sound when tapping Feed Mills
#define AUDIO_ANIMAL_PUSH       AUDIO_FILE("sfx/building/animal_pen", AUDIO_EXT) //Sound when animal is dropping to animal pen and flys off from the animal pen
#define AUDIO_DROP_ANIMAL_TO_BUILDING AUDIO_FILE("sfx/building/animal_barn", AUDIO_EXT) //This sound is played as animals are sent to barn. (Animation in the game shows an icon of the animal dropped to the barn from the air)
#define AUDIO_FUSE_EVOLVE       AUDIO_FILE("sfx/building/evolve", AUDIO_EXT) //This sound is in the first part during Gacha animation
#define AUDIO_FUSE_EVOLVE_2     AUDIO_FILE("sfx/building/evolve_finish", AUDIO_EXT) //This sound is in the second part during Gacha animation
#define AUDIO_PET_LEVEL_UP      AUDIO_FILE("sfx/building/pet_level_up", AUDIO_EXT) //Sound when pet level up

//Race
#define AUDIO_ENERGY_REGEN      AUDIO_FILE("sfx/race/powerup_recharge", AUDIO_EXT) //When your pet completes a lap, pet energy regens, and this sound is played
#define AUDIO_POWER_UP_01       AUDIO_FILE("sfx/race/power_gauge_1", AUDIO_EXT) //Sound when the cursor of the gauge is moving near the end on the left/right side (Gauge is when player has to tap Go button at the start of the race
#define AUDIO_POWER_UP_02       AUDIO_FILE("sfx/race/power_gauge_2", AUDIO_EXT) //Sound when the cursor of the gauge is moving near the end on the opposite side of power_up_01 (Gauge is when player has to tap Go button at the start of the race

#define AUDIO_RACE_CHEERS       AUDIO_FILE("sfx/race/race_cheers", AUDIO_EXT) //During the races, fans who watch the race nearby randomly cheer as you play
#define AUDIO_RACE_WIN1         AUDIO_FILE("sfx/race/race_win", AUDIO_EXT) //Sound when winning played in parallel with WIN2 && Use for ranch level up effects
#define AUDIO_RACE_WIN2         AUDIO_FILE("sfx/race/fireworks_win", AUDIO_EXT) //Sound when winning played in parallel with WIN1
#define AUDIO_RACE_LOSE         AUDIO_FILE("sfx/race/race_lose", AUDIO_EXT) //When when you lose the game
#define AUDIO_RACE_START        AUDIO_FILE("sfx/race/race_start", AUDIO_EXT) //Sound when you tap the Go button

//Ability
#define AUDIO_SKILL_HIT         AUDIO_FILE("sfx/race/hit_by_skill", AUDIO_EXT) //Sound when animal is hit by any skill
#define AUDIO_HIT_FALL          AUDIO_FILE("sfx/race/hit_fall", AUDIO_EXT) //Sound when animal falls on the ground after being hit by skills
#define AUDIO_SKILL_SPEED       AUDIO_FILE("sfx/race/skill_speed", AUDIO_EXT) //Sound when animal is using the dashing skill
#define AUDIO_SKILL_BANANA      AUDIO_FILE("sfx/race/skill_banana", AUDIO_EXT) //Sound when animal drops banana
#define AUDIO_SLIP_BANANA       AUDIO_FILE("sfx/race/banana_slip", AUDIO_EXT) //Sound when animal slips on the bananas and fall down
#define AUDIO_RACE_JUMP         AUDIO_FILE("sfx/race/skill_stomp_jump", AUDIO_EXT) //The jumping sound when animal jumps to stomp
#define AUDIO_SKILL_STOMP       AUDIO_FILE("sfx/race/skill_stomp_land", AUDIO_EXT) //Sound when animal is using the stomping skill
#define AUDIO_SKILL_WAVE        AUDIO_FILE("sfx/race/skill_shockwave", AUDIO_EXT) //Sound when animal launches a shockwave at other animals
#define AUDIO_SKILL_WAVE_FLY    AUDIO_FILE("sfx/race/skill_wave_fly", AUDIO_EXT) //Sound when shockwave flying throught the air


//Gacha
#define AUDIO_GACHA_OPEN        AUDIO_FILE("sfx/gacha/gachaOpen", AUDIO_EXT)
#define AUDIO_GACHA_PRIZE       AUDIO_FILE("sfx/gacha/gachaPrize", AUDIO_EXT)
#define AUDIO_GACHA_TICK        AUDIO_FILE("sfx/gacha/gachaTick", AUDIO_EXT)

#define AUDIO_PIG_GRUNT         AUDIO_FILE("sfx/animalx/pig", AUDIO_EXT) //This sound is played when tapping a Pig-type animal

//Music
#define AUDIO_AMBIENCE_FARM     AUDIO_FILE("music/ambience_map", AUDIO_EXT) //Ambient sound for the farm and the map
#define MUSIC_MAP               AUDIO_FILE("music/ambience_map", AUDIO_EXT) //Ambient sound for the farm and the map
#define MUSIC_RACE              AUDIO_FILE("music/capture_Ranch run01", AUDIO_EXT)//Music during a race
#define MUSIC_MATCH             AUDIO_FILE("music/"
#define MUSIC_VIL               AUDIO_FILE("music/avalable_scene_01_rev", AUDIO_EXT) //Music in the village
#define MUSIC_VISIT             AUDIO_FILE("music/Savannah Sketch Cut", AUDIO_EXT) //Music when visiting friends and other people's farm
#define MUSIC_GACHA             AUDIO_FILE("music/Jaunty Gumption", AUDIO_EXT) //Music in the Gacha screen

//#define SPLASH_SOUND_1 AUDIO_FILE("sfx/success elegant 3", AUDIO_EXT)
#define SPLASH_SOUND_2 AUDIO_FILE("music/Take a Chance", AUDIO_EXT) //Sound during the splash screen


class GameAudioService {

    bool m_sound_on;
    bool m_music_on;
    
    bool isUpdate;
    float stepVolume;
    float stepFadeOut;
    float stepFadeIn;
    float stepMusicFadeOut;
    float stepMusicFadeIn;
    
    int stateFade;      //normal = 0 , fadeIn = 1 , fadeOut = 2 , fadeInAndOut = 3
    int stateMusicFade;
    
    const char *_audio_name_;
    bool _Loop;
    float _fadeTime;
    
    const char *_audio_name_Music;
    bool _LoopMusic;
    float _fadeTimeMusic;
    float _delayMusic;

public:
    
    GameAudioService();
    ~GameAudioService();
    
    /** call this on main update */
    void update();
    void preloadAllEffect();
    void unloadAllEffect();

    enum PreloadEffectType
    {
        Generic,
        TutorialRace,
        TutorialVillage,
        Village,
        Map,
        Race,
        Gacha,
    };

    // scene specific preloading
    void preloadEffects(PreloadEffectType scene);
    void unloadEffects(PreloadEffectType scene);

    void clearMusicFadeOut(float fadeTime);
    void clearEffectFadeOut(float fadeTime);
        
    void playEffectFadeOutAndFadeIn(const char *audio_name_, bool Loop, float fadeTime);
    unsigned int playEffectFadeIn(const char *audio_name_, bool Loop, float fadeTime);
    unsigned int playEffect(const char *audio_name_, bool Loop);
    unsigned int playEffect(const char *audio_name_);
    
    
    void playMusicFadeOutAndDelayBeforeFadeIn(const char *audio_name_, bool Loop, float fadeOut, float delay, float fadeIn);
    void playMusicFadeOutAndFadeIn(const char *audio_name_, bool Loop, float fadeTime);
    void playMusicFadeIn(const char *audio_name_, bool Loop, float fadeTime);
    void playMusicFadeIn(const char *audio_name_, float fadeTime);
    void playMusic(const char *audio_name_, bool Loop);
    void playMusic(const char *audio_name_);
    
    void stopEffect(unsigned int nSoundId);
    void stopAllEffect();
    void setEffectsVolume(float volume);
    void stopMusic();

    void resumeEffects();
    void pauseEffects();

    void updateAudioSetting();
    
    void toggleSound();
    void toggleMusic();
    bool isMusicOn();
    bool isSoundOn();
    
};

#endif /* defined(__Cheetah__GameAudioService__) */
