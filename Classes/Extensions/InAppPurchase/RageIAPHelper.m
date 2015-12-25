//
//  RageIAPHElper.m
//  Avalable
//
//  Created by Pipat Shuleepongchad on 9/26/56 BE.
//  Copyright (c) 2556 Pipat Shuleepongchad. All rights reserved.
//

#import "RageIAPHelper.h"

@implementation RageIAPHelper

+ (RageIAPHelper *)sharedInstance {
    static dispatch_once_t once;
    static RageIAPHelper * sharedInstance;
    dispatch_once(&once, ^{
        NSSet * productIdentifiers = [NSSet setWithObjects:
                                      @"com.meetnlunch.hoofit.gem01",
                                      @"com.meetnlunch.hoofit.gem02",
                                      @"com.meetnlunch.hoofit.gem03",
                                      @"com.meetnlunch.hoofit.gem04",
                                      @"com.meetnlunch.hoofit.gem05",
                                      nil];
        sharedInstance = [[self alloc] initWithProductIdentifiers:productIdentifiers];
    });
    return sharedInstance;
}

+ (RageIAPHelper *)sharedInstanceWithProductIdentifiers:(NSSet *)productIdentifiers {
    static dispatch_once_t once;
    static RageIAPHelper * sharedInstance;
    dispatch_once(&once, ^{
        sharedInstance = [[self alloc] initWithProductIdentifiers:productIdentifiers];
    });
    return sharedInstance;
}

- (void)buyProductWithIdentifier:(NSString *)identifier withCompletionHandler:(IAPHelperCompletionHandler)completionHandler
{
    self.completionHandler = completionHandler;
    
    for (SKProduct *product in self.product) {
        if ([product.productIdentifier isEqualToString:identifier]) {

//            if (self.alertView && ![self.alertView isHidden]) {
//                [self.alertView dismissWithClickedButtonIndex:0 animated:YES];
//            }
//            self.alertView = [[UIAlertView alloc] initWithTitle:@"Loading..." message:Nil delegate:Nil cancelButtonTitle:Nil otherButtonTitles:nil];
//            [self.alertView show];

            [self buyProduct:product];
            return;
        }
    }

    self.completionHandler(self, Nil, IAPHelperStateNotfound);
}

- (id)initWithProductIdentifiers:(NSSet *)productIdentifiers
{
    self = [super initWithProductIdentifiers:productIdentifiers];
    if (self) {
        self.delegate = self;
    }
    return self;
}


- (void)IAPHelper:(IAPHelper *)helper didCompleteTransaction:(SKPaymentTransaction *)transaction
{
//    [self.alertView dismissWithClickedButtonIndex:0 animated:NO];
    if (self.completionHandler) self.completionHandler (helper, transaction, IAPHelperStateCompleteTransaction);
}

- (void)IAPHelper:(IAPHelper *)helper didFailedTransaction:(SKPaymentTransaction *)transaction
{
//    [self.alertView dismissWithClickedButtonIndex:0 animated:NO];
    if (self.completionHandler) self.completionHandler (helper, transaction, IAPHelperStateFailedTransaction);
}

- (void)IAPHelper:(IAPHelper *)helper didRestoreTransaction:(SKPaymentTransaction *)transaction
{
//    [self.alertView dismissWithClickedButtonIndex:0 animated:NO];
    if (self.completionHandler) self.completionHandler (helper, transaction, IAPHelperStateRestoreTransaction);
}
@end
