//
//  RatingWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "RatingWrapper.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

void RatingWrapper::init()
{
    // init is done in Cheetah.java: onCreate
}

bool RatingWrapper::canRate()
{
/*
#ifdef RATE_ALWAYS
    return true;
#else
    return [Appirater canRate];
#endif
*/

    JniMethodInfo methodInfo;
    bool ret = false;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "com/sbstrm/appirater/Appirater",          // fully qualified class name
            "canRate",                                 // method name
            "()Z")) {                                  // data type of argument

        jboolean val = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        ret = val;

        methodInfo.env->DeleteLocalRef(methodInfo.classID);

        return ret;
    }

    return ret;
}

void RatingWrapper::rateApp()
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "com/sbstrm/appirater/Appirater",          // fully qualified class name
            "onRate",                                  // method name
            "()V")) {                                  // data type of argument

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void RatingWrapper::dontRateApp()
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "com/sbstrm/appirater/Appirater",          // fully qualified class name
            "onDontRate",                              // method name
            "()V")) {                                  // data type of argument

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void RatingWrapper::onEnterForeground()
{
	// not present in android port
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
