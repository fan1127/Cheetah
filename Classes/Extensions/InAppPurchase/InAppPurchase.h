//
//  InAppPurchase.h
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/4/2557 BE.
//
//

#ifndef __Cheetah__InAppPurchase__
#define __Cheetah__InAppPurchase__

#include <iostream>
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

USING_NS_CC;

struct ProductObject : CCObject {
  
public:
    CCString *localizedDescription;
    CCString *localizedTitle;
    CCString *priceString;
    CCString *currencyString;
    float price;
    int gemAmount;
    CCString *productIdentifier;

    ~ProductObject();
};

class InAppPurchase;

class InAppPurchaseDelegate
{
public:
    virtual void getProductComplete(CCArray *product) {};
    
    virtual void inAppPurchaseDidFailedTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt) {};
    virtual void inAppPurchaseDidCompleteTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt) {};
    virtual void inAppPurchaseDidRestoreTransaction(InAppPurchase *inAppPurchase, const char *transactionIdentifier, const char *transactionReceipt) {};
};


class InAppPurchase {
    
    InAppPurchaseDelegate *m_delegate;
    CCArray *m_product;
    
public:
    
    static InAppPurchase* sharedInAppPurchase();
    bool init();

    void getProduct();
    void buyProductWithIdentifier(const char *identifier);
    
    void setDelegate(InAppPurchaseDelegate *delegate) { m_delegate = delegate; };



#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

    static std::vector<std::string> customSplit(const std::string &s, char delim);

    void onAndroidQuerySKUDetailsCallback(std::string resultString);
    void onAndroidPurchaseDidCompleteTransaction(std::string transactionIdentifier, std::string transactionReceipt);
    void onAndroidPurchaseDidFailedTransaction(std::string transactionIdentifier, std::string transactionReceipt);
    void onAndroidPurchaseDidRestoreTransaction(std::string transactionIdentifier, std::string transactionReceipt);

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
};


#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" {

JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onQuerySKUDetailsCallback(JNIEnv * env, jobject  obj, jstring resultString);

JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseCompleteCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt);

JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseFailedCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt);

JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_iab_InAppBillingHelper_onPurchaseRestoredCallback(JNIEnv * env, jobject  obj, jstring transactionID, jstring transactionReceipt);


}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)


#endif /* defined(__Cheetah__InAppPurchase__) */
