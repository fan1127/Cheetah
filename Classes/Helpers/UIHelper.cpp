//
// Created by Vaclav Samec on 10/20/14 AD.
//

#include "cocos2d.h"
#include "cocos-ext.h"
#include "UIHelper.h"
#include "DialogSettings.h"
#include "Settings.h"
#include "AppMacros.h"

extern TargetDevice targetDevice;

using namespace cocos2d;

void UIHelper::setTwoNodesToMiddlePosition(cocos2d::CCNode* nodeLeft, cocos2d::CCNode* nodeRight, float margin, cocos2d::CCPoint position)
{
    float widthNodeLeft = nodeLeft->getContentSize().width * nodeLeft->getScale();
    float widthNodeRight = nodeRight->getContentSize().width * nodeRight->getScale();
    
    setTwoNodesToMiddlePosition(widthNodeLeft, widthNodeRight, nodeLeft, nodeRight, margin, position);
}

void UIHelper::setTwoNodesToMiddlePosition(float widthNodeLeft, float widthNodeRight, cocos2d::CCNode* nodeLeft, cocos2d::CCNode* nodeRight, float margin, cocos2d::CCPoint position)
{
    float totalWidth = widthNodeLeft + margin + widthNodeRight;
    
    nodeLeft->setAnchorPoint(ccp(0.5f, nodeLeft->getAnchorPoint().y));
    nodeRight->setAnchorPoint(ccp(0.5f, nodeRight->getAnchorPoint().y));
    
    nodeLeft->setPosition(ccp(position.x
                              - totalWidth*0.5f
                              + widthNodeLeft*0.5f,
                              position.y));
    nodeRight->setPosition(ccp(position.x
                               - totalWidth*0.5f
                               + widthNodeLeft
                               + margin
                               + widthNodeRight*0.5f,
                               position.y));
}

void UIHelper::cacheSprites()
{
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("ui-scalable.plist");
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("ui-not-scalable-0.plist");
    if (targetDevice == TD_IPHONE_RETINA) {
        cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("ui-not-scalable-1.plist");
    }
    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);

#if IS_DEBUG_BUILD
    checkSprites();
#endif
}

void UIHelper::checkSprites()
{
    //
    // test scalable sprites
    //
    for (int i=Header; i<ScalableSpritesEnd; i++)
    {
        getScalableSprite(ScalableSprites(i));
    }

    //
    // test non-scalable sprites
    //
    for (int i=AbilitiesAbilitiesButton; i<UISpritesEnd; i++)
    {
        getSprite(UISprites(i));
    }
}

extension::CCScale9Sprite* UIHelper::getScalableSprite(ScalableSprites sprite)
{
    extern float sizeMultiplier;

    switch (sprite)
    {
        case Header:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_HEADER);

        case ProgressBarBg:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_PROGRESS_BAR_BG);

        case ProgressBarColor:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_PROGRESS_BAR_COLOR);

        case ScreenForSlice:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_SCREEN_FOR_SLICE);

        case TabActive:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_TAB_ACTIVE);

        case TabInactive:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_TAB_INACTIVE);

        case AchievementBG:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_ACHIEVEMENT);

        case AchievementBGSolid:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_ACHIEVEMENT);

        case Ribbon:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RIBBON);

        case RibbonGrey:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RIBBON);

        case RibbonSmall:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RIBBON_SMALL);

        case TrackMenuBg:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_TRACK_MENU);

        case RaceElementsBarWhite:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RACE_WHITE_BG);

        case RaceElementsBarGreen:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RACE_GREEN_BG);

        case RaceElementsBarYellow:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_RACE_YELLOW_BG);

        case PaperBg:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_PAPER_BG);

        case ProgressBarBgHUD:
            return extension::CCScale9Sprite::createWithSpriteFrameName(scalableSprites[sprite], SLICE_LAYOUT_PROGRESS_BAR_BG_HUD);

        default:
            CCAssert(false, "Invalid scalable sprite!");
    }

    return NULL;
}

cocos2d::CCSpriteFrame* UIHelper::getSpriteFrame(UISprites sprite)
{
    CCAssert(sprite < UISpritesEnd, "Invalid sprite name!");
    return CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(uiSprites[sprite]);
}

cocos2d::CCSpriteFrame* UIHelper::getSpriteFrame(ScalableSprites sprite)
{
    CCAssert(sprite < ScalableSpritesEnd, "Invalid sprite name!");
    return CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(scalableSprites[sprite]);
}

std::vector<cocos2d::CCSpriteFrame*> UIHelper::getAnimationFrames(UIAnimations animation) {
    
    std::vector<cocos2d::CCSpriteFrame*> array;
    CREATE_CHAR_BUFFER(ss, 128);

    switch (animation) {
        case AnimationHUDGatcha:{
            const char* baseString = "HUD/gacha_animation/ui_gacha_";
            const char* endString = ".png";
            for (int i=0; i<20; i++) {
                snprintf(ss, ss_size, "%s%d%s", baseString, i, endString);
                auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(ss);
                CCAssert(frame != NULL, "Image frame is NULL");
                array.push_back(frame);
            }
        }break;
        case AnimationHUDSignalSign: {
            const char* baseString = "HUD/Signal_effect/_hud_connection_";
            const char* endString = ".png";
            for (int i=1; i<=3; i++) {
                snprintf(ss, ss_size, "%s%d%s", baseString, i, endString);
                auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(ss);
                CCAssert(frame != NULL, "Image frame is NULL");
                array.push_back(frame);
            }
        }break;
        default: break;
    }
    
    return array;
    
}

//! definition of all scalable sprites
const char* const UIHelper::scalableSprites[ScalableSpritesEnd] =
{
    "Scalable/Header.png",
    "Scalable/Progress_bar_bg.png",
    "Scalable/Progress_bar_color.png",
    "Scalable/bg.png",
    "Scalable/bg_solid.png",
    "Scalable/ribbon.png",
    "Scalable/small_ribbon.png",
    "Scalable/ribbon_lose.png",
    "Scalable/screen_for_slice.png",
    "Scalable/tab_active.png",
    "Scalable/tab_icanctive.png",
    "Scalable/tiled_bg_build.png",
    "Scalable/paper_bg.png",

    // racing stuff
    "Scalable/bar_green_tiled.png",
    "Scalable/bar_yellow_tiled.png",
    "Scalable/bar_white_bg_tiled.png",

    "Scalable/Progress_bar_map_bg.png",
};

const char* const UIHelper::uiSprites[UISpritesEnd] =
{
    "Abilities/abilities_button.png",
    "Abilities/abilities_button_bg.png",
    "Abilities/powerup_1.png",
    "Abilities/powerup_2.png",
    "Abilities/powerup_3.png",
    "Abilities/powerup_4.png",
    "Abilities/powerup_5.png",
    "Abilities/stan_icon.png",
    "Abilities/abilities_button_inactive.png",
    "Abilities/lock.png",

    "Abilities/abilities_button_grayscale.png",
    "Abilities/Inactive_abilities/powerup_1.png",
    "Abilities/Inactive_abilities/powerup_2.png",
    "Abilities/Inactive_abilities/powerup_3.png",
    "Abilities/Inactive_abilities/powerup_4.png",
    "Abilities/Inactive_abilities/powerup_5.png",

    "Achievements/Claim_button.png",
    "Achievements/Claim_gem.png",
    "Achievements/Game_center.png",
    "Achievements/Star.png",
    "Achievements/Star_empty.png",
    "Achievements/reward_gem.png",

    "Animal_stats/_icon_track_asphalt.png",
    "Animal_stats/_icon_track_dirt.png",
    "Animal_stats/_icon_track_grass.png",
    "Animal_stats/_icon_track_sand.png",
    "Animal_stats/_icon_track_snow.png",
    "Animal_stats/energy_icon.png",
    "Animal_stats/from_barn_icon.png",
    "Animal_stats/in_barn_icon.png",
    "Animal_stats/leader_icon.png",
    "Animal_stats/like_icon.png",
    "Animal_stats/mobility_icon.png",
    "Animal_stats/speed_icon.png",
    "Animal_stats/sell_button.png",

    "Build/Buildings_icon.png",
    "Build/cr_tree_icon.png",
    "Build/description_bg.png",
    "Build/arrow_build.png",

    "Facebook_connection/dialogue_box.png",
    "Facebook_connection/facebook_button.png",
    "Facebook_connection/no_thanks.png",
    "Facebook_connection/claim_gems.png",

    "Gacha/gacha_promotion_crossout.png",
    "Gacha/promotion_button.png",
    "Gacha/promotion_glow.png",
    "Gacha/promotion_marker_gacha.png",
    "Gacha/scaled_gem.png",
    "Gacha/title_lucky_box.png",

    "HUD/Gatcha_button.png",
    "HUD/achievements.png",
    "HUD/add.png",
    "HUD/build_button.png",
    "HUD/coin.png",
    "HUD/counter_big.png",
    "HUD/counter_big_color.png",
    "HUD/counter_medals_n_trace.png",
    "HUD/expirience_bar.png",
    "HUD/expirience_bar_color.png",
    "HUD/food.png",
    "HUD/friends.png",
    "HUD/gem.png",
    "HUD/inbox.png",
    "HUD/map_button.png",
    "HUD/farm_icon.png", // home button
    "HUD/new_marker_big.png",
    "HUD/new_marker_small.png",
    "HUD/settings.png",
    "HUD/trace.png",

    "HUD/ground_ok.png",
    "HUD/ground_false.png",
    "HUD/ground_ok_icon.png",
    "HUD/ground_false_icon.png",
    "HUD/checkmark_big.png",
    "HUD/checkmark_small.png",
    "HUD/icon_coin.png",
    "HUD/icon_food.png",
    "HUD/Signal_effect/_hud_connection_1.png",
    
    "paper_arrow.png",
    "paper_arrow_left.png",

    "N_S/Claim_gem.png",
    "N_S/blue_stroke_gem.png",

    "N_S/Classes/E_class.png",
    "N_S/Classes/D_class.png",
    "N_S/Classes/C_class.png",
    "N_S/Classes/B_class.png",
    "N_S/Classes/A_class.png",
    "N_S/Classes/S_class.png",
    "N_S/Classes/SS_class.png",
    "N_S/Classes/ribbon_friend.png",
    "N_S/Classes/ribbon_enemy.png",

    "N_S/Friends_icon.png",
    "N_S/buy_coin_icon.png",
    "N_S/buy_gem_icon.png",
    "N_S/close_button.png",
    "N_S/help_button.png",
    "N_S/medal.png",
    "N_S/reward_gem.png",
    "N_S/world_icon.png",

    "Pet_shop/_ui_habitat.png",
    "Pet_shop/_ui_habitat_questionmark.png",

    "Possible_rewards/change_button.png",
    "Possible_rewards/change_icon.png",
    "Possible_rewards/paper.png",
    "Possible_rewards/slider.png",
    "Possible_rewards/visit_button.png",

    "Purchase/coin_icon.png",
    "Purchase/coins_1.png",
    "Purchase/coins_2.png",
    "Purchase/coins_3.png",
    "Purchase/food_1.png",
    "Purchase/food_2.png",
    "Purchase/food_3.png",
    "Purchase/food_icon.png",
    "Purchase/gem_icon.png",
    "Purchase/gems_1.png",
    "Purchase/gems_2.png",
    "Purchase/gems_3.png",
    "Purchase/gems_4.png",
    "Purchase/gems_5.png",
    "Purchase/item_bg.png",
    "Purchase/most_popular.png",
    "light.png",
    "Purchase/promotion_crossout.png",
    "Purchase/promotion_marker.png",
    "Purchase/promotion_time.png",

    "Race_elements/arrows.png",
    "Race_elements/back_button.png",
    "Race_elements/bar_border.png",
    "Race_elements/enemy_pet_bg.png",
    "Race_elements/give_up_button_1.png",
    "Race_elements/give_up_button_2.png",
    "Race_elements/go_button.png",
    "Race_elements/leader_bg.png",
    "Race_elements/power_icon.png",
    "Race_elements/speedometer.png",
    "Race_elements/speedometer_arrow.png",
    "Race_elements/start_button.png",
    "Race_elements/your_pet.png",
    "Race_elements/wake_up_button.png",
    "Race_elements/feed_button.png",
    "Race_elements/food_icon.png",
    "Race_elements/active_animal.png",
    "Race_elements/inactive_animal.png",
    "Scalable/add_pattern_green_area.png",
    "Race_elements/power_bar_white_bg.png",
    "Race_elements/start_bar_border.png",
    "Race_elements/helper_friend.png",
    "Race_elements/helper_not_friend.png",

    "Ranking/Visit_button.png",
    "Ranking/score_counter.png",

    "Settings/off_button.png",
    "Settings/on_button.png",
    "Settings/support_button.png",

    "Trace_&_buildings_options/add_button.png",
    "Trace_&_buildings_options/cancel_button.png",
    "Trace_&_buildings_options/delete_button.png",
    "Trace_&_buildings_options/info_button.png",
    "Trace_&_buildings_options/ok_button.png",
    "Trace_&_buildings_options/overview_button.png",
    "Trace_&_buildings_options/rotate_button.png",
    "Trace_&_buildings_options/upgrade_button.png",
    "Trace_&_buildings_options/Start_button.png",
    "Trace_&_buildings_options/start_reverse_button.png", // reverse
    "Trace_&_buildings_options/axe.png",
    "Trace_&_buildings_options/collect_coins.png",
    "Trace_&_buildings_options/collect_coins_inactive.png",
    "Trace_&_buildings_options/collect_food.png",
    "Trace_&_buildings_options/collect_food_inactive.png",
    "Trace_&_buildings_options/pack_obejct.png",
    "Trace_&_buildings_options/set_as_leader.png",
    "Trace_&_buildings_options/instant_upgrade.png",
    "Trace_&_buildings_options/construction_process.png",
    "Trace_&_buildings_options/sleep_process.png",
    "Trace_&_buildings_options/upgrade_process.png",
    "Trace_&_buildings_options/hungry_emotion.png",
    "Trace_&_buildings_options/sleepy_emotion.png",
    "Trace_&_buildings_options/buy_items.png",
    "Trace_&_buildings_options/powerup_small_1.png",
    "Trace_&_buildings_options/powerup_small_2.png",
    "Trace_&_buildings_options/powerup_small_3.png",
    "Trace_&_buildings_options/powerup_small_4.png",
    "Trace_&_buildings_options/powerup_small_5.png",
    "Trace_&_buildings_options/gem_for_animation.png",
    "Trace_&_buildings_options/bubble_emotion.png",

    "Upgrade/+bg.png",
    "Upgrade/Building_bg.png",
    "Upgrade/Clock_icon.png",
    "Upgrade/Food_1.png",
    "Upgrade/Food_2.png",
    "Upgrade/Ranch_icon.png",
    "Upgrade/Up_arrow.png",
    "Upgrade/instant_buy_button.png",
    "Upgrade/energy_icon.png",
    "Upgrade/coins_storage.png",
    "Upgrade/item_capacity.png",
    "Upgrade/tickets.png",
    "Upgrade/icon_max_bonus.png",
    "Upgrade/icon_animal_room.png",
    "Upgrade/icon_distance.png",
    "Upgrade/icon_trackpoint.png",
    "Upgrade/icon_tradable_animal.png",

    "Upgrade_and_trade_items_for_animal/arrow.png",
    "Upgrade_and_trade_items_for_animal/buy_button.png",

    "Wake_up/close_button.png",
    "Wake_up/paper.png",

    "Friends_icon.png",
    "earth_icon.png",

    "Friends_help/start_button_s.png",
    "Friends_help/stun_help_icon.png",
    "Friends_help/help_bg.png",
    "Friends_help/help_bg_active.png",
    "Friends_help/facebook_button_2.png",
    "Friends_help/facebook_icon.png",

    "Inbox/claim_button.png",
    "Inbox/coin_inbox.png",
    "Inbox/gem_inbox.png",
    "Inbox/level_icon.png",
    "Inbox/medal_icon_s.png",
    "Inbox/revenge_button.png",
    "new_marker.png",

    "avatar_border.png",
    "avatar_default.png",

    "Notifications/yes_button.png",
    "Notifications/no_button.png",
    
    "Merchant/merchant_object_button.png",
    "Merchant/merchant_object_light.png",
    "Merchant/merchant_coins_button.png",
    "Merchant/merchant_gems_button.png",
    "Merchant/merchant_object_button_sold.png",
    "Merchant/merchant_sold_button.png",
    
    "plus_button.png",
    "minus_button.png",
    
    "Replay_HUD/1x.png",
    "Replay_HUD/2x.png",
    "Replay_HUD/pause.png",
    "Replay_HUD/play_small.png",
    "Replay_HUD/play_big.png",

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    "Achievements/Google_play_service.png",
    "Facebook_connection/google_play_service.png",
#endif
};
