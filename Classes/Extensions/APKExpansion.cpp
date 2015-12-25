//
// Created by Vaclav Samec on 12/24/14 AD.
//

#include "APKExpansion.h"

#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

bool APKExpansion::isExpansionInstalled()
{
    JniMethodInfo methodInfo;
    bool ret = false;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/apkexpansion/ExpansionFileManager",   // fully qualified class name
            "isExpansionInstalled",                                      // method name
            "()Z")) {                                                    // data type of argument

        jboolean val = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        ret = val;

        methodInfo.env->DeleteLocalRef(methodInfo.classID);

        return ret;
    }

    return ret;
}

std::string APKExpansion::getInstallationDir()
{
	JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/apkexpansion/ExpansionFileManager",   // fully qualified class name
            "getInstallationDir",                                        // method name
            "()Ljava/lang/String;")) {                                   // data type of argument

        jstring retFromJava = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        const char* str = methodInfo.env->GetStringUTFChars(retFromJava, 0);
        ret = str;

        methodInfo.env->ReleaseStringUTFChars(retFromJava, str);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);

        return ret;
    }

    return ret;
}

void APKExpansion::hideSplashscreen()
{
    JniMethodInfo methodInfo;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/Cheetah",   // fully qualified class name
            "hideSplashscreen",                // method name
            "()V")) {                          // data type of argument

        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

int APKExpansion::getScreenWidth()
{
    JniMethodInfo methodInfo;
    int ret = 0;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getWidth",                                  // method name
            "()I")) {                                   // data type of argument

        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }

    return ret;
}

int APKExpansion::getScreenHeight()
{
    JniMethodInfo methodInfo;
    int ret = 0;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getHeight",                                 // method name
            "()I")) {                                   // data type of argument

        ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return ret;
    }

    return ret;
}

#endif //End if of (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
