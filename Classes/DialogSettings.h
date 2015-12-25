//
//  UISettings.h
//  Cheetah
//
//  Created by Vaclav Samec on 10/15/2557 BE.
//
//

#ifndef Cheetah_UISettings_h
#define Cheetah_UISettings_h

//
// these are settings for all dialogs in the game
//

#define kDialogAchievementSize      cocos2d::CCSize(1650.0f, 1010.0f)
#define kFrameAchievementSize       cocos2d::CCSize(1360.0f, 240.0f)
#define kDialogAbilitySize          cocos2d::CCSize(1574.0f, 1110.0f)

#define kLeaderBoardSizeWidth       1650.0f
#define kLeaderBoardSizeHeight      1020.0f
#define kDialogLeaderboardSize      cocos2d::CCSize(kLeaderBoardSizeWidth, kLeaderBoardSizeHeight)
#define kTabLeaderboardSize         cocos2d::CCSize(kLeaderBoardSizeWidth, kLeaderBoardSizeHeight - 730.0f)
#define kFrameLeaderboardSize       cocos2d::CCSize(1360.0f, 145.0f)

#define kFriendHelpSizeHeight       1110.0f
#define kDialogFriendHelpSize       cocos2d::CCSize(1650.0f, kFriendHelpSizeHeight)
#define kFrameFriendHelpSize        cocos2d::CCSize(477.0f,  kFriendHelpSizeHeight-236.0f)

#define kDialogTradeAnimalSize      cocos2d::CCSize(1428.0f, 1000.0f)
#define kDialogAnimalBarnSize       cocos2d::CCSize(1428.0f, 920.0f)

#define kDialogInboxSize            cocos2d::CCSize(1650.0f, 1000.0f)
#define kFrameInboxSize             cocos2d::CCSize(1360.0f, 145.0f)

#define kDialogItemBarnSize         cocos2d::CCSize(1328.0f, 870.0f)

#define kDialogIAPSize              cocos2d::CCSize(1650.0f, 1030.0f)
#define kDialogNotEnoughSpaceSize   cocos2d::CCSize(1000.0f, 770.0f)
#define kDialogNotificationSize     cocos2d::CCSize(800.0f, 550.0f)
#define kDialogRatingSize           cocos2d::CCSize(800.0f, 500.0f)

#define kDialogFBLoginSize          cocos2d::CCSize(1536.0f, 657.0f)

#define kDialogInboxPosition        ccp(0.0f,-40.0f*sizeMultiplier*isIPhoneMultiplier)
#define kDialogIAPPosition          ccp(0.0f,-60.0f*sizeMultiplier*isIPhoneMultiplier)
#define kDialogLeaderBoardPosition  ccp(0.0f,-20.0f*sizeMultiplier*isIPhoneMultiplier)
#define kDialogAchievementPosition  ccp(0.0f,-20.0f*sizeMultiplier*isIPhoneMultiplier)
#define kDialogAbilityPosition      ccp(0.0f,-40.0f*sizeMultiplier*isIPhoneMultiplier)
#define kDialogFriendHelpPosition   ccp(0.0f,-40.0f*sizeMultiplier*isIPhoneMultiplier)

#define kDialogSetting              cocos2d::CCSize(920.0f, 1080.0f)
#define kDialogSimpleDefault        cocos2d::CCSize(1000.0f, 600.0f)

#define kDialogLinkDevice			cocos2d::CCSize(920.0f, 930.0f)

// parameters:
// (BounceOptions bounceOptions, float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift)
//
//                                      BounceOptions bounceOptions
//                                      |                               float leftMargin
//                                      |                               |       float rightMargin
//                                      |                               |       |       float topMargin
//                                      |                               |       |       |       float bottomMargin
//                                      |                               |       |       |       |       float contentXShift
//                                      |                               |       |       |       |       |       float contentYShift
//#define ABILITY_DIALOG_SETTINGS         ScrollDialog::BounceHorizontal, 0.08f,  0.08f,  0.12f,  0.1f
//#define ACHIEVEMENT_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   0.08f,  0.08f,  0.13f,  0.13f
//#define ANIMALBARN_DIALOG_SETTINGS      ScrollDialog::BounceHorizontal, 0.1f,   0.1f,   0.15f,  0.15f
//#define ANIMALMARKET_DIALOG_SETTINGS    ScrollDialog::BounceHorizontal, 0.1f,   0.1f,   0.15f,  0.15f
//#define ANIMALTRADE_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   0.1f,   0.55f,  0.17f,  0.15f,  0.0f,   0.0f
//#define CHOOSEHELPER_DIALOG_SETTINGS    ScrollDialog::BounceHorizontal, 0.08f,  0.08f,  0.12f,  0.18f
//#define IAPP_DIALOG_SETTINGS            ScrollDialog::BounceHorizontal, 0.03f,  0.07f,  0.16f,  0.1f,   0.07f,  0.0f
//#define INBOX_DIALOG_SETTINGS           ScrollDialog::BounceVertical,   0.07f,  0.07f,  0.12,   0.12f,  0.0f,   0.0f
#define LEADERBOARD_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   0.07f,  0.07f,  0.19f,  0.09f,  0.0f,   0.0f
//#define SEARCHITEM_DIALOG_SETTINGS      ScrollDialog::BounceVertical,   0.03f,  0.03f,  0.11f,  0.1f
//#define ITEMSTORAGE_DIALOG_SETTINGS     ScrollDialog::BounceHorizontal, 0.09f,  0.1f,   0.15f,  0.15f

#define DefaultLeftMargin (123.0f*sizeMultiplier)
#define DefaultRightMargin (135.0f*sizeMultiplier)
#define DefaultTopMargin (142.0f*sizeMultiplier)
#define DefaultBottomMargin (133.0f*sizeMultiplier)

#define ABILITY_DIALOG_SETTINGS         ScrollDialog::BounceHorizontal, DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin
#define ACHIEVEMENT_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   0,  0,  DefaultTopMargin,  DefaultBottomMargin
#define ANIMALBARN_DIALOG_SETTINGS      ScrollDialog::BounceHorizontal, DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin
#define ANIMALMARKET_DIALOG_SETTINGS    ScrollDialog::BounceHorizontal, DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin
#define ANIMALTRADE_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   DefaultLeftMargin,  830.0f*sizeMultiplier, DefaultTopMargin,  DefaultBottomMargin,  0.0f,   0.0f
#define CHOOSEHELPER_DIALOG_SETTINGS    ScrollDialog::BounceHorizontal, DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin
#define IAPP_DIALOG_SETTINGS            ScrollDialog::BounceHorizontal, DefaultLeftMargin,  (122.0f*sizeMultiplier),  DefaultTopMargin,  DefaultBottomMargin,   0.0f,  0.0f
#define INBOX_DIALOG_SETTINGS           ScrollDialog::BounceVertical,   DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,   DefaultBottomMargin,  0.0f,   0.0f
//#define LEADERBOARD_DIALOG_SETTINGS     ScrollDialog::BounceVertical,   (115.0f*sizeMultiplier),  (115.0f*sizeMultiplier),  (185.0f*sizeMultiplier),  (78.0f*sizeMultiplier),  0.0f,   0.0f
#define SEARCHITEM_DIALOG_SETTINGS      ScrollDialog::BounceVertical,   DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin
#define ITEMSTORAGE_DIALOG_SETTINGS     ScrollDialog::BounceHorizontal, DefaultLeftMargin,  DefaultRightMargin,  DefaultTopMargin,  DefaultBottomMargin


//
// these are settings for all modal dialogs in the game with front background (inner window)
//
// parameters:
// (BounceOptions bounceOptions, float leftMargin, float rightMargin, float topMargin, float bottomMargin, float contentXShift, float contentYShift)
//
#define TUTORIAL_DIALOG_SETTINGS        0.05f, 0.05f, 0.13f, 0.13f

//
// slice values for scalable sprites:
// (left margin, top margin, width and height of the center rectangle)
//
// 184x26 - 315x44
// 179x20 - 306x36
//
#define SLICE_LAYOUT_SCREEN_FOR_SLICE    CCRectMake(414.0f*sizeMultiplier, 424.0f*sizeMultiplier, 109.0f*sizeMultiplier,  45.0f*sizeMultiplier)
#define SLICE_LAYOUT_HEADER              CCRectMake(108.0f*sizeMultiplier, 0.0f*sizeMultiplier,   14.0f*sizeMultiplier,  131.0f*sizeMultiplier)
#define SLICE_LAYOUT_PROGRESS_BAR_BG     CCRectMake(50.0f*sizeMultiplier,  10.0f*sizeMultiplier,  215.0f*sizeMultiplier,  24.0f*sizeMultiplier)

#define SLICE_LAYOUT_PROGRESS_BAR_BG_HUD CCRectMake(30.0f*sizeMultiplier,  15.0f*sizeMultiplier,  267.0f*sizeMultiplier,  24.0f*sizeMultiplier)

#define SLICE_LAYOUT_PROGRESS_BAR_COLOR  CCRectMake(17.0f*sizeMultiplier,  10.0f*sizeMultiplier,  272.0f*sizeMultiplier,  16.0f*sizeMultiplier)
#define SLICE_LAYOUT_TAB_ACTIVE          CCRectMake(30.0f*sizeMultiplier,  30.0f*sizeMultiplier,  298.0f*sizeMultiplier, 100.0f*sizeMultiplier)
#define SLICE_LAYOUT_TAB_INACTIVE        CCRectMake(30.0f*sizeMultiplier,  30.0f*sizeMultiplier,  315.0f*sizeMultiplier,  33.0f*sizeMultiplier)
#define SLICE_LAYOUT_ACHIEVEMENT         CCRectMake(43.0f*sizeMultiplier,   43.0f*sizeMultiplier,  5.0f*sizeMultiplier,    4.0f*sizeMultiplier)
#define SLICE_LAYOUT_RIBBON              CCRectMake(398.0f*sizeMultiplier,   0.0f*sizeMultiplier,  11.0f*sizeMultiplier,  185.0f*sizeMultiplier)
#define SLICE_LAYOUT_RIBBON_SMALL        CCRectMake(292.0f*sizeMultiplier,   0.0f*sizeMultiplier,  8.0f*sizeMultiplier,  139.0f*sizeMultiplier)
#define SLICE_LAYOUT_TRACK_MENU          CCRectMake(0.25f*sizeMultiplier,    0.0f*sizeMultiplier,  0.5f*sizeMultiplier,   247.0f*sizeMultiplier)
#define SLICE_LAYOUT_RACE_WHITE_BG       CCRectMake(0.25f*sizeMultiplier,    0.0f*sizeMultiplier,  0.5f*sizeMultiplier,   82.0f*sizeMultiplier)
#define SLICE_LAYOUT_RACE_GREEN_BG       CCRectMake(1.0f*sizeMultiplier,     0.0f*sizeMultiplier,  1.0f*sizeMultiplier,   69.0f*sizeMultiplier)
#define SLICE_LAYOUT_RACE_YELLOW_BG      CCRectMake(1.0f*sizeMultiplier,     0.0f*sizeMultiplier,  1.0f*sizeMultiplier,   69.0f*sizeMultiplier)
#define SLICE_LAYOUT_PAPER_BG            CCRectMake(150.0f*sizeMultiplier,   144.0f*sizeMultiplier, 12.5f*sizeMultiplier, 13.0f*sizeMultiplier)

#define TITLE_HEADER_OFFSET                         (230.0f*sizeMultiplier)
#define DIALOG_SIMPLE_TITLE_OFFSET                  (80.0f*sizeMultiplier)  // y offset from top border
#define DIALOG_SIMPLE_DESCRIPTION_OFFSET            (50.0f*sizeMultiplier) // y offset from title
#define DIALOG_SIMPLE_BUTTON_OFFSET                 (150.0f*sizeMultiplier) // y offset from description
#define DIALOG_SIMPLE_MARGIN_LEFT                   (100.0f*sizeMultiplier)
#define DIALOG_SIMPLE_MARGIN_BOTTOM                 (0.0f*sizeMultiplier)

#define DESCRIPTION_TEXT_MAX_LENGTH                 (750.0f*sizeMultiplier)
#define DESCRIPTION_TEXT_NOTIFY_MAX_LENGTH          (650.0f*sizeMultiplier)

#define ACHIEVEMENT_RIBBON_SIZE         (CCSizeMake())

#endif
