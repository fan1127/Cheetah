//
//  CCLocalNotificationAndroid.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 4/11/2557 BE.
//
//


#include "CCLocalNotification.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

// Java class
// NativeActivity
#define  CLASS_NAME "ppl/cocos2dx/ranchrun/Cheetah"

static bool getJNIStaticMethodInfo(JniMethodInfo &methodinfo,
                                   const char *methodName,
                                   const char *paramCode)
{
	return JniHelper::getStaticMethodInfo(methodinfo,
                                                   CLASS_NAME,
                                                   methodName,
                                                   paramCode);

	return true;
}

void CCLocalNotification::show(std::string message, int interval, int tag)
{
    JniMethodInfo methodInfo;
    
    if (! getJNIStaticMethodInfo(methodInfo, "showLocalNotification", "(Ljava/lang/String;II)V")) {
        return;
    }

    jstring stringArg = methodInfo.env->NewStringUTF(message.c_str());
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg, interval, tag);
    methodInfo.env->DeleteLocalRef(stringArg);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);

}

void CCLocalNotification::cancel(int tag)
{
    JniMethodInfo methodInfo;
    
    if (! getJNIStaticMethodInfo(methodInfo, "cancelLocalNotification", "(I)V")) {
        return;
    }
    
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, tag);
    methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

void CCLocalNotification::cancelAll() {

	JniMethodInfo methodInfo;

	if (! getJNIStaticMethodInfo(methodInfo, "cancelAllLocalNotifications", "()V")) {
		return;
	}

	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
	methodInfo.env->DeleteLocalRef(methodInfo.classID);
}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
