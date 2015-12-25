//
//  CCLocalNotification.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/11/2557 BE.
//
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "CCLocalNotification.h"
#include "../../Settings.h"
using namespace std;

void CCLocalNotification::show(string message, int interval, int tag) {
    
    CREATE_CHAR_BUFFER(_buffer, 64);
    snprintf(_buffer, _buffer_size, " (%ds)", interval);
//    message.append(_buffer);
    CCLog_("schedule(%d): %s ... in %d[s]", tag, message.c_str(), interval);
    
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    
    notification.fireDate = [[NSDate date] dateByAddingTimeInterval:interval];
    notification.timeZone = [NSTimeZone systemTimeZone];//[NSTimeZone defaultTimeZone];
    notification.alertBody = [NSString stringWithCString:message.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
    notification.alertAction = @"Open";
    notification.soundName = UILocalNotificationDefaultSoundName;
    
    NSNumber* tag1 = [NSNumber numberWithInteger:tag];
    NSDictionary *infoDict = [NSDictionary dictionaryWithObject:tag1 forKey:@"ID"];
    notification.userInfo = infoDict;
    
    [[UIApplication sharedApplication] scheduleLocalNotification:notification];
    
    [notification release];
    
}

void CCLocalNotification::cancel(int tag) {
    
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        if([[notification.userInfo objectForKey:@"ID"] integerValue] == tag) {
            [[UIApplication sharedApplication] cancelLocalNotification:notification];
        }
    }
    
}

void CCLocalNotification::cancelAll() {
    
    CCLog_("Clear All Local Notification.\n");
//    [[UIApplication sharedApplication] cancelLocalNotificationByUserInfo];
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        [[UIApplication sharedApplication] cancelLocalNotification:notification];
    }
    
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS


