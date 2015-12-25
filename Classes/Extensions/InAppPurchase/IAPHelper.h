//
//  IAPHelper.h
//  Avalable
//
//  Created by Pipat Shuleepongchad on 9/26/56 BE.
//  Copyright (c) 2556 Pipat Shuleepongchad. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

@class IAPHelper;

@interface SKProduct (priceAsString)
@property (nonatomic, readonly) NSString *priceAsString;
@end

UIKIT_EXTERN NSString *const IAPHelperProductPurchasedNotification;

@protocol IAPHelperDelegate <NSObject>

- (void)IAPHelper:(IAPHelper *)helper didFailedTransaction:(SKPaymentTransaction *)transaction;
- (void)IAPHelper:(IAPHelper *)helper didCompleteTransaction:(SKPaymentTransaction *)transaction;
- (void)IAPHelper:(IAPHelper *)helper didRestoreTransaction:(SKPaymentTransaction *)transaction;

@end


typedef void (^RequestProductsCompletionHandler)(BOOL success, NSArray * products);

@interface IAPHelper : NSObject

@property (strong, nonatomic) NSArray *product;
@property (weak , nonatomic) id<IAPHelperDelegate> delegate;

- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers;
- (void)requestProductsWithCompletionHandler:(RequestProductsCompletionHandler)completionHandler;

- (void)buyProduct:(SKProduct *)product;
- (BOOL)productPurchased:(NSString *)productIdentifier;
- (void)restoreTransactions;


@end
