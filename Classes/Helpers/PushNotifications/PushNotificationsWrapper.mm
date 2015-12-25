//
//  PushNotificationsWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/24/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "PushNotificationsWrapper.h"


void PushNotificationsWrapper::RegisterRemoteNotifications()
{
    //TODO: use this code when Jenkin's system is upgraded to XCode 6.1

    //-- Set Notification
    if ([[UIApplication sharedApplication] respondsToSelector:@selector(isRegisteredForRemoteNotifications)])
    {
        // iOS 8 Notifications
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    }
    else
    {
        // iOS < 8 Notifications
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes: (UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeSound)];
    }
}


#endif
