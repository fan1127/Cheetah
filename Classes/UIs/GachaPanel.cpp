//
//  GachaPanel.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 3/24/2557 BE.
//
//

#include "GachaPanel.h"
USING_NS_CC;
using namespace std;

#include <spine/Json.h>
using namespace cocos2d::extension;

#include "../Services/GameContentService.h"
extern GameContentService* g_gameContent;

#include "../Services/BackendService.h"
#include "../Services/APIs/APIGachaGet.h"
#include "../Services/APIs/APIPetRetrieve.h"
#include "../Entities/PlayerState.h"

#include "../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../Services/GameAudioService.h"
extern GameAudioService *g_gameAudio;

#include "../Scenes/SceneVille.h"
extern CCScene* g_currentScene;
#include "../Extensions/CCMenuItemScaleSprite.h"
#include "../UIs/GachaResult.h"

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../Extensions/CCNikButton.h"
#include "../Extensions/CCLabelHelper.h"
#include "../Extensions/Modal/CCModalController.h"
#include "../Helpers/CCStringHelper.h"
#include "../UIs/Dialogs/IAPDialog.h"

#include "../Extensions/EllipseParicle.h"
#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/AnalyticsClientWrapper.h"

extern float margin;
extern float retinaMultiplier;
extern float sizeMultiplier;
extern float isIPhoneMultiplier;
extern float isIPadMultiplier;

#include "../AppMacros.h"
extern TargetDevice targetDevice;

#include "../Entities/Tutorials/TutGacha.h"
#include "../Helpers/FlurryCpp.h"
#include "../Helpers/AnalyticsHelper.h"
#include "../Helpers/CustomEventsHelper.h"
#include "../DialogSettings.h"
#include "../Helpers/StringHelper.h"
#include "../UIs/Elements/AlertDialog.h"

#include "../Helpers/FBAppEventsWrapper.h"
#include "../Helpers/AppsFlyerWrapper.h"

GachaPanel* GachaPanel::create() {
    
    GachaPanel *obj = new GachaPanel();
    if (!obj->init()) {
        return NULL;
    }
    obj->autorelease();
    
    return obj;

}

bool GachaPanel::init()
{
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    CCSize _size = CCDirector::sharedDirector()->getWinSize();
    
    if (!CCLayerColor::initWithColor(ccc4(35, 40, 39 ,255), _size.width, _size.height)) {
        return false;
    }
    
    PlayerState *_playerState = PlayerState::getPlayer();

    checkPromotion();

    m_freeSpinBtnEnabled = true;
    m_normalSpinBtnEnabled = true;
    m_premiumSpinBtnEnabled = true;
    m_backBtnEnabled = true;
    
    m_delegate = NULL;
    
    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);

    contentNode = CCNode::create();
    contentNode->setAnchorPoint(ccp(0.5f, 0.5f));
    contentNode->setContentSize(this->getContentSize());
    contentNode->setPosition(this->getContentSize()*0.5f);
    this->addChild(contentNode);
    
    CCSprite *bg = CCSprite::createWithSpriteFrameName("bg_screen.png");
    
    bg->setScale(2.08f);
    
    bg->setPosition(contentNode->getContentSize()*0.5f);
    contentNode->addChild(bg);

    m_glowIns = CCArray::create();
    m_glowIns->retain();
    
    m_starLightIns = CCArray::create();
    m_starLightIns->retain();
    
    m_starLightOuts = CCArray::create();
    m_starLightOuts->retain();
    
    m_glowOuts = CCArray::create();
    m_glowOuts->retain();

    animateStarLights();

    createGachaBoxes();
    createGachaButtons();

    backButton = CCNikButton::create(CCSprite::createWithSpriteFrameName("oldversion/_mini_button_close_vasili.png"), NULL);
    backButton->setTarget(this, menu_selector(GachaPanel::onBackBtnPressed), CCNikButtonEventTouchUpInside);
    backButton->setPosition(ccp(_screenSize.width - backButton->getContentSize().width*0.5 - margin,
                                _screenSize.height - backButton->getContentSize().height*0.5 - margin));
    backButton->setTouchPriority(0);
    this->addChild(backButton);
    
    contentNode->setScale(this->getContentSize().width/_screenSize.width);
    
    // Gems
    int m_gemValue = _playerState->player.balance.gem;
    m_gemButton = ResourceProgressButton::create(UIHelper::getSprite(UIHelper::HUDGem));
    m_gemButton->setPosition(ccp(m_gemButton->getContentSize().width*0.5f
                                 + 40.0f*sizeMultiplier,
                                 this->getContentSize().height
                                 - 60.0f*sizeMultiplier));
    m_gemButton->setTarget(this, menu_selector(GachaPanel::onGemsBarPressed), CCNikButtonEventTouchUpInside);
    this->addChild(m_gemButton);
    m_gemButton->setAmount(m_gemValue);
    m_gemButton->setProgressBarEnable(false);
    m_gemButton->setMaxLevelLabelShowed(false);

    CCSprite *_hudAddGem = UIHelper::getSprite(UIHelper::HUDAdd);
    _hudAddGem->setPosition(ccp(m_gemButton->getContentSize().width,
                                m_gemButton->getContentSize().height*0.5f));
    m_gemButton->addChild(_hudAddGem);
    m_gemButton->setLabelOffsetX(_hudAddGem->getContentSize().width*0.5f);
    
    m_gotItems.clear();
    m_gotPets.clear();
    
    titleNode = CCNode::create();
    titleNode->setScale(0.8f*isIPhoneMultiplier + 1.0f*isIPadMultiplier);
    this->addChild(titleNode);
    titleNode->setPosition(_screenSize.width / 2.f, _screenSize.height * 0.88f);
    
    auto ribbon = UIHelper::getScalableSprite(UIHelper::Ribbon);
    ribbon->setContentSize(CCSizeMake(ribbon->getContentSize().width + 120.0f*sizeMultiplier,
                                      ribbon->getContentSize().height));
    titleNode->addChild(ribbon);
    
    auto *luckyBoxTitleSprite = UIHelper::getSprite(UIHelper::UISprites(UIHelper::GachaTitleText));
    luckyBoxTitleSprite->setPosition(ccp(0.0f, 15.0f*sizeMultiplier));
    titleNode->addChild(luckyBoxTitleSprite);

    m_promotionRibbon = UIHelper::getScalableSprite(UIHelper::Ribbon);
    m_promotionText = CCLabelHelper::createBMFont("Special one time offer!", kFontSizeExtraLarge+10, true, kTextColorYellow);
    m_promotionRibbon->addChild(m_promotionText);
    this->addChild(m_promotionRibbon);

    m_promotionRibbon->setPosition(_screenSize.width / 2.f, _screenSize.height * 0.09f);
    m_promotionRibbon->setContentSize(ccp(m_promotionText->getContentSize().width*1.7f, m_promotionRibbon->getContentSize().height));

    m_promotionText->setPosition(m_promotionRibbon->getContentSize()*0.5f + ccp(0, 15*sizeMultiplier));

    helpTextLabel = CCLabelHelper::createBMFont(g_gameContent->Word["gachapanel_footer_line_1"].c_str(), kFontSizeLarge, true);
    helpTextLabel->setPosition(_screenSize.width / 2.f, _screenSize.height * 0.1f);
    this->addChild(helpTextLabel);

    helpTextSmallLabel = CCLabelHelper::createBMFont(g_gameContent->Word["gachapanel_footer_line_2"].c_str(), kFontSizeMedium, true);
    helpTextSmallLabel->setPosition(_screenSize.width / 2.f, _screenSize.height * 0.05f);
    this->addChild(helpTextSmallLabel);

    if (!isPromotion())
    {
        helpTextLabel->setVisible(true);
        helpTextSmallLabel->setVisible(true);
        m_promotionRibbon->setVisible(false);
        CustomEventsHelper::Instance().onVisitedGatchaScreenWithoutPromotion(isPremiumBoxUnlocked());
    }
    else
    {
        helpTextLabel->setVisible(false);
        helpTextSmallLabel->setVisible(false);
        m_promotionRibbon->setVisible(true);
    }

    oldGems = _playerState->player.balance.gem;
    
    scheduleOnce(schedule_selector(GachaPanel::_checkItemCapacity), 1.0f);
    
    this->scheduleUpdate();
    
    paticleRandom();

    g_gameAudio->preloadEffects(GameAudioService::Gacha);

    return true;
}

void GachaPanel::onExit()
{
    g_gameAudio->unloadEffects(GameAudioService::Gacha);
    cocos2d::CCLayerColor::onExit();
}

GachaPanel::Promotion GachaPanel::getPromotion(int originalPrice) const
{
    CCAssert(CustomEventsHelper::Instance().isOneTimeGatchaOffer(), "Asking for promotion when no promotion!");

    Promotion p;
    p.percent = int(g_gameContent->GameConfig["GACHA_DISCOUNT_RATE"] * 100.0f);
    p.oneTimeOffer = true;
    p.originalPrice = originalPrice;

    return p;
}

void GachaPanel::checkPromotion()
{
    m_promotion = false;
    m_promotionUsed = false;

    if (CustomEventsHelper::Instance().isOneTimeGatchaOffer())
    {
        float price = g_gameContent->GameConfig[!isPremiumBoxUnlocked() ? "NORMAL_GACHA_PRICE" : "PREMIUM_GACHA_PRICE"];

        PlayerState *_playerState = PlayerState::getPlayer();

        if (_playerState->player.balance.gem < getPromotion(price).getNewPrice())
        {
            m_promotion = true;
        }
    }
}

void GachaPanel::applyPromotion()
{
    CustomEventsHelper::Instance().onOneTimeGatchaOffer();
    checkPromotion();
    m_promotionUsed = true;
}

void GachaPanel::createGachaBoxes()
{
    CCSize screenSize = CCDirector::sharedDirector()->getWinSize();

    m_gachaBoxFree = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ball_gacha_open");
    m_gachaBoxNormal = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ball_gacha_open");
    m_gachaBoxPremium = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ball_gacha_open");
    m_gachaBoxOpen = g_sceneManager->sharedSkeletonData->getSkeleton("common/vfx/", "ball_gacha_open");

    float boxHeight = (screenSize.height/2.f) - (350.f * sizeMultiplier);
    float widthCenter = screenSize.width/2.f;
    float offset = screenSize.width/3.5f;
    m_gachaBoxFree->setPosition(ccp(widthCenter - offset, boxHeight));
    m_gachaBoxOpen->setPosition(ccp(widthCenter, boxHeight));

    this->addChild(m_gachaBoxFree, 0);

    if(isPremiumBoxUnlocked())
    {
        m_gachaBoxNormal->setPosition(ccp(widthCenter + offset, boxHeight));
        m_gachaBoxPremium->setPosition(ccp(widthCenter, boxHeight));
        this->addChild(m_gachaBoxNormal, 0);
        this->addChild(m_gachaBoxPremium, 0);
    }
    else
    {
        m_gachaBoxNormal->setPosition(ccp(widthCenter, boxHeight));
        m_gachaBoxPremium->setPosition(ccp(widthCenter + offset, boxHeight));
        this->addChild(m_gachaBoxPremium, 0);
        this->addChild(m_gachaBoxNormal, 0);
    }

    this->addChild(m_gachaBoxOpen, 0);

    m_gachaBoxFree->setScale(sizeMultiplier * 0.85f);
    m_gachaBoxNormal->setScale(sizeMultiplier * 0.85f);
    m_gachaBoxPremium->setScale(sizeMultiplier * 0.85f);
    m_gachaBoxOpen->setScale(sizeMultiplier * 0.85f);

    m_gachaBoxFree->clearAnimation();
    m_gachaBoxFree->setAnimation("gacha_brown_free", true);

    m_gachaBoxNormal->clearAnimation();
    m_gachaBoxNormal->setAnimation("gacha_blue_normal", true);

    m_gachaBoxPremium->clearAnimation();
    m_gachaBoxPremium->setAnimation(isPremiumBoxUnlocked() ? "gacha_red_premium" : "gacha_red_premium_inactive", true);
    
    if (!isPremiumBoxUnlocked()) {
        int blackness = 50;
        m_gachaBoxPremium->setColor(ccc3(blackness, blackness, blackness));
        
        CREATE_CHAR_BUFFER(_buffer, 64);
        snprintf(_buffer, _buffer_size, g_gameContent->Word["gachapanel_premium_locked_label"].c_str(), premiumBoxUnlockLevel());
        m_lockedLabel = CCLabelHelper::createBMFont(_buffer, kFontSizeLarge, true);
        m_lockedLabel->setWidth(300.0f*sizeMultiplier);
        m_lockedLabel->setAlignment(kCCTextAlignmentCenter);
        m_lockedLabel->setPosition(m_gachaBoxPremium->getPosition() + ccp(0.0f,
                                                                        400.0f*sizeMultiplier));
        this->addChild(m_lockedLabel);
    }

    m_gachaBoxOpen->setVisible(false);
}

void GachaPanel::createGachaButtons()
{
    float buttonHeight = 100.f;
    
    freeButton = createGachaButtons(true, 0, false, PlayerState::getPlayer()->player.info.free_gacha);
    freeButton->setTarget(this, menu_selector(GachaPanel::onFreeBtnPressed), CCNikButtonEventTouchUpInside);
    freeButton->setPosition(ccp(0, buttonHeight));
    m_gachaBoxFree->addChild(freeButton);

    bool normalPromotion = isPromotion() && !isPremiumBoxUnlocked();
    bool premiumPromotion = isPromotion() && isPremiumBoxUnlocked();

    if (normalPromotion)
    {
        normalButton = createGachaPromotionButton(getPromotion(static_cast<int>(g_gameContent->GameConfig["NORMAL_GACHA_PRICE"])));
        normalButton->setPosition(ccp(0, 170));
    }
    else
    {
        normalButton = createGachaButtons(!PlayerState::getPlayer()->Checklist[TutGacha::key()],
                                            static_cast<int>(g_gameContent->GameConfig["NORMAL_GACHA_PRICE"]),
                                            false);
        normalButton->setPosition(ccp(0, buttonHeight));
    }
    normalButton->setTarget(this, menu_selector(GachaPanel::onNormalBtnPressed), CCNikButtonEventTouchUpInside);
    m_gachaBoxNormal->addChild(normalButton);

    if (premiumPromotion)
    {
        premiumButton = createGachaPromotionButton(getPromotion(static_cast<int>(g_gameContent->GameConfig["PREMIUM_GACHA_PRICE"])));
        premiumButton->setPosition(ccp(0, 170));
    }
    else
    {
        premiumButton = createGachaButtons(false,
                                            static_cast<int>(g_gameContent->GameConfig["PREMIUM_GACHA_PRICE"]),
                                            !isPremiumBoxUnlocked());
        premiumButton->setPosition(ccp(0, buttonHeight));
    }

    premiumButton->setTarget(this, menu_selector(GachaPanel::onPremiumBtnPressed), CCNikButtonEventTouchUpInside);
    m_gachaBoxPremium->addChild(premiumButton);
    
    premiumButton->setEnabled(isPremiumBoxUnlocked());
    //premiumButton->setEnabled(true);
    setFreeSpinEnabled(isFreeBoxAvailable());
}

CCNikButton* GachaPanel::createGachaButtons(bool isFree, int price, bool locked)
{
    return createGachaButtons(isFree, price, locked, 0);
}

CCNikButton* GachaPanel::createGachaButtons(bool isFree, int price, bool locked, int badge)
{
    CCSprite *buttonSprite = CCSprite::createWithSpriteFrameName("oldversion/_button_action_green.png");
    CCSprite *buttonSpriteDisabled = CCSprite::createWithSpriteFrameName("oldversion/_button_action_green_disable.png");
    CCNikButton* button = CCNikButton::create(buttonSprite, NULL);

    if (targetDevice != TD_IPAD_RETINA)
    {
        button->setScale(1.8f);
    }

    button->setDisableSprite(buttonSpriteDisabled);
    button->setPosition(ccp(0.f, 50.f));

    if(locked) {
        CCSprite *lockSprite = CCSprite::createWithSpriteFrameName("oldversion/_button_key_lock.png");
        lockSprite->setPosition(ccp(button->getContentSize().width * 0.5f,
                                    button->getContentSize().height * 0.5f));
        button->addChild(lockSprite);
        return button;
    }

    CCLabelBMFont* priceLabel = CCLabelHelper::createBMFont(isFree ? "FREE" : CCString::createWithFormat("%d", price)->getCString(),
                                                            kFontSizeExtraLarge*1.4f,
                                                            true,
                                                            ccc3(255, 255, 255));
    button->addChild(priceLabel);

    if(!isFree) {
        CCSprite *gemSprite = UIHelper::getSprite(UIHelper::HUDGem);
        gemSprite->setScale(0.75f);
        gemSprite->setPosition(ccp(button->getContentSize().width * 0.5f,
                                   button->getContentSize().height * 0.7f));
        button->addChild(gemSprite);
        
        priceLabel->setPosition(ccp(button->getContentSize().width * 0.5f,
                                    button->getContentSize().height * 0.34f));
      } else {
        priceLabel->setPosition(ccp(button->getContentSize().width*0.5,
                                    button->getContentSize().height*0.5));
    }

    if (badge > 0) {
        CREATE_CHAR_BUFFER(buffer, 32);
        CCSprite *badgeSprite = UIHelper::getSprite(UIHelper::HUDNewMarkerBig);
        badgeSprite->setPosition(ccp(button->getContentSize().width*0.85f,
                                      button->getContentSize().height*0.85f));
        button->addChild(badgeSprite);
        
        snprintf(buffer, buffer_size, "%d", badge);
        CCLabelBMFont *_badgeLabel = CCLabelHelper::createBMFont(buffer, kFontSizeLarge-5.0f, true);
        _badgeLabel->setPosition(badgeSprite->getContentSize()*0.5f);
        badgeSprite->addChild(_badgeLabel);
    }

    return button;
}

CCNikButton* GachaPanel::createGachaPromotionButton(const Promotion& promotion)
{
    CCSprite *buttonSprite = UIHelper::getSprite(UIHelper::GachaPromotionButton);
    CCNikButton *button = CCNikButton::create(buttonSprite, NULL);
    auto glow = UIHelper::getSprite(UIHelper::GachaPromotionGlow);
    glow->setPosition(button->getContentSize()*0.5f);
    button->addChild(glow, -5);

    if (targetDevice != TD_IPAD_RETINA)
    {
        button->setScale(1.8f);
    }

    glow->setScale(1.8f);

    auto promotionPercent = UIHelper::getSprite(UIHelper::GachaPromotionMarker);
    std::stringstream ss;
    ss << "-" << promotion.percent << "%";
    auto promotionPercentLabel = CCLabelHelper::createBMFont(ss.str().c_str(), kFontSizeLarge, true);
    promotionPercentLabel->setPosition(promotionPercent->getContentSize()*0.5f);
    promotionPercent->addChild(promotionPercentLabel);
    promotionPercent->setPosition(button->getContentSize()*0.5f + ccp(button->getContentSize().width*0.5f, button->getContentSize().height*0.5f));
    button->addChild(promotionPercent);

    CCLabelBMFont *oldPriceTitle = CCLabelHelper::createBMFont("WAS", kFontSizeVeryLarge, true, ccc3(255, 255, 255));
    CCLabelBMFont *oldPrice = CCLabelHelper::createBMFont(CCString::createWithFormat("%d", promotion.originalPrice)->getCString(),
                                                          kFontSizeVeryLarge,
                                                          true,
                                                          ccc3(255, 255, 255));
    auto stroke = UIHelper::getSprite(UIHelper::GachaPromotionStroke);

    button->addChild(oldPrice);
    button->addChild(oldPriceTitle);
    button->addChild(stroke);

    CCSprite *gemSprite = UIHelper::getSprite(UIHelper::HUDGem);
    gemSprite->setScale(0.5f);
    button->addChild(gemSprite);

    oldPriceTitle->setAnchorPoint(ccp(1.0f, 0.5f));
    oldPrice->setPosition(ccp(button->getContentSize().width * 0.5f, button->getContentSize().height * 0.7f));
    oldPriceTitle->setPosition(oldPrice->getPosition() - ccp(oldPrice->getContentSize().width*0.5f + 50.0f*sizeMultiplier, 0.0f));
    gemSprite->setPosition(oldPrice->getPosition() + ccp(oldPrice->getContentSize().width*0.5f + 40.0f*sizeMultiplier, 0.0f));
    stroke->setPosition(oldPrice->getPosition() + ccp(sizeMultiplier*10, sizeMultiplier*10));

    CCLabelBMFont *newPriceTitle = CCLabelHelper::createBMFont("NOW ONLY", kFontSizeVeryLarge, true, ccc3(255, 255, 255));
    CCLabelBMFont *newPrice = CCLabelHelper::createGatchaFont(StringHelper::toString(promotion.getNewPrice()).c_str(), 25);

    button->addChild(newPrice);
    button->addChild(newPriceTitle);

    CCSprite *newGemSprite = UIHelper::getSprite(UIHelper::GachaPromotionGem);
    newGemSprite->setScale(1.3f);
    button->addChild(newGemSprite);

    newPriceTitle->setAnchorPoint(ccp(1.0f, 0.5f));
    newPrice->setPosition(ccp(button->getContentSize().width * 0.5f, button->getContentSize().height * 0.33f));
    newPriceTitle->setPosition(ccp(oldPriceTitle->getPositionX(), newPrice->getPositionY()));
    newGemSprite->setPosition(newPrice->getPosition() + ccp(130.0f*sizeMultiplier, 0.0f));

    return button;
}

void GachaPanel::showAcquiredPet()
{
    //Ake Change
    m_gachaBoxOpen->runAction(CCSequence::create(CCFadeOut::create(0.5f),
   //m_gachaBoxOpen->runAction(CCSequence::create(CCFadeOut::create(1.5f),
                                                 CCHide::create(),
                                                 NULL));
    
    GDPetVO petVO = m_gotPets.back();
    GachaResult *_result = GachaResult::create(petVO, this);
    _result->presentResult(this, true);
}

void GachaPanel::showAcquiredItem()
{
    m_gachaBoxOpen->runAction(CCSequence::create(CCFadeOut::create(0.5f),
    //Ake Change
    //m_gachaBoxOpen->runAction(CCSequence::create(CCFadeOut::create(1.5f),
                                                 CCHide::create(),
                                                 NULL));

    GDItemVO itemVO = m_gotItems.back();
    GachaResult *_result = GachaResult::create(itemVO, this);
    _result->presentResult(this, true);
}

void GachaPanel::onResultDismissed()
{
    freeButton->removeFromParent();
    normalButton->removeFromParent();
    premiumButton->removeFromParent();

    createGachaButtons();
    showGachaBoxes();
    enableButtons();
    checkPromotion();
}

void GachaPanel::showGachaBoxes()
{
    titleNode->setVisible(true);
    
    m_gemButton->setEnabled(true);
    backButton->setEnabled(true);

    m_gachaBoxFree->setVisible(true);
    freeButton->setEnabled(isFreeBoxAvailable());
    freeButton->setVisible(true);

    m_gachaBoxNormal->setVisible(true);
    normalButton->setEnabled(true);
    normalButton->setVisible(true);

    m_gachaBoxPremium->setVisible(true);
    premiumButton->setEnabled(isPremiumBoxUnlocked());
    premiumButton->setVisible(true);

    if (!isPromotion())
    {
        helpTextLabel->setVisible(true);
        helpTextSmallLabel->setVisible(true);
        m_promotionRibbon->setVisible(false);
    }
    else
    {
        helpTextLabel->setVisible(false);
        helpTextSmallLabel->setVisible(false);
        m_promotionRibbon->setVisible(true);
    }

    if (m_lockedLabel)
        m_lockedLabel->setVisible(!isPremiumBoxUnlocked());
}

bool GachaPanel::isPremiumBoxUnlocked()
{
    int playerLevel = PlayerState::getInstance()->player.racing.level;
    return playerLevel >= premiumBoxUnlockLevel();
}

int GachaPanel::premiumBoxUnlockLevel()
{
    return static_cast<int>(GameContentService::getInstance()->GameConfig["PREMIUM_GACHA_UNLOCK_AT_LEVEL"]);
}

bool GachaPanel::isFreeBoxAvailable()
{
    return (PlayerState::getPlayer()->player.info.free_gacha > 0);
}


bool GachaPanel::isBarnFull()
{
    return PlayerState::getPlayer()->checkAndReturnIfItemBarnIsFull();
}


void GachaPanel::_checkItemCapacity() {
    
    PlayerState::getPlayer()->checkAndPrintIfItemBarnIsFull();
    
}

GachaPanel::~GachaPanel()
{
    if (m_glowIns) {
        m_glowIns->release();
        m_glowIns = NULL;
    }
    if (m_glowOuts) {
        m_glowOuts->release();
        m_glowOuts = NULL;
    }
    
    if (m_starLightIns) {
        m_starLightIns->release();
        m_starLightIns = NULL;
    }
    
    if (m_starLightOuts) {
        m_starLightOuts->release();
        m_starLightOuts = NULL;
    }
}

void GachaPanel::update(float delta) {
    
    PlayerState *_playerState = PlayerState::getPlayer();

    if (oldGems == _playerState->player.balance.gem) return;
    
    int m_gemAddingValue = (int)ceil(abs(_playerState->player.balance.gem - oldGems)/15.0f);
    if (oldGems > _playerState->player.balance.gem) {
        oldGems -= m_gemAddingValue;
    }else if (oldGems < _playerState->player.balance.gem) {
        oldGems += m_gemAddingValue;
    }
    if (abs(_playerState->player.balance.gem - oldGems) <= m_gemAddingValue) {
        oldGems = _playerState->player.balance.gem;
    }
    
    updateUI(static_cast<unsigned int>(oldGems));
    
}

void GachaPanel::updateUI(unsigned int numGems)
{
    m_gemButton->setAmount(numGems);
}

void GachaPanel::animateStarLights()
{
    for (int i = 0; i < m_starLightIns->count(); i++) {
        if (arc4random()%8 == 0) {
            CCSprite *light = (CCSprite *)m_starLightIns->objectAtIndex(i);
            CCSprite *lightStar = CCSprite::create("common/vfx/star_light.png");
            lightStar->setScale(0);
            lightStar->setPosition(light->getContentSize()*0.5f +  ccp(arc4random()%2 ? 1 : -1 * light->getContentSize().width*0.4f,
                                                                       arc4random()%2 ? 1 : -1 * light->getContentSize().width*0.4f));
            light->addChild(lightStar);
            
            lightStar->runAction(CCRotateBy::create(0.5f, 360));
            lightStar->runAction(CCSequence::create(CCScaleTo::create(0.5f, sizeMultiplier*0.5f),
                                                    CCRemoveSelf::create(),
                                                    NULL));
        }
    }
    
    for (int i = 0; i < m_starLightOuts->count(); i++) {
        if (arc4random()%8 == 0) {
            CCSprite *light = (CCSprite *)m_starLightIns->objectAtIndex(i);
            CCSprite *lightStar = CCSprite::create("common/vfx/star_light.png");
            lightStar->setScale(0);
            lightStar->setPosition(light->getContentSize()*0.5f +  ccp(arc4random()%2 ? 1 : -1 * light->getContentSize().width*0.4f,
                                                                       arc4random()%2 ? 1 : -1 * light->getContentSize().width*0.4f));
            light->addChild(lightStar);
            
            lightStar->runAction(CCRotateBy::create(0.5f, 180 + arc4random()%180));
            lightStar->runAction(CCSequence::create(CCScaleTo::create(0.3f, sizeMultiplier*0.5f),
                                                    CCScaleTo::create(0.2f, sizeMultiplier*0.2f),
                                                    CCRemoveSelf::create(),
                                                    NULL));
        }
    }
    
    this->runAction(CCSequence::create(CCDelayTime::create(0.5f + (arc4random()%100)/100.f),
                                       CCCallFunc::create(this, callfunc_selector(GachaPanel::animateStarLights)),
                                       NULL));
}

void GachaPanel::spinForPet(const char* type)
{
    CCAssert(string(type) == "normal" || string(type) == "premium", "invalid gacha type");
    PlayerState *_playerState = PlayerState::getPlayer();
    
    float price = g_gameContent->GameConfig[(string(type) == "normal") ? "NORMAL_GACHA_PRICE" : "PREMIUM_GACHA_PRICE"];
    float promotion = 1.0f;
    bool isProm = false;

    if (isPromotion())
    {
        if ((string(type) == "normal" && !isPremiumBoxUnlocked()) ||
            (string(type) == "premium" && isPremiumBoxUnlocked()))
        {
            auto prom = getPromotion(price);
            promotion = prom.percent/100.0f;
            isProm = true;
        }
    }

    CCLog_("Gacha promotion: %f %f %f", price, promotion, price*promotion);
    price *= promotion;

    if (_playerState->player.balance.gem < price &&
        _playerState->Checklist[TutGacha::key()])
    {
        
        SceneVille* _scene = (SceneVille*)g_currentScene;
        if (!_scene || _scene->modalController->getNStackView() != 0) return;

        CCDictionary *params = CCDictionary::create();
        params->setObject(CCString::create((string(type) == "normal") ? "normalGacha" : "premiumGacha"), "Source");
        FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, params);
        
        CSJson::Value eventParameters;
        eventParameters["source"] = (string(type) == "normal") ? "normalGacha" : "premiumGacha";
        AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
        
        CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
        IAPDialog *_dialog = IAPDialog::create(_scene->modalController, RESOURCE_TYPE_GEM);
        _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
        _scene->addChild(_scene->modalController, 20000);
        _scene->modalController->pushView(_dialog);

        if (isProm)
        {
            _dialog->setCallback(this, GachaPanel::onIAPSuccess);
        }

        return;
    }

    int size = 0;
    for (int i = 0; i < _playerState->Pets.size(); i++) {
        Pet *pet = (Pet *)_playerState->Pets[i];
        if (pet->ppVo.located_in == "") {
            size++;
        }
    }
    
    int nslot = 0;
    Building *animalDorm = _playerState->getBuilding(kBuildingAnimal, 0);
    if (animalDorm)
        nslot = static_cast<int>(animalDorm->bVo.param_1);
    if (size >= nslot) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                                                 g_gameContent->Word["animal_barn_is_full_msg"].c_str());
        return;
    }

    g_gameAudio->playEffect(AUDIO_FINISH_NOW);
    g_gameAudio->playEffect(AUDIO_FINISH_NOW);

    if (_playerState->Checklist[TutGacha::key()]) {
        
        CCDictionary* params = CCDictionary::create();
        
        params->setObject(CCString::createWithFormat("%s", type), "gachaType");
        int gachaPrice = (int)price;
        params->setObject(CCString::createWithFormat("%d", gachaPrice), "gachaPrice");
        
        FlurryCpp::logEvent(EVENT_GEM_GACHA, params);
        
        CSJson::Value eventParameters;
        eventParameters["gachaType"] = type;
        eventParameters["gachaPrice"] = gachaPrice;
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_GACHA, eventParameters);
        
    }

    disableButtons();
    g_sceneManager->showLoading();

    BackendService::getInstance()->send(APIGachaGet::create(type, false, this, GachaPanel::onGachaReturn));
}

void GachaPanel::onIAPSuccess(void* sender)
{
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;

    GachaPanel* panel = static_cast<GachaPanel*>(sender);

    if (panel)
    {
        panel->buyAnimalWithPromotion();
    }
}

void GachaPanel::buyAnimalWithPromotion()
{
    CCAssert(isPromotion(), "Buying animal without promotion!");

    const char* type = isPremiumBoxUnlocked()?"premium":"normal";
    float price = g_gameContent->GameConfig[(string(type) == "normal") ? "NORMAL_GACHA_PRICE" : "PREMIUM_GACHA_PRICE"];
    PlayerState *_playerState = PlayerState::getPlayer();
//    CCAssert(_playerState->player.balance.gem > getPromotion(price).getNewPrice(), "Buying animal with lower price!");

    int size = 0;
    for (int i = 0; i < _playerState->Pets.size(); i++) {
        Pet *pet = (Pet *)_playerState->Pets[i];
        if (pet->ppVo.located_in == "") {
            size++;
        }
    }

    int nslot = 0;
    Building *animalDorm = _playerState->getBuilding(kBuildingAnimal, 0);
    if (animalDorm)
        nslot = static_cast<int>(animalDorm->bVo.param_1);
    if (size >= nslot) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                g_gameContent->Word["animal_barn_is_full_msg"].c_str());
        return;
    }

    g_gameAudio->playEffect(AUDIO_FINISH_NOW);
    g_gameAudio->playEffect(AUDIO_FINISH_NOW);

    if (_playerState->Checklist[TutGacha::key()]) {

        CCDictionary* params = CCDictionary::create();

        params->setObject(CCString::createWithFormat("%s", type), "gachaType");
        int gachaPrice = (int)price;
        params->setObject(CCString::createWithFormat("%d", gachaPrice), "gachaPrice");

        FlurryCpp::logEvent(EVENT_GEM_GACHA, params);
        
        CSJson::Value eventParameters;
        eventParameters["gachaType"] = type;
        eventParameters["gachaPrice"] = gachaPrice;
        AnalyticsClient::getInstance()->logEvent(EVENT_GEM_GACHA, eventParameters);
    }

    disableButtons();
    g_sceneManager->showLoading();
    applyPromotion();

    BackendService::getInstance()->send(APIGachaGet::create(type, true, this, GachaPanel::onGachaReturn));
}

void GachaPanel::openGachaBox(const char *animName, SEL_CallFunc selector)
{
    titleNode->setVisible(false);
    
    m_gachaBoxFree->setVisible(false);
    m_gachaBoxNormal->setVisible(false);
    m_gachaBoxPremium->setVisible(false);
    
    if (m_lockedLabel)
        m_lockedLabel->setVisible(false);
    
    helpTextLabel->setVisible(false);
    helpTextSmallLabel->setVisible(false);
    m_promotionRibbon->setVisible(false);
    m_promotionRibbon->setVisible(false);

    m_gachaBoxOpen->setOpacity(255);
    m_gachaBoxOpen->setVisible(true);
    m_gachaBoxOpen->clearAnimation();
    m_gachaBoxOpen->setAnimation(animName, false);

    float duration = m_gachaBoxOpen->states[0]->animation->duration;
    m_gachaBoxOpen->runAction(CCSequence::create(CCDelayTime::create(duration), CCCallFunc::create(this, selector), NULL));
    
}

bool GachaPanel::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    CCPoint point = this->convertTouchToNodeSpace(touch);
    CCRect rect = this->boundingBox();
    return rect.containsPoint(point);
}

#pragma mark - Class Funcitons

void GachaPanel::_updateFreeButtonIdleState()
{
    if (!freeButton->isEnabled() || !m_freeSpinBtnEnabled) {
        freeButton->stopAllActions();
        return;
    }
    if (CustomEventsHelper::Instance().shouldAnimateIdle(kIdleAnimationFreeItem)) {
        freeButton->runAction(CustomEventsHelper::Instance().getIdleAnimationWithOriginalScale(freeButton->getScale(),
                                                                                               0.01f,
                                                                                               kDefaultIdleAnimationDelay,
                                                                                               kDefaultIdleAnimationScaleDifference));
    }
}

void GachaPanel::setFreeSpinEnabled(bool enabled)
{
    if (!freeButton) return;

    freeButton->setEnabled(enabled);
    _updateFreeButtonIdleState();
}

void GachaPanel::setFreeSpinButtonEnabled(bool enabled)
{
    m_freeSpinBtnEnabled = enabled;
    _updateFreeButtonIdleState();
}

#pragma mark - Core functions

void GachaPanel::presentPanel(cocos2d::CCNode *onNode)
{
    onNode->addChild(this, 1000);
    this->setOpacity(0);
    this->runAction(CCFadeIn::create(0.3f));
    CCModalController::showAllChildrensAnimated(this, 0.3f);
}

void GachaPanel::dismissPanel()
{
    CCModalController::hideAllChildrensAnimated(this, 0.3f);
    this->runAction(CCSequence::create(CCFadeOut::create(0.3f),
                                       CCRemoveSelf::create(),
                                       NULL));

    if (m_delegate) {
        m_delegate->gachaPanelDidClosing(this);
    }
}

void GachaPanel::disableButtons()
{
    backButton->setEnabled(false);
    m_gemButton->setEnabled(false);
    setFreeSpinButtonEnabled(false);
    setNormalSpinButtonEnabled(false);
    setPremiumSpinButtonEnabled(false);
}

void GachaPanel::enableButtons()
{
    backButton->setEnabled(true);
    m_gemButton->setEnabled(true);
    setFreeSpinButtonEnabled(true);
    setNormalSpinButtonEnabled(true);
    setPremiumSpinButtonEnabled(true);
}

#pragma mark - Button callback

void GachaPanel::onFreeBtnPressed(cocos2d::CCObject *sender)
{
    if (!m_freeSpinBtnEnabled) return;
    
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    PlayerState *_playerState = PlayerState::getPlayer();
    
    if (_playerState->player.info.free_gacha <= 0) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                                                 g_gameContent->Word["no_free_spin_msg"].c_str());
        return;
    }
    
    Building *_itemStorage = _playerState->getBuilding(kBuildingItem, 0);
    
    if (isBarnFull()) {
        g_sceneManager->notification->addMessage(MSGN_TYPE_ERROR,
                                                 g_gameContent->Word["item_barn_is_full_msg"].c_str());
        return;
    }
    
    CustomEventsHelper::Instance().checkAndIncreaseTimesUsedFor(kIdleAnimationFreeItem);
    
    disableButtons();
    g_sceneManager->showLoading();
    
    BackendService::getInstance()->send(APIGachaGet::create("free", 1.0f, this, GachaPanel::onGachaReturn));
    
}

void GachaPanel::onNormalBtnPressed(CCObject *sender)
{
    if (!m_normalSpinBtnEnabled) return;
    spinForPet("normal");
}

void GachaPanel::onPremiumBtnPressed(CCObject *sender)
{
    if (!m_premiumSpinBtnEnabled) return;
    spinForPet("premium");
}

void GachaPanel::onEndDialog()
{
    SceneVille *_scene = (SceneVille *) g_currentScene;
    if (!_scene) return;
    if (_scene->modalController && _scene->modalController->getNStackView() > 1) return;

    PlayerState *_playerState = PlayerState::getPlayer();

    if (!m_backBtnEnabled || !_playerState->Checklist[TutGacha::key()]) {
        g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
        return;
    }
    
    if (isPromotion()) {
        
        CSJson::Value eventParameters;
        
        eventParameters["promotionUsed"] = m_promotionUsed;
        
        if (!isPremiumBoxUnlocked()) {
            eventParameters["promotionType"] = "normal";
        } else {
            eventParameters["promotionType"] = "premium";
        }
        
        AnalyticsClient::getInstance()->logEvent(EVENT_ONE_TIME_PROMO_GACHA, eventParameters);
    }

    backButton->setEnabled(false);

    g_gameAudio->stopAllEffect();
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    g_gameAudio->playEffectFadeOutAndFadeIn(AUDIO_AMBIENCE_FARM, true, 2.5f);
    g_gameAudio->playMusicFadeOutAndDelayBeforeFadeIn(MUSIC_VIL, true, 2.0, 2.5f, 3.0);

    freeButton->removeFromParent();
    normalButton->removeFromParent();
    premiumButton->removeFromParent();

    dismissPanel();


    if (m_gotPets.size() > 0) {
        if (Building *building = _playerState->getBuilding(kBuildingAnimal, 0)) {
            Building *_available_habitat = _playerState->getAvailableHabitat();

            if (m_gotPets.size() == 1 &&
                    _available_habitat &&
                    m_lastPet_puid != "") {

                _scene->scrollView->zoomOnPosition(_available_habitat->getPosition() - _scene->isoMap->getContentSize() / 4.f
                                + ccp(0, _available_habitat->bVo.src_height * sizeMultiplier * 0.5f),
                        1.3f,
                        1.0f);

                SceneVille *_scene = (SceneVille *) g_currentScene;
                _scene->isoMap->addPetToBuilding(m_lastPet_puid, _available_habitat->pbVo.buid);

                BackendService::getInstance()->send(APIPetRetrieve::create(_playerState->getPet(m_lastPet_puid),
                                                                           _available_habitat,
                                                                           g_sceneManager,
                                                                           SceneManager::onPetRetrieved));

            } else {
                _scene->scrollView->zoomOnPosition(building->getPosition() - _scene->isoMap->getContentSize() / 4.f
                                + ccp(0, building->bVo.src_height * sizeMultiplier * 0.5f),
                        1.3f,
                        1.0f);

                building->addAnimalToBarn(m_gotPets);
            }

            if (_scene->hudPanel) {
                _scene->hudPanel->resourceBars[HUDPanelBarGems]->addTargetAmount(_playerState->player.balance.gem);
                _scene->hudPanel->resourceBars[HUDPanelBarGems]->processTargetAmount();
            }

        }
    }

    if (m_gotItems.size() > 0) {
        if (Building *building = _playerState->getBuilding(kBuildingItem, 0)) {
            _scene->scrollView->zoomOnPosition(building->getPosition() - _scene->isoMap->getContentSize() / 4.f
                            + ccp(0, building->bVo.src_height * sizeMultiplier * 0.5f),
                    1.3f,
                    1.0f);
            building->addItemToBarn(m_gotItems);
        }
        _scene->hudPanel->checkTrade();

    }

    //update Achievement Pet
    _scene->hudPanel->updateBadgeAchievement();
}

void GachaPanel::onBackBtnPressed(CCObject *sender)
{
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;
    if (_scene->modalController && _scene->modalController->getNStackView() > 0) return;

    if (showPromotionAlert()) {
        if (AlertDialog *_dialog = AlertDialog::create(_scene->modalController,
                                                       "Decline One Time Offer?",
                                                       "Are you sure you want to pass on this?",
                                                       "I'm Sure",
                                                       "Re-Think",
                                                       false,
                                                       this, GachaPanel::onAlertDialog)) {
            _dialog->setPosition(_scene->getContentSize() * 0.5f);
            addChild(_scene->modalController, kModalControllerZOrder);
            _scene->modalController->pushView(_dialog);
        }
    }
    else
    {
        onEndDialog();
    }
}

void GachaPanel::onAlertDialog(void *sender_, bool confirm_)
{
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if (!_scene) return;

    GachaPanel* panel = static_cast<GachaPanel*>(sender_);
    
    CSJson::Value eventParameters;
    
    if (!panel)
        return;

    if (confirm_)
    {
        panel->onEndDialog();
        CustomEventsHelper::Instance().onCancelGatchaOneTimeOffer();
        
        //not interested
        eventParameters["rethink"] = false;
        
    } else {
        
        //still interested
        eventParameters["rethink"] = true;
    }
    
    AnalyticsClient::getInstance()->logEvent(EVENT_ONE_TIME_PROMO_GACHA_ALERT_DIALOG, eventParameters);
}

void GachaPanel::onGemsBarPressed(CCObject *sender)
{
    g_gameAudio->playEffect(AUDIO_UI_CLICK);
    
    SceneVille* _scene = (SceneVille*)g_currentScene;
    if(!_scene || _scene->modalController->getNStackView() != 0) return;

    CCDictionary *ccDict = CCDictionary::create();
    ccDict->setObject(CCString::create("GemsTouch at Gacha Panel"), "Source");
    FlurryCpp::logEvent(EVENT_LAUNCH_IAP_STORE, ccDict);
    
    CSJson::Value eventParameters;
    eventParameters["source"] = "GemsTouch at Gacha Panel";
    AnalyticsClient::getInstance()->logEvent(EVENT_LAUNCH_IAP_STORE, eventParameters);
    
    FBAppEventsWrapper::logEvent(EVENT_USER_OPEN_IAP_STORE);
    
    AppsFlyerWrapper::trackEvent(EVENT_USER_OPEN_IAP_STORE);
    
    CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
    IAPDialog *_dialog = IAPDialog::create(_scene->modalController, RESOURCE_TYPE_GEM);
    _dialog->setPosition(_screenSize*0.5f + kDialogIAPPosition);
    this->addChild(_scene->modalController, kModalControllerZOrder);
    _scene->modalController->pushView(_dialog);
}

#pragma mark - Backend Callback

void GachaPanel::onGachaReturn(void *sender, string resp_str) {
    
    g_sceneManager->hideLoading();
    
    PlayerState *_playerState = PlayerState::getPlayer();

    //JsonAllocator allocator;
    Json* root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(root);
    _playerState->parseChecklistData(root);
    _playerState->parseUserState(root);
    _playerState->parsePetData(root);
    _playerState->parseItemData(root);
    Json_dispose(root);

    GachaPanel *_this = (GachaPanel *)sender;
    if (!_this || typeid(*g_currentScene) != typeid(SceneVille)) return;

    //JsonAllocator allocator2;
    Json *_root = Json_create(resp_str.c_str());
    if (!_root) return;
    
    Json* gacha = Json_getItem(_root, "gacha");
    if (!gacha) {
        Json_dispose(_root);
        return;
    }
    
    string _type = Json_getString(gacha, "type", NULL);
    string _result = Json_getString(gacha, "result", NULL);
    
    if (Json *_pets = Json_getItem(_root, "pet")) {
        if (Json *_pet = Json_getItemAt(_pets, 0)) {
            _this->m_lastPet_puid = Json_getString(_pet, "puid", "");
        }
    }
    
    CCLog_("gacha returns: type=%s, result=%s", _type.c_str(), _result.c_str());
    
    if (_type == "free") {
        GDItemVO _item_vo = g_gameContent->getItemVO(_result);
        if (_item_vo.type != "") {
            _this->m_gotItems.push_back(_item_vo);
            _this->openGachaBox("open_brown", callfunc_selector(GachaPanel::showAcquiredItem));
        }
    } else if (_type == "normal" ||
               _type == "premium") {
        GDPetVO _pet_vo = g_gameContent->getPetVO(_result);
        if (_pet_vo.type != "") {
            _this->updateUI(static_cast<unsigned int>(_playerState->player.balance.gem));
            _this->m_gotPets.push_back(_pet_vo);
            if(_type == "normal")
                _this->openGachaBox("open_violet", callfunc_selector(GachaPanel::showAcquiredPet));
            else
                _this->openGachaBox("open_red", callfunc_selector(GachaPanel::showAcquiredPet));
            
            CCDictionary *params = CCDictionary::create();
            params->setObject(CCString::create(_pet_vo.type), "Result Type");
            params->setObject(CCString::create((string(_type) == "normal") ? "normalGacha" : "premiumGacha"), "Source");
            FlurryCpp::logEvent(EVENT_GACHA_SPIN, params);
            
            CSJson::Value eventParameters;
            eventParameters["source"] = (string(_type) == "normal") ? "normalGacha" : "premiumGacha";
            eventParameters["resultType"] = _pet_vo.type;
            AnalyticsClient::getInstance()->logEvent(EVENT_GACHA_SPIN, eventParameters);
        }
    } else {
        CCAssert(false, "Invalid gacha type");
    }
    
    g_gameAudio->playEffect(AUDIO_GACHA_OPEN);
    
    Json_dispose(_root);

}

#pragma mark - Paticle

void GachaPanel::paticleRandom()
{
    for (int i = 0; i < 20; i++) {
        CCSprite *paticleSprite = CCSprite::create("common/vfx/star_particle.png");
        paticleSprite->setScale(sizeMultiplier*0.2f);
        EllipseParicle *paticle = EllipseParicle::create(paticleSprite);
        paticle->startPaticleRandom(this);
    }
}
