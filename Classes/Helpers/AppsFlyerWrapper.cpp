//
//  RatingWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "AppsFlyerWrapper.h"
#include "cocos2d.h"
#include "Settings.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

extern std::string uuid;

void AppsFlyerWrapper::init()
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/ads/AppsFlyer",          // fully qualified class name
			"init",                                         // method name
			"(Ljava/lang/String;Ljava/lang/String;)V")) {   // data type of argument

		//jstring - C++ representation of Java String
		jstring arg0 = methodInfo.env->NewStringUTF(uuid.c_str());
        jstring arg1 = methodInfo.env->NewStringUTF(APP_FLYERS_DEV_KEY);

		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, arg1);

		methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(arg1);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

void AppsFlyerWrapper::setCurrencyCode(const char* currencyCode)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/AppsFlyer",          // fully qualified class name
            "setCurrencyCode",                                         // method name
            "(Ljava/lang/String;)V")) {                     // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(currencyCode);

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0);

        methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void AppsFlyerWrapper::trackEvent(const char* eventName)
{
    AppsFlyerWrapper::trackPurchaseEvent(eventName, "0");
}

void AppsFlyerWrapper::trackPurchaseEvent(const char* eventName, const char* eventValue)
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/AppsFlyer",          // fully qualified class name
            "trackEvent",                                         // method name
            "(Ljava/lang/String;Ljava/lang/String;)V")) {   // data type of argument

        //jstring - C++ representation of Java String
        jstring arg0 = methodInfo.env->NewStringUTF(eventName);
        jstring arg1 = methodInfo.env->NewStringUTF(eventValue);

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, arg0, arg1);

        methodInfo.env->DeleteLocalRef(arg0);
        methodInfo.env->DeleteLocalRef(arg1);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void AppsFlyerWrapper::startSession()
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/ads/AppsFlyer",          // fully qualified class name
            "startSession",                                 // method name
            "()V")) {                                       // data type of argument

        // call the method, with arguments
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
