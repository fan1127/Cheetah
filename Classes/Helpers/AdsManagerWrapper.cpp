//
//  RatingWrapper.cpp
//  Cheetah
//
//  Created by Vaclav Samec on 11/12/2557 BE.
//
//

#include "AdsManagerWrapper.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

void AdsManagerWrapper::showInterstitial()
{
    JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/ads/Chartbooster",
			"showInterstitial",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void AdsManagerWrapper::cacheInterstitial()
{
    JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/ads/Chartbooster",
			"cacheInterstitial",
			"()V")) {

		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
