/*
 * CCOpenURLAnd.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: patrick.pamplona
 */

#include "CCOpenURL.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

bool CCOpenURL::canOpenURL(std::string stringURL)
{
	//TODO: Add Can Open URL functionality for Android
	return true;
}

bool CCOpenURL::openURL(std::string stringURL)
{
	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/CheetahHelper",    // fully qualified class name
			"openURL",                              // method name
			"(Ljava/lang/String;)V")) {             // data type of argument

		//jstring - C++ representation of Java String
		jstring stringArg = methodInfo.env->NewStringUTF(stringURL.c_str());

		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringArg);

		methodInfo.env->DeleteLocalRef(stringArg);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);

		return true;
	}

	return false;
}

void CCOpenURL::openAndroidMarket()
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/AndroidHelper",   // fully qualified class name
            "openAndroidMarket",                		   // method name
            "()V")) {                          			   // data type of argument

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}


#endif //End if of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
