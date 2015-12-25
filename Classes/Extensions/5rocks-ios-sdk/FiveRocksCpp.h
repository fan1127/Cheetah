//
//  Copyright (c) 2013 5Rocks.io
//  All rights reserved.
//

#ifndef fiverocks_FiveRocksCpp_h
#define fiverocks_FiveRocksCpp_h

#include <stdint.h>

#if defined(ANDROID)
#include <jni.h>
#endif

namespace fiverocks {

    class FiveRocksCallbackHandler;
    class FiveRocksActionRequest;
    class FiveRocksActionRequestHandler;

    /**
     * @brief C++ Bridge class for the FiveRocks API class.
     */
    class FiveRocksBridge {
    public:

#if defined(ANDROID) && defined(FIVEROCKS_STATIC)
        /**
         * @brief Sets the JavaVM instance to interoperate with Java for Android only.
         * This method is equivalent to FiveRocks.initStaticLibrary() in Java.
         * @param vm
         *        a pointer to JavaVM instance
         */
        static jint setJavaVM(JavaVM* vm);
#endif

        /**
         * @brief Returns the version name of the SDK.
         * @return a string which represents the version name
         */
        static const char* getVersion();

        /**
         * @brief Enables the debug mode of the SDK.
         * @param enabled
         *        true to enable
         */
        static void setDebugEnabled(bool enabled);

        /**
         * @brief Enables secure transport for the communication with the 5Rocks Server.
         * @param enabled
         *        true to enable
         */
        static void setSecureTransportEnabled(bool enabled);

        /**
         * @brief Sets the network timeout.
         * @param timeout
         *        timeout in double
         */
        static void setNetworkTimeout(double timeout);


        //////////////////////////////////////////////////
        // App and User Properties
        //////////////////////////////////////////////////

        /**
         * @brief Sets the data version of the App or Game.
         * @param version
         *        the data version
         */
        static void setAppDataVersion(const char *version);

        /**
         * @brief Sets the identifier of the user.
         * @param userId
         *        the identifier of the user
         */
        static void setUserId(const char* userId);

        /**
         * @brief Sets the level of the user.
         * @param userLevel
         *        the level of the user
         */
        static void setUserLevel(int userLevel);

        /**
         * @brief Sets the friends count of the user.
         * @param friendCount
         *        the number of friends
         */
        static void setUserFriendCount(int friendCount);

        /**
         * @brief Sets a variable of user for the cohort analysis.
         * @param index
         *        the index of the variable to set, must be in the range 1 to 5
         * @param value
         *        the value of the variable to set, or null to unset
         */
        static void setUserCohortVariable(int index, const char* value);


        //////////////////////////////////////////////////
        // Session
        //////////////////////////////////////////////////

        /**
         * @brief Notifies the SDK that new session of your application has been started.
         * @param appId
         *        the 5Rocks id of your app, found on the 5Rocks Dashboard
         * @param appKey
         *        the 5Rock key of your app, found on the 5Rocks Dashboard
         */
        static void startSession(const char* appId, const char* appKey);

        /**
         * @brief Notifies the SDK that the session of your application has been ended.
         */
        static void endSession();


        //////////////////////////////////////////////////
        // Event Tracking
        //////////////////////////////////////////////////

        /**
         * @brief Tracks an event of the given category and name, with two parameters.
         * @param category
         *        the category of event, can be nil
         * @param name
         *        the name of event
         * @param parameter1
         *        the parameter of event, string type, can be nil
         * @param parameter2
         *        the parameter of event, string type, can be nil
         */
        static void trackEvent(const char* category, const char* name,
                               const char* parameter1, const char* parameter2);

        /**
         * @brief Tracks an event of the given category and name, with two parameters
         *        and a value.
         * @param category
         *        the category of event, can be null
         * @param name
         *        the name of event
         * @param parameter1
         *        the value of the 1st parameter of event, can be null
         * @param parameter2
         *        the value of the 2nd parameter of event, can be null
         * @param value
         *        the value of event
         */
        static void trackEvent(const char* category, const char* name,
                               const char* parameter1, const char* parameter2, int64_t value);

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
         *        the name of the 1st value of event
         * @param value1
         *        the 1st value of event
         * @param value2Name
         *        the name of the 2nd value of event
         * @param value2
         *        the 2nd value of event
         * @param value3Name
         *        the name of the 3rd value of event
         * @param value3
         *        the 3rd value of event
         */
        static void trackEvent(const char* category, const char* name,
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
        static void trackPurchase(const char* productId, const char* currencyCode, double price, const char* campaignId);

        //////////////////////////////////////////////////
        // Exception Tracking
        //////////////////////////////////////////////////

        /**
         * @brief Tracks the given exception.
         * @param exception
         *        the exception to track
         */
        static void trackException(const char* exception);


        //////////////////////////////////////////////////
        // Placement Content
        //////////////////////////////////////////////////

        /**
         * @brief Request a content for the given placement.
         * @param placement
         *        the desired placement
         */
        static void requestPlacementContent(const char* placement);

        /**
         * @brief Checks a content for the given placement is ready to show.
         * @param placement
         *        the desired placement
         * @return true if a content for the given placement is present and ready to
         *         show, or false
         */
        static bool hasPlacementContent(const char* placement);

        /**
         * @brief Shows a content for the given placement.
         * @param placement
         *        the desired placement
         */
        static void showPlacementContent(const char* placement);


        //////////////////////////////////////////////////
        // Callbacks
        //////////////////////////////////////////////////

        /**
         * @brief Sets the callback handler to be notified.
         * @param handler
         *        a pointer to FiveRocksCallbackHandler instance
         */
        static void setCallbackHandler(FiveRocksCallbackHandler* handler);
    };

    /**
     * @brief Callback handler to be notified.
     */
    class FiveRocksCallbackHandler {
    public:

        virtual ~FiveRocksCallbackHandler() {
        }

        /**
         * @brief Called when no content for the given placement exists.
         * @param placement
         *        the placement that was requested
         */
        virtual void onPlacementContentNone(const char* placement) {
        }

        /**
         * @brief Called when a content for the given placement is ready to show.
         * @param placement
         *        the placement that was requested
         */
        virtual void onPlacementContentReady(const char* placement) {
        }

        /**
         * @brief Called when a content for the given placement is showing.
         * @param placement
         *        the placement that was requested
         */
        virtual void onPlacementContentShow(const char* placement) {
        }

        /**
         * @brief Called when a content for the given placement is going to close by user.
         * @param placement
         *        the placement that was requested
         */
        virtual void onPlacementContentClose(const char* placement) {
        }

        /**
         * @brief Called when a content for the given placement has been clicked by user.
         * @param placement
         *        the placement that was requested
         * @param actionRequest
         *        ActionRequest instance if present, or NULL
         */
        virtual void onPlacementContentClick(const char* placement, FiveRocksActionRequest* actionRequest) {
        }

        /**
         * @brief Called when a content for the given placement is dismissed.
         * @param placement
         *        the placement that was requested
         * @param actionRequest
         *        ActionRequest instance if present, or NULL
         */
        virtual void onPlacementContentDismiss(const char* placement, FiveRocksActionRequest* actionRequest) {
        }
    };

    /**
     * @brief Request of an action to be performed.
     */
    class FiveRocksActionRequest {
    public:
        struct Data;
        FiveRocksActionRequest(const Data* data);
        ~FiveRocksActionRequest();

        /**
         * @brief Dispatch this request to the given handler.
         * @param handler
         *        a handler to process the request.
         */
        void dispatchTo(FiveRocksActionRequestHandler* handler);

    private:
        const Data* data;
    };

    /**
     * @brief Handler to process action requests.
     */
    class FiveRocksActionRequestHandler {
    public:
        virtual ~FiveRocksActionRequestHandler() {
        }

        /**
         * @brief Called when a purchase has been requested.
         * @param campaignId
         *        the identifier of the campaign which make this request
         * @param productId
         *        the product identifier (SKU) of the in-app item to purchase
         */
        virtual void onPurchaseRequest(const char* campaignId, const char* productId) {
        }

        /**
         * @brief Called when a purchase has been requested.
         * @param id
         *        the unique identifier of this reward to prevent the reuse attack
         * @param name
         *        the name of the rewarded item
         * @param quantity
         *        the quantity of the rewarded item
         * @param token
         *        the token to verify this reward request
         */
        virtual void onRewardRequest(const char* id, const char* name, int quantity, const char* token) {
        }
    };

} // namespace fiverocks

#endif // fiverocks_FiveRocksCpp_h
