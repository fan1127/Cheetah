//
//  IAPDialog.h
//  Cheetah
//
//  Created by Teerapat on 4/3/57 BE.
//
//

#ifndef __Cheetah__IAPDialog__
#define __Cheetah__IAPDialog__


#include "../../Extensions/Modal/CCModalView.h"
#include "../../Extensions/CCPhatTouchableSprite.h"
#include "../../Extensions/CCNikScrollView.h"
#include "../../Extensions/CCLabelHelper.h"
#include "../../Extensions/CCClippingLayer.h"
#include "../../Extensions/InAppPurchase/InAppPurchase.h"
#include "../../Settings.h"
#include "ScrollDialog.h"

#define CURRENCY "USD"

//#define DEBUG_FAKE_IAP //for testing the layout when real IAP information isn't available

#define RIBBON_NONE ""
#define RIBBON_MOSTPOPULAR "Most Popular"
#define RIBBON_BESTOFFER "Best Offer!"

#define FIRST_STEP_PERCENT 0.1f
#define SECOND_STEP_PERCENT 0.5f

class IAPDialog : public ScrollDialog, public InAppPurchaseDelegate
{
    CCModalController           *_modalController;

    ProductObject*              m_selected_product;
    CCArray*                    m_products;
    char                        m_transactionIdentifier[256];
    void                        *m_sender;
    void                        (*m_callback)(void*);
    bool                        m_purchaseSuccess;

    RESOURCE_TYPE               m_scrollToResource;

    struct Promotion
    {
        Promotion() : original(NULL), promotion(NULL) {}
        ProductObject* original;
        ProductObject* promotion;
        int percent;
        int timeLeft;
        float orignalPrice;
    };

    bool isPromotionProduct(ProductObject* product) const;
    bool hasPromotion(ProductObject* product, std::vector<ProductObject*> promotions, IAPDialog::Promotion& promotion);
    std::vector<ProductObject*> getPromotions() const;

    bool verifySuccess(int gem, std::string verify);
    
public:

    static IAPDialog *create(CCModalController *parentModalController_, RESOURCE_TYPE scrollToResource_);
    bool init(CCModalController* parentModalController_, RESOURCE_TYPE scrollToResource_, CCSize size);
    void setCallback(void* sender, void (*callback_)(void*)) { m_sender = sender; m_callback = callback_; }

    ~IAPDialog();
    virtual void onExit();
    
    virtual void updateContent();
    
    CCNikScrollViewButton* addResourceButtons(RESOURCE_TYPE resourceType_,
                                              int *buttonPosition);
    CCNikScrollViewButton* addButton(const char* buttonTitle_,
                                     RESOURCE_TYPE resourceType_,
                                     int buttonTypeOrder_,
                                     int buttonMax,
                                     int buttonPosition_,
                                     int resourceAmount_,
                                     const char* priceText_,
                                     const char* ribbonText_,
                                     Promotion* promotionInfo);
    
    void onGemButtons(cocos2d::CCObject *sender_);
    void onCoinButtons(cocos2d::CCObject *sender_);
    void onFoodButtons(cocos2d::CCObject *sender_);
    void onResourceButtons(RESOURCE_TYPE resourceType_, int resourceAmount_);
    int getResourceAmountFromTypeAndOrder(RESOURCE_TYPE resourceType_, int order_);

//    static void onHitShadow(void *sender_);
    virtual void scrollViewWillBeginDragging(CCNikScrollView *view);
    
    virtual void getProductComplete(CCArray *product);

    virtual void inAppPurchaseDidFailedTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt);
    virtual void inAppPurchaseDidCompleteTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt);
    
    static void onPurchaseCompleted(void *sender, std::string resp_json_str);
};


#endif /* defined(__Cheetah__IAPDialog__) */
