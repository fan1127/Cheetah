//
//  IAPDialog.cpp
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#include "IAPDialog.h"

#include "../../Helpers/CCStringHelper.h"
#include "../../Extensions/InAppPurchase/InAppPurchase.h"

USING_NS_CC;
//USING_NS_CC_EXT;

using namespace std;

#include "../../Helpers/CCSpriteHelper.h"
#include "../../Extensions/Modal/CCModalController.h"
#include "../../Extensions/CCMenuHelper.h"
#include "../../Helpers/CreateSpriteHelper.h"
#include "../../Helpers/UIDisplayHelper.h"

#include "../../Scenes/SceneVille.h"
#include "../../Scenes/SceneMatch.h"
#include "../../Scenes/SceneWorldMap.h"
extern CCScene *g_currentScene;

#include "../../SceneManager.h"
extern SceneManager* g_sceneManager;

#include "../../Services/GameContentService.h"
extern GameContentService *g_gameContent;

#include "../../Entities/PlayerState.h"
extern PlayerState* g_playerState;

#include "../../Services/BackendService.h"
#include "../../Services/APIs/APIGemBuy.h"

#include "../../Services/GameAudioService.h"
extern GameAudioService* g_gameAudio;

#include "../../AppMacros.h"

//#include "IAPConfirmDialog.h"
#include "../../VOs/GDIAPVO.h"

#include "../../Helpers/FlurryCpp.h"
#include "../../Helpers/GaiCpp.h"
#include "../../Helpers/StringHelper.h"
#include "../../DialogSettings.h"
#include "../../Helpers/UIHelper.h"

#include "../../../extensions/CCArmature/external_tool/Json/lib_json/value.h"
#include "../../../extensions/CCArmature/external_tool/Json/lib_json/writer.h"

#include "../../Helpers/AnalyticsHelper.h"
#include "../../Helpers/AnalyticsClientWrapper.h"
#include "../../Helpers/CustomEventsHelper.h"

#include "../../Helpers/FBAppEventsWrapper.h"
#include "../../Helpers/AppsFlyerWrapper.h"

#include "../../Helpers/MD5.h"

#define kSpacesBetweenIAPItems (10.0f*sizeMultiplier)

//#define DEBUG_FAKE_IAP

extern TargetDevice targetDevice;
//extern CCPoint screenFactor;
extern float sizeMultiplier;
extern float retinaMultiplier;

extern bool hasPurchasedDuringThisSession;

IAPDialog* IAPDialog::create(CCModalController* parentModalController_, RESOURCE_TYPE scrollToResource_)
{
    IAPDialog *pRet = new IAPDialog();
    pRet->init(parentModalController_, scrollToResource_, kDialogIAPSize);
    pRet->autorelease();
    return pRet;
}

bool IAPDialog::init(CCModalController* parentModalController_, RESOURCE_TYPE scrollToResource_, CCSize size)
{
    if (!ScrollDialog::init(parentModalController_, size))
    {
        return false;
    }

    m_selected_product = NULL;
    m_products = NULL;
    m_callback = NULL;
    m_sender = NULL;
    m_purchaseSuccess = false;
    m_scrollToResource = scrollToResource_;
    
//    m_modalController->onHitShadow = onHitShadow;
    m_modalController->userObject = this;
    _modalController = m_modalController;

    addTitle(g_gameContent->Word["title_get_more_gems"].c_str());

    createScrollContent(IAPP_DIALOG_SETTINGS);

    InAppPurchase::sharedInAppPurchase()->setDelegate(this);
    InAppPurchase::sharedInAppPurchase()->getProduct();
    
#ifdef DEBUG_FAKE_IAP
    updateContent();
#endif
    
    return true;
}

IAPDialog::~IAPDialog()
{
    if (m_products) {
        m_products->release();
        m_products = NULL;
    }
    
    InAppPurchase::sharedInAppPurchase()->setDelegate(NULL);
}

void IAPDialog::onExit()
{
    CCModalView::onExit();
    InAppPurchase::sharedInAppPurchase()->setDelegate(NULL);

    if (m_purchaseSuccess && m_callback && m_sender)
    {
        m_callback(m_sender);
    }
}

bool IAPDialog::isPromotionProduct(ProductObject* product) const
{
    return product && product->productIdentifier->m_sString.find(".sale") != std::string::npos;
}

std::vector<ProductObject*> IAPDialog::getPromotions() const
{
    std::vector<ProductObject*> promotions;

    for (int i = 0; i < m_products->count(); i++)
    {
        ProductObject *product = (ProductObject *) m_products->objectAtIndex(i);

        if (isPromotionProduct(product))
        {
            promotions.push_back(product);
        }
    }

    return promotions;
}

bool IAPDialog::hasPromotion(ProductObject* product, std::vector<ProductObject*> promotions, IAPDialog::Promotion& promotion)
{
    if (!isPromotionProduct(product))
    {
        // compare product ID
        for (auto &p : promotions)
        {
            std::string id0 = product->productIdentifier->m_sString;
            std::string id1 = p->productIdentifier->m_sString;

            size_t idx = id1.find(".sale");

            if (idx != std::string::npos)
            {
                std::string idProm = id1.substr(0, idx);

//                CCLog_("Promotion id: %s %s %d %d", id0.c_str(), idProm.c_str(), id1.length(), idx);

                if (idProm == id0)
                {
                    promotion.original = product;
                    promotion.promotion = p;
                    promotion.orignalPrice = product->price;
                    promotion.percent = (int) ((1.0f - (p->price / product->price)) * 100);
                    promotion.timeLeft = CustomEventsHelper::Instance().getGemPromotionTimer();
                    return true;
                }
            }
        }
    }

    return false;
}

void IAPDialog::updateContent()
{
#ifndef DEBUG_FAKE_IAP
    if (!m_products) return;
#endif
    
    CCNikScrollViewButton *lastButton = NULL;
    m_selected_product = NULL;
    
#ifdef DEBUG_FAKE_IAP
    m_products = CCArray::create();
    for (int i = 0; i < 5; i++)
    {
        ProductObject *object = new ProductObject;
        object->localizedDescription = CCString::create("localizedDescription");
        object->localizedDescription->retain();
        object->localizedTitle = CCString::create("localizedTitle");
        object->localizedTitle->retain();
        object->productIdentifier = CCString::create(std::string("com.meetnlunch.hoofit.gem0")+StringHelper::toString(i+1));
        object->productIdentifier->retain();
        object->priceString = CCString::create("$ 99.99");
        object->priceString->retain();
        object->currencyString = CCString::create("$");
        object->currencyString->retain();
        object->price = 99.99;
        object->gemAmount = 14000;
        m_products->addObject(object);
        object->autorelease();
    }

    ProductObject *object = new ProductObject;
    object->localizedDescription = CCString::create("localizedDescription");
    object->localizedDescription->retain();
    object->localizedTitle = CCString::create("localizedTitle");
    object->localizedTitle->retain();
    object->productIdentifier = CCString::create("com.meetnlunch.hoofit.gem02.sale01");
    object->productIdentifier->retain();
    object->priceString = CCString::create("$ 33.99");
    object->priceString->retain();
    object->currencyString = CCString::create("$");
    object->currencyString->retain();
    object->price = 33.99;
    object->gemAmount = 14000;
    m_products->addObject(object);
    object->autorelease();

    m_products->retain();
#endif
    
    int _buttonPosition = 0;
    bool isPromotion = CustomEventsHelper::Instance().isGemPromotion();
    std::vector<ProductObject*> promotions = getPromotions();
    
    for (int i = 0; i < m_products->count(); i++)
    {
        ProductObject *product = (ProductObject *)m_products->objectAtIndex(i);
        Promotion promotion;

        if (isPromotionProduct(product))
            continue;

        if (isPromotion && hasPromotion(product, promotions, promotion))
        {
            // rewrite the product
            product = promotion.promotion;
        }

        const char* _ribbonText = RIBBON_NONE;
        if (i == 2) {
            _ribbonText = RIBBON_MOSTPOPULAR;
        }
        if (i == 4) {
            _ribbonText = RIBBON_BESTOFFER;
        }
        
        int _gemAmount = product->gemAmount;
        
        for (vector<GDIAPVO>::iterator it=g_gameContent->Inapp.begin(); it!=g_gameContent->Inapp.end(); ++it) {
            GDIAPVO _iapvo = (GDIAPVO)(*it);
            if(_iapvo.product == product->productIdentifier->getCString()) {
                _gemAmount = _iapvo.gems_value;
            }
        }
        
        lastButton = addButton(product->localizedTitle->getCString(),
                               RESOURCE_TYPE_GEM,
                               i+1,
                               m_products->count(),
                               _buttonPosition,
                               _gemAmount,
                               product->priceString->getCString(),
                               _ribbonText,
                                &promotion);

        _buttonPosition++;
    }
    
    CCNikScrollViewButton *lastCoinButton = addResourceButtons(RESOURCE_TYPE_COIN,
                                                               &_buttonPosition);
    if (lastCoinButton) {
        lastButton = lastCoinButton;
    }
    
    CCPoint lastCoinButtonPosition = lastButton->getPosition();
    
    CCNikScrollViewButton *lastFoodButton = addResourceButtons(RESOURCE_TYPE_FOOD,
                                                               &_buttonPosition);
    if (lastFoodButton != NULL) {
        lastButton = lastFoodButton;
    }
    
    if (lastButton) {
        m_scrollView->setContentView(CCSizeMake(MAX(lastButton->getPositionX()
                                                    + lastButton->getContentSize().width
//                                                    + 60*retinaMultiplier
                                                    ,
                                                    m_scrollView->getContentSize().width),
                                                m_scrollView->getContentSize().height));
    }
    
    if (m_scrollToResource == RESOURCE_TYPE_COIN) {
        
        m_scrollView->zoomOnPosition(lastCoinButtonPosition - ccp(lastButton->getContentSize().width*0.5f, 0.0f),
                                     1.0f,
                                     1.0f);
    } else if (m_scrollToResource == RESOURCE_TYPE_FOOD) {
        
        m_scrollView->zoomOnPosition(lastButton->getPosition(),
                                     1.0f,
                                     1.0f);
    }
}

CCNikScrollViewButton* IAPDialog::addResourceButtons(RESOURCE_TYPE resourceType_,
                                                     int *buttonPosition)
{
    CCNikScrollViewButton *lastButton = NULL;
    int buttonPosition_ = *buttonPosition;
    
    float percent = FIRST_STEP_PERCENT;
    int maxItemCount = 3;
    for (int i = 0; i < maxItemCount; i++)
    {
        CREATE_CHAR_BUFFER(_buttonTitleText, 32);

        int buttonOrder = i+1;
        int resourcesUntilStorageIsFull = 0;
        int resourcesToPurchase = getResourceAmountFromTypeAndOrder(resourceType_, buttonOrder);
        
        if (resourceType_ == RESOURCE_TYPE_COIN)
        {
            resourcesUntilStorageIsFull = g_playerState->getTotalCoinStorageCapacity() - g_playerState->player.balance.gold;
            
            strncpy(_buttonTitleText, g_gameContent->Word["iap_fill_coin"].c_str(), _buttonTitleText_size);
        }
        else if (resourceType_ == RESOURCE_TYPE_FOOD)
        {
            resourcesUntilStorageIsFull = g_playerState->getTotalFoodStorageCapacity() - g_playerState->player.balance.food;
            strncpy(_buttonTitleText, g_gameContent->Word["iap_fill_food"].c_str(), _buttonTitleText_size);
        }
        
        bool isLastItem = (buttonOrder == maxItemCount);
        
        if (!isLastItem)
        {
            snprintf(_buttonTitleText, _buttonTitleText_size, g_gameContent->Word["iap_fill_by_percent"].c_str(), (int)(percent*100.0f));
        }
        
        if (!isLastItem &&
            (resourcesToPurchase >= resourcesUntilStorageIsFull))
        {
            continue;
        }
        
        if (resourcesToPurchase <= 0) {
            continue;
        }
        
        int _gemPrice = 0;
        
        if (resourceType_ == RESOURCE_TYPE_COIN)
        {
            _gemPrice = g_gameContent->get_gemcal_for_coins(resourcesToPurchase);
        }
        else if (resourceType_ == RESOURCE_TYPE_FOOD)
        {
            _gemPrice = g_gameContent->get_gemcal_for_foods(resourcesToPurchase);
        }
        
        CREATE_CHAR_BUFFER(_gemPriceText, 32);
        snprintf(_gemPriceText, _gemPriceText_size, "%s", CCStringHelper::createWithNumberFormat(_gemPrice).c_str());
        
        lastButton = addButton(_buttonTitleText,
                               resourceType_,
                               i+1,
                               maxItemCount,
                               buttonPosition_,
                               resourcesToPurchase,
                               _gemPriceText,
                               RIBBON_NONE, NULL);
        
        percent = SECOND_STEP_PERCENT;
        buttonPosition_++;
    }
    
    *buttonPosition = buttonPosition_;
    
    return lastButton;
}

CCNikScrollViewButton* IAPDialog::addButton(const char* buttonTitle_,
                                            RESOURCE_TYPE resourceType_,
                                            int buttonTypeOrder_,
                                            int buttonMax,
                                            int buttonPosition_,
                                            int resourceAmount_,
                                            const char* priceText_,
                                            const char* ribbonText_,
                                            Promotion* promotionInfo)
{
    CCNikScrollViewButton *button = NULL;
    
    button = CCNikScrollViewButton::create(UIHelper::getSprite(UIHelper::PurchaseItemBg), NULL);
    button->setPosition(ccp(button->getContentSize().width*buttonPosition_
                            +(kSpacesBetweenIAPItems*(buttonPosition_)),
                            m_clippingLayer->getContentSize().height*0.5
                            -button->getContentSize().height*0.5));
    button->setTag(buttonTypeOrder_);

    auto shine = UIHelper::getSprite(UIHelper::PurchaseLight);
    shine->setPosition(CCPointMake(button->getContentSize().width*0.5f, button->getContentSize().height*0.5f));
    
    //TODO: opacity gets overwritten by show/hideAllChildrenAnimated, will need to have opacity values by artists providing multiple images
    GLubyte opacity = (float(buttonTypeOrder_) / float(buttonMax)) * 255;
    button->addChild(shine);
    shine->setOpacity(opacity);

    CCLabelBMFont* numGemTxt;
    CCLabelBMFont* priceTxt;
    CCLabelBMFont* productNameTxt;
    CCSprite *_pic;
    CCLabelBMFont* pricePromotion(NULL);
    CCSprite *promotionStroke(NULL);
    CCSprite *promotionPercent(NULL);
    CCSprite *promotionTime(NULL);
    CCLabelBMFont* promotionTimer(NULL);
    CCLabelBMFont* promotionTimerTitle(NULL);
    CCLabelBMFont* promotionPercentLabel(NULL);

    UIHelper::UISprites sprite;

    CREATE_CHAR_BUFFER(_unitText, 32);

    if (resourceType_ == RESOURCE_TYPE_GEM) {
        
        sprite = UIHelper::UISprites(UIHelper::PurchaseGems1 + buttonTypeOrder_-1);

    } else if (resourceType_ == RESOURCE_TYPE_COIN) {

        sprite = UIHelper::UISprites(UIHelper::PurchaseCoins1 + buttonTypeOrder_-1);
        strncpy(_unitText, "Coins", _unitText_size);

    } else if (resourceType_ == RESOURCE_TYPE_FOOD) {

        sprite = UIHelper::UISprites(UIHelper::PurchaseFood1 + buttonTypeOrder_-1);
        strncpy(_unitText, "Bags", _unitText_size);
    }
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    
    numGemTxt = CCLabelHelper::createBMFont(_buffer, kFontSizeExtraLarge+10.0f, true);
    
    if (resourceType_ == RESOURCE_TYPE_GEM) {
        
        snprintf(_buffer, _buffer_size, "x %s", CCStringHelper::createWithNumberFormat(resourceAmount_).c_str());
        numGemTxt->setString(_buffer);
    } else {
        
        snprintf(_buffer, _buffer_size, "%s %s", CCStringHelper::createWithNumberFormat(resourceAmount_).c_str(), _unitText);
        numGemTxt->setString(_buffer);
    }

    if (promotionInfo && promotionInfo->original)
    {
        priceTxt = CCLabelHelper::createBMFont(priceText_, kFontSizeExtraLarge+20, true);
        priceTxt->setColor(ccc3(255, 255, 0));

        pricePromotion = CCLabelHelper::createBMFont(StringHelper::toString(promotionInfo->orignalPrice).c_str(), kFontSizeMedium, true);
        promotionStroke = UIHelper::getSprite(UIHelper::PurchasePromotionCrossout);

        promotionPercent = UIHelper::getSprite(UIHelper::PurchasePromotionMarker);
        promotionTime = UIHelper::getSprite(UIHelper::PurchasePromotionTime);

        std::stringstream ss;
        ss << "-" << promotionInfo->percent << "%";
        promotionPercentLabel = CCLabelHelper::createBMFont(ss.str().c_str(), kFontSizeLarge, true);
        promotionTimerTitle = CCLabelHelper::createBMFont("Time Left", kFontSizeSmall, false);

        CREATE_CHAR_BUFFER(buffer, 64)
        UIDisplayHelper::convertToDisplayTime(promotionInfo->timeLeft, buffer, buffer_size);
        promotionTimer = CCLabelHelper::createBMFont(buffer, kFontSizeMedium, true);

        promotionPercentLabel->setPosition(promotionPercent->getContentSize()*0.5f);
        promotionPercent->addChild(promotionPercentLabel);

        promotionTimer->setPosition(ccp(promotionTime->getContentSize().width*0.4f, promotionTime->getContentSize().height*0.4f));
        promotionTimerTitle->setPosition(promotionTimer->getPosition() + ccp(0.0f, promotionTimer->getContentSize().height*0.5f));

        promotionTimer->setRotation(-16);
        promotionTimerTitle->setRotation(-16);

        promotionTime->addChild(promotionTimer);
        promotionTime->addChild(promotionTimerTitle);

        button->addChild(pricePromotion);
        button->addChild(promotionStroke);
        button->addChild(promotionTime);
        button->addChild(promotionPercent);
    }
    else
    {
        priceTxt = CCLabelHelper::createBMFont(priceText_, kFontSizeLarge, true);
        priceTxt->setColor(ccc3(255, 255, 0));
    }

    productNameTxt = CCLabelHelper::createBMFont(buttonTitle_, 40.0f, true);
    
    float targetWidth = button->getContentSize().width - 40.0f*sizeMultiplier;
    
    if (numGemTxt->getContentSize().width > targetWidth) {
        
        float mult = targetWidth/numGemTxt->getContentSize().width;
        
        numGemTxt->setScale(numGemTxt->getScale()*mult);
    }
    
    _pic = UIHelper::getSprite(sprite);//CCSprite::createWithSpriteFrameName(_buffer);
    if(buttonPosition_==0) {
        _pic->setAnchorPoint(ccp(0.5,0.6));
    }
    
    _pic->setPosition(button->getContentSize()*0.5f);
    button->addChild(_pic);
    
    CCSprite *_gemIcon = UIHelper::getSprite(UIHelper::PurchaseGemIcon);
    _gemIcon->setScale(0.8f);
    
    button->addChild(numGemTxt);
    button->addChild(priceTxt);
    button->addChild(_gemIcon);
    button->addChild(productNameTxt);

    m_scrollView->addChild(button);
    
    if (resourceType_ == RESOURCE_TYPE_GEM) {
        
        button->setTarget(this, menu_selector(IAPDialog::onGemButtons), CCNikScrollViewButtonStateTouchUp);
    } else if (resourceType_ == RESOURCE_TYPE_COIN) {
        
        button->setTarget(this, menu_selector(IAPDialog::onCoinButtons), CCNikScrollViewButtonStateTouchUp);
    } else if (resourceType_ == RESOURCE_TYPE_FOOD) {
        
        button->setTarget(this, menu_selector(IAPDialog::onFoodButtons), CCNikScrollViewButtonStateTouchUp);
    }
    
    button->setDelegate(m_scrollView);
    
    productNameTxt->setPosition(ccp(button->getContentSize().width*0.5,
                                    670.0f*sizeMultiplier));
    
    
    if (resourceType_ == RESOURCE_TYPE_GEM) {
        float _spacing = 4.0f * sizeMultiplier;
        float _icon_and_text_width = _gemIcon->getContentSize().width*_gemIcon->getScaleX() + numGemTxt->getContentSize().width + _spacing;
        
        _gemIcon->setPosition(ccp(button->getContentSize().width*0.5f
                              - (_icon_and_text_width*0.5f - _gemIcon->getContentSize().width*0.5f),
                              600.0f*sizeMultiplier));
        numGemTxt->setPosition(ccp(button->getContentSize().width*0.5
                                   + (_icon_and_text_width*0.5f - numGemTxt->getContentSize().width*0.5f),
                                   600.0f*sizeMultiplier));
        
        
        priceTxt->setPosition(ccp(button->getContentSize().width*0.5,
                                  150.0f*sizeMultiplier));

        if (promotionStroke)
        {
            pricePromotion->setPosition(priceTxt->getPosition() + ccp(pricePromotion->getContentSize().width*0.65f, -priceTxt->getContentSize().height*0.5f));
            promotionStroke->setPosition(pricePromotion->getPosition());

            promotionPercent->setPosition(button->getContentSize()*0.5f + ccp(button->getContentSize().width*0.33f, -button->getContentSize().height*0.10f));
            promotionTime->setPosition(button->getContentSize()*0.5f    - ccp(button->getContentSize().width*0.32f, -button->getContentSize().height*0.10f));

            promotionPercent->setZOrder(10);
            promotionTime->setZOrder(10);
        }
        
    } else {
        numGemTxt->setPosition(ccp(button->getContentSize().width*0.5,
                                   600.0f*sizeMultiplier));
        
        
        float _spacing = 8.0f * sizeMultiplier;
        float _icon_and_text_width = _gemIcon->getContentSize().width*_gemIcon->getScaleX() + priceTxt->getContentSize().width + _spacing;
        
        _gemIcon->setPosition(ccp(button->getContentSize().width*0.5f
                              - (_icon_and_text_width*0.5f - _gemIcon->getContentSize().width*0.5f),
                              150.0f*sizeMultiplier));
        
        priceTxt->setPosition(ccp(button->getContentSize().width*0.5
                                  + (_icon_and_text_width*0.5f - priceTxt->getContentSize().width*0.5f),
                                  150.0f*sizeMultiplier));
        
        
    }

    if (strcmp(ribbonText_, "") != 0) {
        CCSprite *_ribbon = UIHelper::getSprite(UIHelper::PurchaseMostPopular);
        _ribbon->setAnchorPoint(ccp(1.0f, 0.0f));
        _ribbon->setPosition(ccp(button->getContentSize().width-(-2.0f*sizeMultiplier),
                                 -5.0f*sizeMultiplier));
        button->addChild(_ribbon);
        
        CCLabelBMFont *_ribbonText = CCLabelHelper::createBMFont(ribbonText_, 40.0f, true);
        _ribbonText->setPosition(ccp(button->getContentSize().width
                                     -_ribbon->getContentSize().width*0.45f,
                                     _ribbon->getContentSize().height*0.45f));
        _ribbonText->setRotation(-18.0f);
        button->addChild(_ribbonText);
    }
    
    return button;
}

int IAPDialog::getResourceAmountFromTypeAndOrder(RESOURCE_TYPE resourceType_, int order_)
{
    int currentResources = 0;
    int fullStorage = 0;
    float percent = 0;
    
    if (order_ == 1) {
        percent = FIRST_STEP_PERCENT;
    } else if (order_ == 2) {
        percent = SECOND_STEP_PERCENT;
    }
    
    if (resourceType_ == RESOURCE_TYPE_COIN)
    {
        currentResources = g_playerState->player.balance.gold;
        fullStorage = g_playerState->getTotalCoinStorageCapacity();
    }
    else if (resourceType_ == RESOURCE_TYPE_FOOD)
    {
        currentResources = g_playerState->player.balance.food;
        fullStorage = g_playerState->getTotalFoodStorageCapacity();
    }
//    CCLog_("resourceType_:%d currentResources:%d fullStorage:%d percent:%f order:%d",resourceType_,currentResources,fullStorage,percent,order_);
    
    int resourceAmount = fullStorage * percent;
//    CCLog_("1 resourceType_:%d resourceAmount:%d",resourceType_,resourceAmount);
    
    bool isLastItem = (order_ == 3);
    
    if (isLastItem)
    {
        resourceAmount = fullStorage - currentResources;
//        CCLog_("2 resourceType_:%d resourceAmount:%d",resourceType_,resourceAmount);
    }
    
    return resourceAmount;
}

void IAPDialog::onGemButtons(CCObject *sender_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    CCNode* node = (CCNode*)sender_;
    
    if ((m_selected_product = (ProductObject *)m_products->objectAtIndex(node->getTag()-1))) {
        
        InAppPurchase::sharedInAppPurchase()->buyProductWithIdentifier(m_selected_product->productIdentifier->getCString());
        
        g_sceneManager->showLoading();
        
    }
}

void IAPDialog::onCoinButtons(CCObject *sender_)
{
    CCNode* node = (CCNode*)sender_;
    
    onResourceButtons(RESOURCE_TYPE_COIN, getResourceAmountFromTypeAndOrder(RESOURCE_TYPE_COIN, node->getTag()));
}

void IAPDialog::onFoodButtons(CCObject *sender_)
{
    CCNode* node = (CCNode*)sender_;
    
    onResourceButtons(RESOURCE_TYPE_FOOD, getResourceAmountFromTypeAndOrder(RESOURCE_TYPE_FOOD, node->getTag()));
}

void IAPDialog::onResourceButtons(RESOURCE_TYPE resourceType_, int resourceAmount_)
{
    g_gameAudio->playEffect(AUDIO_GENERAL_CLICK);
    
    switch (g_sceneManager->sceneSelected) {
        case SCENE_VILLE:
        case SCENE_VISIT:
        case SCENE_VISIT2:
        case SCENE_MATCH:
        case SCENE_MATCH2:
        case SCENE_WORLDMAP: {
            CScene* _scene = (CScene*)g_currentScene;
            if (_scene) {
                CCSize _screenSize = CCDirector::sharedDirector()->getWinSize();
                
                InstantDialog *_dialog = InstantDialog::createWithResourceType(_scene->modalController, resourceType_, resourceAmount_, kDialogSmallSize);
                _dialog->setPosition(_screenSize*0.5f);
                _scene->modalController->pushView(_dialog);
            }
        }break;
        default:
            break;
    }
}

#pragma mark - CCNikScrollViewDelegate

void IAPDialog::scrollViewWillBeginDragging(CCNikScrollView *view)
{
    //    if (m_popOver) {
    //        m_popOver->dismissPopover(true);
    //        m_popOver = NULL;
    //    }
}


#pragma mark - InAppPurchaseDelegate

void IAPDialog::getProductComplete(cocos2d::CCArray *product)
{
#ifndef DEBUG_FAKE_IAP
    m_products = product;
    if (m_products) {
        m_products->retain();
        this->updateContent();
    } else {
        InAppPurchase::sharedInAppPurchase()->getProduct();
    }
#endif
}

void IAPDialog::inAppPurchaseDidCompleteTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt) {

    BackendService::getInstance()->send(APIGemBuy::create(transactionIdentifier,
                                                          transactionReceipt,
                                                          this,
                                                          IAPDialog::onPurchaseCompleted));
    snprintf(m_transactionIdentifier, 256, "%s", transactionIdentifier);
    
}

void IAPDialog::inAppPurchaseDidFailedTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt)
{
    g_sceneManager->hideLoading();
}

bool IAPDialog::verifySuccess(int gem, string verify) {
    
    CREATE_CHAR_BUFFER(txt, 64);
    snprintf(txt, txt_size, "%d%s", gem, DEFAULT_BACKEND_PASSWD);
    string verify_key = md5(txt);

    return (verify_key.compare(verify) == 0);
    
}

void IAPDialog::onPurchaseCompleted(void *sender, std::string resp_str) {
    
    PlayerState* _playerState = PlayerState::getInstance();

    //JsonAllocator allocator;
    Json *_root = Json_create(resp_str.c_str());
    _playerState->parseUserBalance(_root);
    string verify = Json_getString(_root, "verify", "undefined");
    Json_dispose(_root);
    
    IAPDialog *_this = (IAPDialog *)sender;
    if (!_this) return;
    
    if (_this->m_selected_product && _this->verifySuccess(_playerState->player.balance.gem, verify)) {
        hasPurchasedDuringThisSession = true;
        CCDictionary *params = CCDictionary::create();
        
        CSJson::Value eventParameters;
        
        double price = AnalyticsHelper::convertFloatTo2DecimalDouble(_this->m_selected_product->price);
        const char *currency = _this->m_selected_product->currencyString->getCString();
        const char *productID = _this->m_selected_product->productIdentifier->getCString();
        
        eventParameters["price"] = price; //Price
        eventParameters["currency"] = currency; //Currency
        eventParameters["productID"] = _this->m_selected_product->productIdentifier->getCString(); //Product Identifier
        
        std::string shortProductID = productID;
        std::string stringToReplace = "com.meetnlunch.hoofit.";
        
        StringHelper::replaceString(shortProductID, stringToReplace, "");
        
        if (IS_STRING_EQUAL(productID, "com.meetnlunch.hoofit.gem01")) {
            eventParameters["gemAmount"] = 500;
        } else if (IS_STRING_EQUAL(productID, "com.meetnlunch.hoofit.gem02")) {
            eventParameters["gemAmount"] = 1200;
        } else if (IS_STRING_EQUAL(productID, "com.meetnlunch.hoofit.gem03")) {
            eventParameters["gemAmount"] = 2500;
        } else if (IS_STRING_EQUAL(productID, "com.meetnlunch.hoofit.gem04")) {
            eventParameters["gemAmount"] = 6500;
        } else if (IS_STRING_EQUAL(productID, "com.meetnlunch.hoofit.gem05")) {
            eventParameters["gemAmount"] = 14000;
        }
        
        params->setObject(ccs(AnalyticsClient::write(eventParameters)), "iapPurchaseInfo");

        FlurryCpp::logEvent(EVENT_IAP_PURCHASE, params);
        
        AnalyticsClient::getInstance()->logEvent(EVENT_IAP_PURCHASE, eventParameters);
        
        GaiCpp::reportPurchase(_this->m_selected_product->productIdentifier->getCString(),
                               _this->m_selected_product->localizedTitle->getCString(),
                               "Gems Purchase",
                               price,
                               currency);
        
        FBAppEventsWrapper::logPurchase(price,
                                        currency);
        
        std::stringstream priceString;
        priceString << price;
        
        AppsFlyerWrapper::setCurrencyCode(currency);
        AppsFlyerWrapper::trackPurchaseEvent(shortProductID.c_str(), priceString.str().c_str());

        _this->m_purchaseSuccess = true;
    }
    
    g_gameAudio->playEffect(AUDIO_FINISH_NOW);
    g_sceneManager->hideLoading();
    
    CScene *_scene = (CScene*)g_currentScene;
    if (!_scene) return;
    
    if (_scene->modalController) {
        _scene->modalController->closeAll();
    }
    if (_scene->hudPanel) {
        _scene->hudPanel->updateAllResourceBars();
    }
}
