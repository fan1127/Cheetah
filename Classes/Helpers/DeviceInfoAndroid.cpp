/*
 * DeviceInfoAndroid.cpp
 *
 *  Created on: Dec 4, 2014
 *      Author: kristian.mitra
 */
#include "DeviceInfo.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include "platform/android/jni/JniHelper.h"

using namespace cocos2d;

std::string DeviceInfo::getName()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getName",                                  // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

	return ret;
}

std::string DeviceInfo::getModel()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getModel",                                 // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

	return ret;
}

bool DeviceInfo::isTablet()
{
    JniMethodInfo methodInfo;
    bool ret = false;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "isTabletDevice",                           // method name
            "()Z")) {                                   // data type of argument

        ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    return ret;
}

int DeviceInfo::getDPI()
{
    JniMethodInfo methodInfo;
    int val=0;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getDPI",                                   // method name
            "()I")) {                                   // data type of argument

        val = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    return val;
}

std::string DeviceInfo::getSystemName()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getSystemName",                            // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

	return ret;
}

std::string DeviceInfo::getSystemVersion()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getSystemVersion",                         // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

	return ret;
}

std::string DeviceInfo::getLanguage()
{
    JniMethodInfo methodInfo;
    std::string ret;

    if (JniHelper::getStaticMethodInfo(methodInfo,
            "ppl/cocos2dx/ranchrun/utils/DeviceInfo",   // fully qualified class name
            "getLanguage",                              // method name
            "()Ljava/lang/String;")) {                  // data type of argument

        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        ret = JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }

	return ret;
}

#endif //(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
