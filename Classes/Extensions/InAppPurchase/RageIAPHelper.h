//
//  RageIAPHElper.h
//  Avalable
//
//  Created by Pipat Shuleepongchad on 9/26/56 BE.
//  Copyright (c) 2556 Pipat Shuleepongchad. All rights reserved.
//

#import "IAPHelper.h"

#import <StoreKit/StoreKit.h>

typedef enum
{
    IAPHelperStateCompleteTransaction,
    IAPHelperStateFailedTransaction,
    IAPHelperStateRestoreTransaction,
    IAPHelperStateNotfound,
}IAPHelperState;

typedef void (^IAPHelperCompletionHandler) (IAPHelper *helper, SKPaymentTransaction *transaction, IAPHelperState state);

@interface RageIAPHelper : IAPHelper<IAPHelperDelegate>

@property (strong, nonatomic) UIAlertView *alertView;
@property (strong, nonatomic) IAPHelperCompletionHandler completionHandler;

+ (RageIAPHelper *)sharedInstance;
+ (RageIAPHelper *)sharedInstanceWithProductIdentifiers:(NSSet *)productIdentifiers;

- (void)buyProductWithIdentifier:(NSString *)identifier withCompletionHandler:(IAPHelperCompletionHandler)completionHandler;
@end
