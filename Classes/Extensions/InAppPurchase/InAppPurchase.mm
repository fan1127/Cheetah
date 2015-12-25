//
//  InAppPurchase.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 4/4/2557 BE.
//
//

#include "InAppPurchase.h"

#import "RageIAPHelper.h"

using namespace std;

ProductObject::~ProductObject()
{
    if (localizedDescription) {
        localizedDescription->release();
        localizedDescription = NULL;
    }
    
    if (localizedTitle) {
        localizedTitle->release();
        localizedTitle = NULL;
    }
    
    if (productIdentifier) {
        productIdentifier->release();
        productIdentifier = NULL;
    }
    
    if (priceString) {
        priceString->release();
        priceString = NULL;
    }
    
    if (currencyString) {
        currencyString->release();
        currencyString = NULL;
    }
}


static InAppPurchase* s_sharedInAppPurchase;

InAppPurchase* InAppPurchase::sharedInAppPurchase()
{
    if (s_sharedInAppPurchase == NULL)
    {
        s_sharedInAppPurchase = new InAppPurchase();
        s_sharedInAppPurchase->init();
    }
    return s_sharedInAppPurchase;
}

bool InAppPurchase::init()
{
    
    m_delegate = NULL;
    m_product = NULL;
    
    return true;
}



void InAppPurchase::getProduct()
{
    if (m_product) {
        if (m_delegate) {
            m_delegate->getProductComplete(m_product);
        }
        return;
    }
    
    [[RageIAPHelper sharedInstance] requestProductsWithCompletionHandler:^(BOOL success, NSArray *products) {
//        NSLog(@"%@",products);
        
        if (products.count) {
            CCArray *array = CCArray::create();
            for (SKProduct *product in products) {
                ProductObject *object = new ProductObject;
                
                if(product.localizedDescription) {
                    object->localizedDescription = CCString::create([product.localizedDescription UTF8String]);
                }
                else object->localizedDescription = CCString::create("");
                object->localizedDescription->retain();
                
                if(product.localizedTitle) {
                    object->localizedTitle = CCString::create([product.localizedTitle UTF8String]);
                }
                else object->localizedTitle = CCString::create("");
                object->localizedTitle->retain();
                
                object->productIdentifier = CCString::create([product.productIdentifier UTF8String]);
                object->productIdentifier->retain();
                
                object->priceString = CCString::create([product.priceAsString UTF8String]);
                object->priceString->retain();
                
                NSNumberFormatter *currencyFormatter = [[NSNumberFormatter alloc] init];
                [currencyFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                [currencyFormatter setLocale:product.priceLocale];
                NSString *currencyString = [currencyFormatter internationalCurrencySymbol]; // EUR, GBP, USD...
                [currencyFormatter release];
                
                object->currencyString = CCString::create([currencyString UTF8String]);
                object->currencyString->retain();
                
                object->price = product.price.floatValue;
                
                NSScanner *scanner = [NSScanner scannerWithString:product.localizedDescription];
                
                // set it to skip non-numeric characters
                [scanner setCharactersToBeSkipped:[[NSCharacterSet decimalDigitCharacterSet] invertedSet]];
                
                int i;
                while ([scanner scanInt:&i])
                {
                    NSLog(@"Found int: %d",i);
                    object->gemAmount = i;
                }
                
                array->addObject(object);
                object->autorelease();
            }
            
            m_product = array;
            m_product->retain();
            
            if (m_delegate) {
                m_delegate->getProductComplete(m_product);
            }
        } else {
            if (m_delegate) {
                m_delegate->getProductComplete(NULL);
            }
        }
    }];
}

void InAppPurchase::buyProductWithIdentifier(const char *identifier)
{
    [[RageIAPHelper sharedInstance] buyProductWithIdentifier:[[NSString alloc] initWithCString:identifier encoding:NSUTF8StringEncoding] withCompletionHandler:^(IAPHelper *helper, SKPaymentTransaction *transaction, IAPHelperState state) {
        if (m_delegate) {
            if (state == IAPHelperStateCompleteTransaction) {
                m_delegate->inAppPurchaseDidCompleteTransaction(this, transaction.transactionIdentifier.UTF8String,  transaction.transactionReceipt.base64Encoding.UTF8String);
            } else if (state == IAPHelperStateFailedTransaction) {
                m_delegate->inAppPurchaseDidFailedTransaction(this, transaction.transactionIdentifier.UTF8String,  transaction.transactionReceipt.base64Encoding.UTF8String);
            } else if (state == IAPHelperStateRestoreTransaction) {
                m_delegate->inAppPurchaseDidRestoreTransaction(this, transaction.transactionIdentifier.UTF8String,  transaction.transactionReceipt.base64Encoding.UTF8String);
            }
        }
    }];
}