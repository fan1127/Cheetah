//
//  GaiCpp.h
//  Cheetah
//
//  Created by Tanapon Petapanpiboon on 8/1/2557 BE.
//
//

#ifndef __Cheetah__GaiCpp__
#define __Cheetah__GaiCpp__

#include <iostream>
#include "cocos2d.h"



class GaiCpp {
    
    
public:
    
//    static GAI* sharedInstance(void);
    
    
    /*!
     Creates or retrieves a GAITracker implementation with name equal to
     the specified tracking ID. If the tracker for the respective name does not
     already exist, it is created, has it's tracking ID set to |trackingId|,
     and is returned; otherwise, the existing tracker is returned. If the existing
     tracker for the respective name has a different tracking ID, that tracking ID
     is not changed by this method. If defaultTracker is not set, it is set to the
     tracker instance returned here.
     
     @param trackingID The tracking ID to use for this tracker.  It should be of
     the form `UA-xxxxx-y`. The name of the tracker will be the same as trackingID.
     
     @return A GAITracker associated with the specified trackingID. The tracker
     can be used to send tracking data to Google Analytics. The first time this
     method is called with a particular trackingID, the tracker for the respective
     name will be returned, and subsequent calls with the same trackingID
     will return the same instance. It is not necessary to retain the tracker
     because the tracker will be retained internally by the library.
     
     If an error occurs or the trackingId is not valid, this method will return
     `nil`.
     */
//    - (id<GAITracker>)trackerWithTrackingId:(NSString *)trackingId;
    
    static void trackerWithTrackingId(const char* trackingId);
    
    static void reportToACTConversionReporter();
    
//    static void setUserID(const char* userID);
    
    static void reportScreenView(const char* screenName);
    
//    The SDK provides a dryRun flag that when set, prevents any data from being sent to Google Analytics. The dryRun flag should be set whenever you are testing or debugging an implementation and do not want test data to appear in your Google Analytics reports.
    static void setDryRun(bool dryRun);
    
    
    static void setLogLevelVerbose();
    
    static void setDispatchInterval(double timeInterval);
    
    static void reportPurchase(const char* productIdentifier,
                               const char* localizedTitle,
                               const char* productCategory,
                               float price,
                               const char* currencyCode);
};

#endif /* defined(__Cheetah__GaiCpp__) */
