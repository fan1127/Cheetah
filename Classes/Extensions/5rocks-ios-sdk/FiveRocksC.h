//
//  Copyright (c) 2013 5Rocks.io
//  All rights reserved.
//

/**
 * @file FiveRocksC.h
 * @brief C API header file of the 5Rocks SDK
 */

#ifndef FiveRocksC_h
#define FiveRocksC_h

#include <stdbool.h>
#include <stdint.h>

#if defined(ANDROID)
#include <jni.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(ANDROID) && defined(FIVEROCKS_STATIC)
    /**
     * @brief Sets the JavaVM instance to interoperate with Java for Android only.
     * This method is equivalent to FiveRocks.initStaticLibrary() in Java.
     * @param vm
     *        a pointer to JavaVM instance
     */
    jint FiveRocks_setJavaVM(JavaVM* vm);
#endif

    /**
     * @brief Returns the version name of the SDK.
     * @return a string which represents the version name
     */
    const char* FiveRocks_getVersion();

    /**
     * @brief Enables the debug mode of the SDK.
     * @param enabled
     *        true to enable
     */
    void FiveRocks_setDebugEnabled(bool enabled);

    /**
     * @brief Enables secure transport for the communication with the 5Rocks Server.
     * @param enabled
     *        true to enable
     */
    void FiveRocks_setSecureTransportEnabled(bool enabled);

    /**
     * @brief Sets the network timeout.
     * @param timeout
     *        timeout in double
     */
    void FiveRocks_setNetworkTimeout(double timeout);


    //////////////////////////////////////////////////
    // App and User Properties
    //////////////////////////////////////////////////

    /**
     * @brief Sets the data version of the App or Game.
     * @param version
     *        data version
     */
    void FiveRocks_setAppDataVersion(const char* version);

    /**
     * @brief Sets the identifier of the user.
     * @param userId
     *        the identifier of the user
     */
    void FiveRocks_setUserId(const char* userId);

    /**
     * @brief Sets the level of the user.
     * @param userLevel
     *        the level of the user
     */
    void FiveRocks_setUserLevel(int userLevel);

    /**
     * @brief Sets the friends count of the user.
     * @param friendCount
     *        the number of friends
     */
    void FiveRocks_setUserFriendCount(int friendCount);

    /**
     * @brief Sets a variable of user for the cohort analysis.
     * @param index
     *        the index of the variable to set, must be in the range 1 to 5
     * @param value
     *        the value of the variable to set, or null to unset
     */
    void FiveRocks_setUserCohortVariable(int index, const char* value);


    //////////////////////////////////////////////////
    // Session
    //////////////////////////////////////////////////

    /**
     * @brief Notifies the SDK that new session of your application has been started.
     * @param appId
     *        the id of your application which the 5Rocks Dashboard
     * @param appKey
     *        the key of your application which the 5Rocks Dashboard
     */
    void FiveRocks_startSession(const char* appId, const char* appKey);

    /**
     * @brief Notifies the SDK that the session of your application has been ended.
     */
    void FiveRocks_endSession();


    //////////////////////////////////////////////////
    // Event Tracking
    //////////////////////////////////////////////////

    /**
     * @brief Tracks an event of the given category and name, with two parameters
     *        and three named values at most.
     * @param category
     *        the category of event, can be null
     * @param name
     *        the name of event
     * @param parameter1
     *        the value of the 1st parameter of event, can be null
     * @param parameter2
     *        the value of the 2nd parameter of event, can be null
     * @param value1Name
     *        the name of the 1st value of event, or null to be ignored
     * @param value1
     *        the 1st value of event, or 0 to be ignore
     * @param value2Name
     *        the name of the 2nd value of event
     * @param value2
     *        the 2nd value of event
     * @param value3Name
     *        the name of the 3rd value of event
     * @param value3
     *        the 3rd value of event
     */
    void FiveRocks_trackEvent(const char* category, const char* name,
                              const char* parameter1, const char* parameter2,
                              const char* value1Name, int64_t value1,
                              const char* value2Name, int64_t value2,
                              const char* value3Name, int64_t value3);


    //////////////////////////////////////////////////
    // Purchase Tracking
    //////////////////////////////////////////////////

    /**
     * @brief Tracks the purchase.
     * @param productId
     *        the product identifier
     * @param currencyCode
     *        the currency code of price as an alphabetic currency code specified
     *        in ISO 4217, i.e. "USD", "KRW"
     * @param price
     *        the price of product
     * @param campaignId
     *        the campaign id of the Purchase Action Request if it
     *        initiated this purchase, can be null
     */
    void FiveRocks_trackPurchase(const char* productId, const char* currencyCode, double price, const char* campaignId);


    //////////////////////////////////////////////////
    // Exception Tracking
    //////////////////////////////////////////////////

    /**
     * @brief Tracks the given exception.
     * @param exception
     *        the exception to track
     */
    void FiveRocks_trackException(const char* exception);


    //////////////////////////////////////////////////
    // Placement Content
    //////////////////////////////////////////////////

    /**
     * @brief Request a content for the given placement.
     * @param placement
     *        the desired placement
     */
    void FiveRocks_requestPlacementContent(const char* placement);

    /**
     * @brief Checks a content for the given placement is ready to show.
     * @param placement
     *        the desired placement
     * @return true if a content for the given placement is present and ready to
     *         show, or false
     */
    bool FiveRocks_hasPlacementContent(const char* placement);

    /**
     * @brief Shows a content for the given placement.
     * @param placement
     *        the desired placement
     */
    void FiveRocks_showPlacementContent(const char* placement);


    //////////////////////////////////////////////////
    // Callbacks
    //////////////////////////////////////////////////

    typedef struct FiveRocks_ActionRequest FiveRocks_ActionRequest;

    /**
     * @brief Structure definition for callback functions to be notified.
     */
    typedef struct {
        /**
         * @brief Called when no content for the given placement exists.
         * @param placement
         *        the placement that was requested
         */
        void (*onPlacementContentNone)(const char* placement);

        /**
         * @brief Called when a content for the given placement is ready to show.
         * @param placement
         *        the placement that was requested
         */
        void (*onPlacementContentReady)(const char* placement);

        /**
         * @brief Called when a content for the given placement is showing.
         * @param placement
         *        the placement that was requested
         */
        void (*onPlacementContentShow)(const char* placement);

        /**
         * @brief Called when a content for the given placement is going to close by user.
         * @param placement
         *        the placement that was requested
         */
        void (*onPlacementContentClose)(const char* placement);

        /**
         * @brief Called when a content for the given placement has been clicked by user.
         * @param placement
         *        the placement that was requested
         * @param actionRequest
         *        ActionRequest instance if present, or NULL
         */
        void (*onPlacementContentClick)(const char* placement, FiveRocks_ActionRequest* actionRequest);

        /**
         * @brief Called when a content for the given placement is dismissed.
         * @param placement
         *        the placement that was requested
         * @param actionRequest
         *        ActionRequest instance if present, or NULL
         */
        void (*onPlacementContentDismiss)(const char* placement, FiveRocks_ActionRequest* actionRequest);
    } FiveRocks_CallbackHandler;

    /**
     * @brief Sets the callback handler to be notified.
     * @param handler
     *        a pointer to FiveRocks_CallbackHandler instance
     */
    void FiveRocks_setCallbackHandler(FiveRocks_CallbackHandler* handler);


    //////////////////////////////////////////////////
    // Action Request
    //////////////////////////////////////////////////

    /**
     * @brief Structure definition for callback functions to process action requests.
     */
    typedef struct {
        /**
         * @brief Called when a purchase has been requested.
         * @param campaignId
         *        the identifier of the campaign which make this request
         * @param productId
         *        the product identifier (SKU) of the in-app item to purchase
         */
        void (*onPurchaseRequest)(const char* campaignId, const char* productId);

        /**
         * @brief Called when a reward unlock has been requested.
         * @param id
         *        the unique identifier of this reward to prevent the reuse attack
         * @param name
         *        the name of the rewarded item
         * @param quantity
         *        the quantity of the rewarded item
         * @param token
         *        the token to verify this reward request
         */
        void (*onRewardRequest)(const char* id, const char* name, int quantity, const char* token);
    } FiveRocks_ActionRequestHandler;

    /**
     * @brief Dispatch an action request to the given handler.
     * @param actionRequest
     *        an action request to process
     * @param handler
     *        a handler to process the request
     */
    void FiveRocks_dispatchActionRequestTo(FiveRocks_ActionRequest* actionRequest, FiveRocks_ActionRequestHandler* handler);

#if defined(__cplusplus)
}
#endif

#endif // FiveRocksC_h
