//
//  GaiCpp.cpp
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 8/1/2557 BE.
//
//

#import "GaiCpp.h"
#import "GAI.h"
#import "GAIFields.h"
#import "GAIDictionaryBuilder.h"
#import "ACTReporter.h"

#define MY_CONVERSION_ID @"963562779"

//GAI* GaiCpp::sharedInstance(void)
//{
//    return [GAI sharedInstance];
//}


void GaiCpp::trackerWithTrackingId(const char* trackingId)
{
    [[GAI sharedInstance] trackerWithTrackingId:[NSString stringWithUTF8String:trackingId]];
}

void GaiCpp::reportToACTConversionReporter()
{
    [ACTAutomatedUsageTracker enableAutomatedUsageReportingWithConversionID:MY_CONVERSION_ID];
    
    [ACTConversionReporter reportWithConversionID:MY_CONVERSION_ID label:@"SUfuCPeNtFcQm5q7ywM" value:@"1.00" isRepeatable:NO];
}

//void GaiCpp::setUserID(const char* userID)
//{
//    id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];
//    
//    // You only need to set User ID on a tracker once. By setting it on the tracker, the ID will be
//    // sent with all subsequent hits.
//    [tracker set:@"&uid"
//           value:[NSString stringWithUTF8String:userID]];
//}

void GaiCpp::reportScreenView(const char* screenName)
{
    id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];
    
    if (tracker) {
        [tracker set:kGAIScreenName
               value:[NSString stringWithUTF8String:screenName]];
        
        [tracker send:[[GAIDictionaryBuilder createScreenView] build]];
    }
}

void GaiCpp::setDryRun(bool dryRun)
{
    [[GAI sharedInstance] setDryRun:dryRun];
}

void GaiCpp::setLogLevelVerbose()
{
    [[GAI sharedInstance].logger setLogLevel:kGAILogLevelVerbose];
}

void GaiCpp::setDispatchInterval(double timeInterval)
{
    [[GAI sharedInstance] setDispatchInterval:(NSTimeInterval)timeInterval];
}

void GaiCpp::reportPurchase(const char* productIdentifier,
                            const char* localizedTitle,
                            const char* productCategory,
                            float price,
                            const char* currencyCode)
{
    // Assumes a tracker has already been initialized with a property ID, otherwise
    // this call returns null.
    
    id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];
    
    if (tracker) {
        
        NSString *transactionID = [[NSUUID UUID] UUIDString];
        
        [tracker send:[[GAIDictionaryBuilder createTransactionWithId:transactionID             // (NSString) Transaction ID
                                                         affiliation:@"In-App Store"         // (NSString) Affiliation
                                                                    // (NSNumber) Order revenue (including tax and shipping)
                                                             revenue:[NSNumber numberWithFloat:price]
                                                                 tax:@0                  // (NSNumber) Tax
                                                            shipping:@0                      // (NSNumber) Shipping
                                                                    // (NSString) Currency code
                                                        currencyCode:[NSString stringWithUTF8String:currencyCode]]
                       build]];
        
        
        [tracker send:[[GAIDictionaryBuilder createItemWithTransactionId:transactionID         // (NSString) Transaction ID
                                                                        // (NSString) Product Name
                                                                    name:[NSString stringWithUTF8String:localizedTitle]
                                                                        // (NSNumber)  Product price
                                                                     sku:[NSString stringWithUTF8String:productIdentifier]
                                                                        // (NSString) Product category
                                                                category:[NSString stringWithUTF8String:productCategory]
                                                                        // (NSNumber)  Product price
                                                                   price:[NSNumber numberWithFloat:price]
                                                                quantity:@1                  // (NSInteger)  Product quantity
                                                                        // (NSString) Currency code
                                                            currencyCode:[NSString stringWithUTF8String:currencyCode]]
                        build]];
    }
    
}

