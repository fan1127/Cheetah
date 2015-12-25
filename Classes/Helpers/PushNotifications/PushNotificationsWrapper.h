//
//  PushNotificationsWrapper.h
//  Cheetah
//
//  Created by Vaclav Samec on 11/24/2557 BE.
//
//

#ifndef __Cheetah__PushNotificationsWrapper__
#define __Cheetah__PushNotificationsWrapper__

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

USING_NS_CC;

class PushNotificationsWrapper
{
public:

    void RegisterRemoteNotifications();
};

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

extern "C" {

JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_pushnotif_PushNotifHelper_onRegisterPushCallback(JNIEnv * env, jobject  obj, jstring regID);

}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#endif /* defined(__Cheetah__PushNotificationsWrapper__) */
