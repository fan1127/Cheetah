/*
 * PushNotificationsWrapper.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */
#include "PushNotificationsWrapper.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "PushNotifications.h"
#include "platform/android/jni/JniHelper.h"


void PushNotificationsWrapper::RegisterRemoteNotifications()
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"ppl/cocos2dx/ranchrun/pushnotif/PushNotifHelper",    	// fully qualified class name
			"registerRemoteNotifications",                          // method name
			"()V")) {             		   							// data type of argument


		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);

		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}


JNIEXPORT void JNICALL
Java_ppl_cocos2dx_ranchrun_pushnotif_PushNotifHelper_onRegisterPushCallback(JNIEnv * env, jobject  obj, jstring regID)
{
	const char *tmpRegIDString = env->GetStringUTFChars(regID, 0);
	std::string regIDString = (tmpRegIDString);

	PushNotifications::Instance().onRegisteredRemoteNotificationsAndroid(regIDString);

	env->ReleaseStringUTFChars(regID, tmpRegIDString);
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
