//
//  FiveRocks.h
//  FiveRocks
//
//  Created by Sangmin Hong on 3/11/13.
//  Copyright (c) 2013 ABLAR CO., Ltd. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    kFRInactiveSessionError,
    kFRUnreachableFRHostError,
    kFRTooManyValuesError,
    kFRInvalidValuesTypeError
} FiveRocksErrorCode;


@protocol FiveRocksDelegate;

@interface FiveRocks : NSObject

/**
 * @brief Notifies the SDK that new session of your application has been started.
 * @param appId
 *        the id of your application which is present on the 5Rocks Dashboard
 * @param appKey
 *        the key of your application which is present on the 5Rocks Dashboard
 */
+ (void)startSession:(NSString *)appId withAppKey:(NSString *)appKey;
/**
 * @brief Notifies the SDK that the session of your application has been ended.
 */
+ (void)endSession;
/**
 * @brief Registers the Apple Push Service (APS) Token.
 * @param deviceToken
 *        the same paramter that passed on application:didRegisterForRemoteNotificationsWithDeviceToken:
 */
+ (void)applicationDidRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
/**
 * @brief Notifies the SDK that application did finish launching with options.
 * @param launchOptions
 *        the same parameter that passed on application:didFinishLaunchingWithOptions:
 */
+ (void)applicationDidFinishLaunchingWithOptions:(NSDictionary *)launchOptions;
/**
 * @brief Notifies the SDK that application did finish receive remote notification.
 * @param userInfo
 *        the same parameter that passed on application:didReceiveRemoteNotification:
 */
+ (void)applicationDidReceiveRemoteNotification:(NSDictionary *)userInfo;

/**
 * @brief Returns the version name of the SDK.
 * @return a string which represents the version name
 */
+ (NSString *)getFiveRocksSDKVersion;
/**
 * @brief Enables the debug mode of the SDK.
 * @param enabled
 *        true to enable
 */
+ (void)setDebugEnabled:(BOOL)enabled; // default NO
/**
 * @brief Enables secure transport for the communication with the 5Rocks Server.
 * @param enabled
 *        true to enable
 */
+ (void)setSecureTransportEnabled:(BOOL)enabled; // default NO
/**
 * @brief Sets the network timeout.
 * @param timeout
 *        timeout in NSTimeInterval
 */
+ (void)setFiveRocksNetworkTimeout:(NSTimeInterval)timeout; // default 10s

// Delegate
/**
 * @brief Sets the callback delegate to be notified.
 * @param delegate
 *        FiveRocksDelegate instance
 */
+ (void)setFiveRocksDelegate:(id <FiveRocksDelegate>)delegate;

// Data Version
/**
 * @brief Sets the data version of the App or Game.
 * @param dataVersion
 *        dataVersion String
 */
+ (void)setAppDataVersion:(NSString *)dataVersion;

// User Id
/**
 * @brief Sets the identifier of the user.
 * @param userId
 *        the identifier of the user
 */
+ (void)setUserId:(NSString *)userId;

// User Level
/**
 * @brief Sets the level of the user.
 * @param userLevel
 *        the level of the user
 */
+ (void)setUserLevel:(int)userLevel;

// User Friends Count
/**
 * @brief Sets the friends count of the user.
 * @param friendCount
 *        the number of friends
 */
+ (void)setUserFriendCount:(int)friendCount;

// User Cohort Variable
/**
 * @brief Sets a variable of the cohort.
 * @param index
 *        the index of the cohort to set (1,2,3,4,5)
 * @param value
 *        the value of the property to set
 */
+ (void)setUserCohortVariable:(int)index value:(NSString *)value;

// PlacementContent
/**
 * @brief Request a content for the given placement.
 * @param placement
 *        the desired placement
 */
+ (void)requestPlacementContent:(NSString *)placement;
/**
 * @brief Checks a content for the given placement is ready to show.
 * @param placement
 *        the desired placement
 * @return true if a content for the given placement is present and ready to
 *         show, or false
 */
+ (BOOL)hasPlacementContent:(NSString *)placement;
/**
 * @brief Shows a content for the given placement.
 * @param placement
 *        the desired placement
 */
+ (void)showPlacementContent:(NSString *)placement;

/**
 * @brief Tracks the purchase.
 * @param productIdentifier
 *        the product identifier
 * @param currencyCode
 *        the currency code of price as an alphabetic currency code specified
 *        in ISO 4217, i.e. "USD", "KRW"
 * @param price
 *        the price of product
 * @param campaignId
 *        the campaign id of the purchase request which initiated this
 *        purchase, can be nil
 */
+ (void)trackPurchase:(NSString *)productIdentifier
         currencyCode:(NSString *)currencyCode
                price:(double)price
           campaignId:(NSString *)nilOrCampaignId
            withError:(NSError **)error;

/**
 * @brief Tracks the given NSException.
 * @param exception
 *        the exception to track
 */
+ (void)trackException:(NSException *)exception withError:(NSError **)error;
/**
 * @brief Tracks the given exception.
 * @param description
 *        the exception to track
 */
+ (void)trackExceptionWithDescription:(NSString *)description withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
         withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2 and value.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 * @param value
 *        the value of event
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
             value:(int64_t)value
         withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2 and value.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 * @param value1name
 *        the name of value1 of event
 * @param value1
 *        the value of value1name
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
        value1name:(NSString *)value1name
            value1:(int64_t)value1
         withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2 and value.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 * @param value1name
 *        the name of value1 of event
 * @param value1
 *        the value of value1name
 * @param value2name
 *        the name of value2 of event
 * @param value2
 *        the value of value2name
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
        value1name:(NSString *)value1name
            value1:(int64_t)value1
        value2name:(NSString *)value2name
            value2:(int64_t)value2
         withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2 and value.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 * @param value1name
 *        the name of value1 of event
 * @param value1
 *        the value of value1name
 * @param value2name
 *        the name of value2 of event
 * @param value2
 *        the value of value2name
 * @param value3name
 *        the name of value3 of event
 * @param value3
 *        the value of value3name
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
        value1name:(NSString *)value1name
            value1:(int64_t)value1
        value2name:(NSString *)value2name
            value2:(int64_t)value2
        value3name:(NSString *)value3name
            value3:(int64_t)value3
         withError:(NSError **)error;

/**
 * @brief Tracks an event of the given name with category, parameter1, parameter2 and values.
 * @param name
 *        the name of event
 * @param category
 *        the category of event, can be nil
 * @param parameter1
 *        the parameter of event, string type, can be nil
 * @param parameter2
 *        the parameter of event, string type, can be nil
 * @param values
 *        NSDictionary that contains values of event
 */
+ (void)trackEvent:(NSString *)name
          category:(NSString *)category
        parameter1:(NSString *)parameter1
        parameter2:(NSString *)parameter2
            values:(NSDictionary *)values
         withError:(NSError **)error;

@end

@protocol FiveRocksDelegate <NSObject>

@optional

/**
 * @brief Called when no content for the given placement exists.
 * @param placement
 *        the placement that was requested
 */
- (void)didFailToReadyPlacementContent:(NSString *)placement;
/**
 * @brief Called when a content for the given placement is ready to show.
 * @param placement
 *        the placement that was requested
 */
- (void)didReadyPlacementContent:(NSString *)placement;

/**
 * @brief Called when a content for the given placement is showing.
 * @param placement
 *        the placement that was requested
 */
- (void)didShowPlacementContent:(NSString *)placement;
/**
 * @brief Called when a content for the given placement is going to close by user.
 * @param placement
 *        the placement that was requested
 */
- (void)didClosePlacementContent:(NSString *)placement;

/**
 * @brief Called when a content for the given placement has been clicked by user.
 * @param placement
 *        the placement that was requested
 */
- (void)didClickPlacementContent:(NSString *)placement;
/**
 * @brief Called when a content for the given placement has been clicked by user, with a purchase request.
 * @param placement
 *        the placement that was requested
 * @param productId
 *        the product identifier (SKU) of the in-app item to purchase
 * @param campaignId
 *        the identifier of the campaign which make this request
 */
- (void)didClickPlacementContent:(NSString *)placement purchaseRequestForProductId:(NSString *)productId withCampaignId:(NSString *)campaignId;
/**
 * @brief Called when a content for the given placement has been clicked by user, with a reward request.
 * @param placement
 *        the placement that was requested
 * @param rewardId
 *        the unique identifier of this reward to prevent the reuse attack
 * @param name
 *        the name of the rewarded item
 * @param quantity
 *        the quantity of the rewarded item
 * @param token
 *        the token to verify this reward request
 */
- (void)didClickPlacementContent:(NSString *)placement rewardRequestForRewardId:(NSString *)rewardId name:(NSString *)name quantity:(NSUInteger)quantity token:(NSString *)token;

/**
 * @brief Called when a content for the given placement is dismissed.
 * @param placement
 *        the placement that was requested
 */
- (void)didDismissPlacementContent:(NSString *)placement;
/**
 * @brief Called when a content for the given placement is dismissed, with a purchase request.
 * @param placement
 *        the placement that was requested
 * @param productId
 *        the product identifier (SKU) of the in-app item to purchase
 * @param campaignId
 *        the identifier of the campaign which make this request
 */
- (void)didDismissPlacementContent:(NSString *)placement purchaseRequestForProductId:(NSString *)productId withCampaignId:(NSString *)campaignId;
/**
 * @brief Called when a content for the given placement is dismissed, with a reward request.
 * @param placement
 *        the placement that was requested
 * @param rewardId
 *        the unique identifier of this reward to prevent the reuse attack
 * @param name
 *        the name of the rewarded item
 * @param quantity
 *        the quantity of the rewarded item
 * @param token
 *        the token to verify this reward request
 */
- (void)didDismissPlacementContent:(NSString *)placement rewardRequestForRewardId:(NSString *)rewardId name:(NSString *)name quantity:(NSUInteger)quantity token:(NSString *)token;

@end