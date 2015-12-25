/*
 * CCAlertAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */

#include "CCAlert.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"

CCObject* _target;
SEL_Alert _selector;


void CCAlert::Show(const char* title, const char* text, const char* firstButtonTitle, const char* secondButtonTitle, CCObject* target, SEL_Alert selector)
{
	_target = target;
	_selector = selector;
	if (_target != NULL)
		_target->retain();

	JniMethodInfo methodInfo;

	if (JniHelper::getStaticMethodInfo(methodInfo,
			"com/avalable/cheetah/CheetahHelper",    // fully qualified class name
			"makeAlertDialog",                              // method name
			"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V")) {             // data type of argument

		//jstring - C++ representation of Java String
		jstring stringTitle = methodInfo.env->NewStringUTF(title);
		jstring stringMsg = methodInfo.env->NewStringUTF(text);
		jstring stringBtnTitle1 = methodInfo.env->NewStringUTF(firstButtonTitle);
		jstring stringBtnTitle2 = methodInfo.env->NewStringUTF(secondButtonTitle);

		// call the method, with arguments
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, stringTitle, stringMsg, stringBtnTitle1, stringBtnTitle2);

		methodInfo.env->DeleteLocalRef(stringTitle);
		methodInfo.env->DeleteLocalRef(stringMsg);
		methodInfo.env->DeleteLocalRef(stringBtnTitle1);
		methodInfo.env->DeleteLocalRef(stringBtnTitle2);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}

JNIEXPORT void JNICALL
Java_com_avalable_cheetah_CheetahHelper_onDialogClick(JNIEnv * env, jobject  obj, jint buttonIdx)
{
	CCLog("#######################");
	CCLOG("QQQ Sign In Failed!");
	CCLog("#######################");

	int buttonIndex = (int)buttonIdx;

	if (_target != NULL)
	{
		if (buttonIndex == 0)
		{
			(_target->*_selector)(true);
		}
		else
		{
			(_target->*_selector)(false);
		}
		_target->release();
	}
}

#endif // (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
